#include <iostream>
#include<string>
#include <fstream>
#include <windows.h>
#include <vector>
#include <time.h>

#define COMPUTER 0
#define HUMAN 1
#define BOARDSIZE 8
#define INVALID -1

#define COLORX 12
#define COLORO 10
#define COLORRESET 7

#define PLAYERX '#'
#define PLAYERO 'O'


using namespace std;


// Player 1 is X, player 2 is O
static char board[BOARDSIZE][BOARDSIZE];
static int turn;
static float timeLimit;

static int playerX;
static int playerO;

// static bool gameEnd;

HANDLE hConsole;
int validMoves[64][2];
static int simValidMoves[64][2];
static int simNMoves = 0;
int nMoves = 0;

time_t startTime;
time_t endTime;

bool ended = false;

void drawBoard(){
    cout<< "      1   2   3   4   5   6   7   8 " << endl;
    cout<< "    ---------------------------------"<<endl;
    for (int row = 0; row < BOARDSIZE; row++){
        cout << row + 1 << "   |";
        for (int column = 0; column < BOARDSIZE; column++){
            if(board[row][column] == PLAYERX){
                SetConsoleTextAttribute(hConsole, COLORX);
            }else if (board[row][column] == PLAYERO){
                SetConsoleTextAttribute(hConsole, COLORO);
            }

            cout <<' '<< board[row][column] ;
            SetConsoleTextAttribute(hConsole, COLORRESET);
            cout << ' '<< "|";
        }
        cout<< endl<< "    ---------------------------------"<<endl;
    }
}

void initializeBoard(){
    for (int row = 0; row < BOARDSIZE; row++){
        for (int column = 0; column < BOARDSIZE; column++){
            board[row][column] = ' ';
        }
    }

    board[3][3] = PLAYERO;
    board[3][4] = PLAYERX;
    board[4][3] = PLAYERX;
    board[4][4] = PLAYERO;
}

// Load board from a given file and play based on it
void loadBoard(string fileName){
    ifstream file;
    string line;
    int lineNumber = 1;
    int row = 0;
    int column = 0;

    file.open(fileName);
    
    if(file.is_open()){
        while(getline(file, line)){
            if(row < 8){
                for(int i = 0; i < sizeof(line); i++){
                    switch (line[i]){
                        case '1':
                            board[row][column] = PLAYERX;
                            column++;
                            break;
                        case '2':
                            board[row][column] = PLAYERO;
                            column++;
                            break;
                        case '0':
                            board[row][column] = ' ';
                            column++;
                            break;
                        default:
                            break;
                    }   
                }
                row++;
                column = 0;
            }

            if(lineNumber == 9){
                turn = line[0];
            }

            if(lineNumber == 10){
                timeLimit = line[0];
            }

            lineNumber++;
        }

        file.close();
    }else{
        cout << "Unable to open the file!" << endl;
        initializeBoard();
    } 

}

void initialAttempt(){
    char loadFromFile;
    string fileName;
    string player1Role;
    string player2Role;
    string timeLimitValue;
    
    // Initialize console color
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, COLORRESET);
    
    cout << "Welcome to the Othello paradise!" << endl;
    cout << "Do you want to load the Board from an file?(Y/N)"<< endl;
    cin >> loadFromFile;
    if(loadFromFile == 'Y'){
        cout <<"Enter the file name:" << endl;
        cin >> fileName;
        loadBoard(fileName);
    }else if (loadFromFile == 'N'){
        initializeBoard();
    }else{
        cout << "Invalid input, load from the start." << endl;
        initializeBoard();
    }

    cout << "Player for X? 1.you 2.Computer (1/2)" << endl;
    cin >> player1Role;
    if(player1Role == "1"){
        playerX = HUMAN;
    }else{
        // If user entered invalid then playerX is the computer by default
        playerX = COMPUTER;
    }

    cout << "Player for O? 1.you 2.Computer (1/2)" << endl;
    cin >> player2Role;
    if(player2Role == "1"){
        playerO = HUMAN;
    }else{
        // If user entered invalid then playerO is the computer by default
        playerO = COMPUTER;
    }

    cout << "Enter the time limit in second for each game round:" << endl;
    cin >> timeLimitValue;

    if(stof(timeLimitValue) > 100 || stoi(timeLimitValue) < 0.0001){
        cout << "Invalid time limit value, time limit set to 5s by default";
        timeLimit = 5;
    }else{
        timeLimit = stof(timeLimitValue);
    }

     cout << "TURN for "<< PLAYERX << ":" << endl;
    drawBoard();
}

bool addValidPosition(int row, int column, bool simulation = false){
    int (*mMoves)[64][2];
    int *mNMoves;

    if(simulation){
        mMoves = &simValidMoves;
        mNMoves = &simNMoves; 
    }else{
        mMoves = &validMoves;
        mNMoves = &nMoves;
    }


    for(int i = 0; i < (*mNMoves); i++){
        if((*mMoves)[i][0] == row && (*mMoves)[i][1]  == column){
            // A repeated solution
            return false;            
        }
    }

    (*mMoves)[(*mNMoves)][0] = row;
    (*mMoves)[(*mNMoves)][1] = column;
                        
    (*mNMoves)++;
    return true;
}


void checkHorizontal(char mBoard[8][8] ,char playerSymbol, char oppoSymbol, bool simulation){
    int tempStart[] = {INVALID,INVALID};
    int tempEnd[] = {INVALID,INVALID};

    for (int row = 0; row < BOARDSIZE; row++){
        for (int column = 0; column < BOARDSIZE; column++){
            if(mBoard[row][column] == playerSymbol){
                tempStart[0] = row;
                tempStart[1] = column;
            }

            if(column -1 >= 0){
                if(mBoard[row][column] == oppoSymbol && mBoard[row][column-1] == ' '){
                    tempEnd[0] = row;
                    tempEnd[1] = column-1;
                }
            }
            
            
            if(mBoard[row][column] == ' '){

                if(tempStart[0] != INVALID){
                     // empty not directly after the player symbol
                    if(abs( column - tempStart[1]) !=1){

                        addValidPosition(row, column, simulation);
                        
                    }
                    
                }
                
                tempStart[0] = INVALID;
                tempStart[1] = INVALID;

                tempEnd[0] = INVALID;
                tempEnd[1] = INVALID;
               
            }

            if(tempEnd[0]!= INVALID && mBoard[row][column] == playerSymbol){
                addValidPosition(tempEnd[0], tempEnd[1], simulation);

                tempEnd[0] = INVALID;
                tempEnd[1] = INVALID;
            }
        }
        
        // Would not be valid for next row
        tempStart[0] = INVALID;
        tempStart[1] = INVALID;

        tempEnd[0] = INVALID;
        tempEnd[1] = INVALID;
    }
}

// Just the opposite of horizontal
void checkVertical(char mBoard[8][8], char playerSymbol, char oppoSymbol, bool simulation){
    int tempStart[] = {INVALID,INVALID};
    int tempEnd[] = {INVALID,INVALID};
    

    for (int column = 0; column < BOARDSIZE; column++){
        for (int row = 0; row < BOARDSIZE; row++){
            if(mBoard[row][column] == playerSymbol){
                tempStart[0] = row;
                tempStart[1] = column;
            }
           
            if(row - 1 >= 0){
                if(mBoard[row][column] == oppoSymbol && mBoard[row-1][column] == ' '){
                    tempEnd[0] = row-1;
                    tempEnd[1] = column;
                }
            }
            
            if(mBoard[row][column] == ' '){
                if(tempStart[0] != INVALID){
                    // empty not directly after the player symbol
                    if(abs(row - tempStart[0]) ==1){
                        tempStart[0] = INVALID;
                        tempStart[1] = INVALID;
                    }else{
                        addValidPosition(row, column, simulation);
                    }
                }

                tempStart[0] = INVALID;
                tempStart[1] = INVALID;

                tempEnd[0] = INVALID;
                tempEnd[1] = INVALID;
            }

            if(tempEnd[0]!= INVALID && mBoard[row][column] == playerSymbol){
                addValidPosition(tempEnd[0], tempEnd[1], simulation);
                
                tempEnd[0] = INVALID;
                tempEnd[1] = INVALID;
            }
        }
        
        // Would not be valid for next collumn
        tempStart[0] = INVALID;
        tempStart[1] = INVALID;

        tempEnd[0] = INVALID;
        tempEnd[1] = INVALID;
        
    }
}

// check whether valid moves in the / diagonal in the matrix
void checkUpDia(char mBoard[8][8], char playerSymbol, char oppoSymbol, bool simulation){
    int tempStart[] = {INVALID,INVALID};
    int tempEnd[] = {INVALID, INVALID};

    for (int k = 2; k <= 12; k++){
        int column = k - 7 > 0 ? k - 7 : 0;
        for (column; column <= k; column++){
            int row = k - column;

            // Might outside the boundary
            if(row < BOARDSIZE && column < BOARDSIZE){
                if(mBoard[row][column] == playerSymbol){
                    tempStart[0] = row;
                    tempStart[1] = column;
                }
           
                if(row + 1 < BOARDSIZE && column - 1 >= 0 ){
                    if(mBoard[row][column] == oppoSymbol && mBoard[row+1][column-1] == ' '){
                        tempEnd[0] = row + 1;
                        tempEnd[1] = column - 1;
                    }
                }
                
                if(mBoard[row][column] == ' '){
                    if(tempStart[0] != INVALID){
                        // empty not directly after the player symbol
                        if(abs(row - tempStart[0]) ==1){
                            tempStart[0] = INVALID;
                            tempStart[1] = INVALID;
                        }else{
                            addValidPosition(row, column, simulation);
                        }
                    }

                    tempStart[0] = INVALID;
                    tempStart[1] = INVALID;

                    tempEnd[0] = INVALID;
                    tempEnd[1] = INVALID;
                }

                if(tempEnd[0]!= INVALID && mBoard[row][column] == playerSymbol){
                    addValidPosition(tempEnd[0], tempEnd[1], simulation);
                    
                    tempEnd[0] = INVALID;
                    tempEnd[1] = INVALID;
                }
            }
        }

        // Would not be valid for next collumn
        tempStart[0] = INVALID;
        tempStart[1] = INVALID;

        tempEnd[0] = INVALID;
        tempEnd[1] = INVALID;
            
    }
}

// check whether valid moves in the \ diagonal in the matrix
void checkDownDia(char mBoard[8][8],char playerSymbol, char oppoSymbol, bool simulation){
    int tempStart[] = {INVALID,INVALID};
    int tempEnd[] = {INVALID, INVALID};

    int column;
    int row;
    int* dir;

    for (int k = 5; k >= -5; k--){
        if(k >= 0){
            column = k;
            dir = &row;
        }else{
            row = abs(k);
            dir = &column;
        }

        for (*dir = 0; column < BOARDSIZE; row++, column++){

            // Might outside the boundary
            if(row < BOARDSIZE && column < BOARDSIZE){
                if(mBoard[row][column] == playerSymbol){
                    tempStart[0] = row;
                    tempStart[1] = column;
                }
           
                if(row - 1 >= 0 && column - 1 >= 0 ){
                    if(mBoard[row][column] == oppoSymbol && mBoard[row-1][column-1] == ' '){
                        tempEnd[0] = row - 1;
                        tempEnd[1] = column - 1;
                    }
                }
                
                if(mBoard[row][column] == ' '){
                    if(tempStart[0] != INVALID){
                        // empty not directly after the player symbol
                        if(abs(row - tempStart[0]) ==1){
                            tempStart[0] = INVALID;
                            tempStart[1] = INVALID;
                        }else{
                            addValidPosition(row, column, simulation);
                        }
                    }

                    tempStart[0] = INVALID;
                    tempStart[1] = INVALID;

                    tempEnd[0] = INVALID;
                    tempEnd[1] = INVALID;
                }

                if(tempEnd[0]!= INVALID && mBoard[row][column] == playerSymbol){
                    addValidPosition(tempEnd[0], tempEnd[1], simulation);
                    
                    tempEnd[0] = INVALID;
                    tempEnd[1] = INVALID;
                }
            }
        }

        // Would not be valid for next collumn
        tempStart[0] = INVALID;
        tempStart[1] = INVALID;

        tempEnd[0] = INVALID;
        tempEnd[1] = INVALID;
            
    }
}

int calcScore(char symbol){
    int score = 0;

    for(int row = 0; row < BOARDSIZE; row++){
        for(int column = 0; column < BOARDSIZE; column++){
            if(board[row][column] == symbol){
                score++;
            }
        }
    }

    return score;
}


void gameEnd(char winner){
    cout << "Game end!" << endl;
    cout << "The winner is " << winner << "!"<< endl;

    cout << "The score for " << PLAYERX << " is "<< calcScore(PLAYERX)<< endl;
    cout << "The score for " << PLAYERO << " is " << calcScore(PLAYERO)<< endl;

    ended = true;

}

void getValidMoves(char mBoard[BOARDSIZE][BOARDSIZE], char playerSymbol, char oppoSymbol, bool recursive = false, bool simulation=false){
    nMoves = 0;
    checkHorizontal(mBoard, playerSymbol, oppoSymbol, simulation);
    checkVertical(mBoard,playerSymbol, oppoSymbol, simulation);
    checkUpDia(mBoard,playerSymbol, oppoSymbol, simulation);
    checkDownDia(mBoard,playerSymbol, oppoSymbol, simulation);


    if(nMoves == 0 && !recursive && !simulation) {
        getValidMoves(mBoard, oppoSymbol, playerSymbol, true);
    }else if(nMoves == 0 && !simulation){
        // No available moves for both Sides
        calcScore(playerSymbol);
        // TODO:Need to change
        gameEnd(playerX);
    }
}

void flipOthers(int position[2], char mSymbol, char (*mBoard)[8][8]){
    // TODO: This function is a bit redundant

    // Check Flip in the Horizontal
    for(int column = position[1] -1; column >= 0; column --){
        if((*mBoard)[position[0]][column] == mSymbol){
            for( int flipStart = column+1 ; flipStart < position[1]; flipStart++){
                (*mBoard)[position[0]][flipStart] = mSymbol;
            }
            break;
        }else if((*mBoard)[position[0]][column] == ' '){
            break;
        }
    }


    for(int column = position[1] + 1; column < BOARDSIZE; column ++){
        if((*mBoard)[position[0]][column] == mSymbol){
            for( int flipStart = column-1 ; flipStart > position[1]; flipStart--){
                (*mBoard)[position[0]][flipStart] = mSymbol;
            }
            break;
        }else if((*mBoard)[position[0]][column] == ' '){
            break;
        }
    }
    

    // Flip verticals
    for(int row = position[0] -1; row >= 0; row --){
        if((*mBoard)[row][position[1]] == mSymbol){
            for( int flipStart = row+1 ; flipStart < position[0]; flipStart++){
                (*mBoard)[flipStart][position[1]] = mSymbol;
            }
            break;
        }else if((*mBoard)[row][position[1]] == ' '){
            break;
        }
    }


    for(int row = position[0] + 1; row < BOARDSIZE; row ++){
        if((*mBoard)[row][position[1]] == mSymbol){
            for( int flipStart = row-1 ; flipStart > position[0]; flipStart--){
                (*mBoard)[flipStart][position[1]] = mSymbol;
            }
            break;
        }else if((*mBoard)[row][position[1]] == ' '){
            break;
        }
    }

    // Check for \ direction
    for(int row = position[0]+1, column = position[1] + 1; row < BOARDSIZE && column < BOARDSIZE; row++, column++){
        if((*mBoard)[row][column] == mSymbol){
            for(int flipStart[2] = {row-1, column-1}; flipStart[0] > position[0]; flipStart[0]--, flipStart[1]-- ){
                (*mBoard)[flipStart[0]][flipStart[1]] = mSymbol;
            }
            break;
        }else if((*mBoard)[row][column] == ' '){
            break;
        }
    }

    
    for(int row = position[0] - 1,column = position[1] - 1 ; row >= 0 && column >= 0; row--, column--){
        if((*mBoard)[row][column] == mSymbol){
            for(int flipStart[2] = {row+1, column+1}; flipStart[0] < position[0]; flipStart[0]++, flipStart[1]++ ){
                (*mBoard)[flipStart[0]][flipStart[1]] = mSymbol;
            }
            break;
        }else if((*mBoard)[row][column] == ' '){
            break;
        }
    }

    // Check for / direction
    for(int row = position[0]-1, column = position[1] + 1; row >= 0 && column < BOARDSIZE; row--, column++){
        if((*mBoard)[row][column] == mSymbol){
            for(int flipStart[2] = {row+1, column-1}; flipStart[0] < position[0]; flipStart[0]++, flipStart[1]-- ){
                (*mBoard)[flipStart[0]][flipStart[1]] = mSymbol;
            }
            break;
        }else if((*mBoard)[row][column] == ' '){
            break;
        }
    }

    for(int row = position[0] + 1, column = position[1] - 1; row < BOARDSIZE && column >= 0; row++, column--){
        if((*mBoard)[row][column] == mSymbol){
            for(int flipStart[2] = {row-1, column+1}; flipStart[0] > position[0]; flipStart[0]--, flipStart[1]++ ){
                (*mBoard)[flipStart[0]][flipStart[1]] = mSymbol;
            }
            break;
        }else if((*mBoard)[row][column] == ' '){
            break;
        }
    }
    

}

void outputMoves(){
    for(int i = 0; i < nMoves; i++){
        cout << i+1 << ". ("<< validMoves[i][0]+1<< "," << validMoves[i][1]+1<< ")"<< endl;
    }
}

void getHumanAction(char symbol){
    outputMoves();
    
    int moveChosen;

    if(nMoves == 0){
        cout << "There are no available moves. Press any to skip the turn"<< endl;
        cin >> moveChosen;

        return;
    }

    while(true){
        cout << "Which Position do you want to place?" << endl;
        cin >> moveChosen;
        if(moveChosen <= nMoves && moveChosen > 0 ){
            int targetPosi[2] =  {validMoves[moveChosen-1][0], validMoves[moveChosen-1][1]};         
            board[targetPosi[0]][targetPosi[1]] = symbol;

            flipOthers(targetPosi, symbol, &board);
            
            return;
        }
    }

}



bool cutOff(){
    // time limit or depth limit
    // When hit the limit, should choose the move dtermined by the deepest complted search
    time(&endTime);
    cout << difftime(startTime, endTime) << endl;
    if( abs(difftime(startTime, endTime)) > 10){
        return true;
    }
    
    return false;

}

// The evaluation function as the heuristic
int heuristic(char symbol){
    return calcScore(symbol);

}

char getOppoSymbol(char symbol){
    if(symbol != PLAYERX){
        return PLAYERX;
    }else{
        return PLAYERO;
    }
}

int* minValue(char (*mBoard)[8][8], int alpha, int beta, char symbol);
void boardCopy(char (*destBoard)[8][8], char (*sourceBoard)[8][8]){
    for(int row = 0; row < BOARDSIZE; row++){
        for(int column = 0; column < BOARDSIZE; column++){
            (*destBoard)[row][column] = (*sourceBoard)[row][column];
        }
    }
}

int* maxValue(char (*mBoard)[8][8], int alpha, int beta, char symbol){
    
    int value_Move_Pair[2];
    
    // From the minValue function to compare
    int* value_Move_Pair2;

    // board represent the current state
    char tempBoard[8][8];
    int v = -INFINITE;
    int a;


    boardCopy(&tempBoard, mBoard);
    for (int row = 0; row < BOARDSIZE; row++){
        for (int column = 0; column < BOARDSIZE; column++){
            cout << tempBoard[row][column];
        }
        cout << endl;
    }

    if(cutOff()){
        value_Move_Pair[0] = heuristic(symbol);
        value_Move_Pair[1] = INVALID;
        return &value_Move_Pair[0];
    }
    getValidMoves(tempBoard, symbol,getOppoSymbol(symbol),false,true);

    int mMoves[simNMoves][2];
    for(int i = 0; i < simNMoves; i++){
        mMoves[i][0] = simValidMoves[i][0];
        mMoves[i][1] = simValidMoves[i][1];
    }
    

    for (int i = 0; i < simNMoves; i++){
        
        flipOthers(mMoves[i],symbol, &tempBoard);
        value_Move_Pair2 = minValue(&tempBoard,alpha,beta,getOppoSymbol(symbol));

        if(value_Move_Pair2[0] > v){
            v = value_Move_Pair2[0];
            a = i;
            alpha = max(alpha, v);
        }
        if( v >= beta){
            value_Move_Pair[0] = v;
            value_Move_Pair[1] = a;
            return value_Move_Pair;
        }
    }

    return &value_Move_Pair[0];
    
}

int* minValue(char (*mBoard)[8][8], int alpha, int beta, char symbol){
    // Terminal state is already handled in the getValidMoves function
    // TODO is cutoff
    cout << "min" << endl;

    int value_Move_Pair[2];
    
    // From the minValue function to compare
    int* value_Move_Pair2;

    char tempBoard[8][8];
    int v = INFINITE;
    int a;

    if(cutOff()){
        value_Move_Pair[0] = heuristic(symbol);
        value_Move_Pair[1] = INVALID;
        return &value_Move_Pair[0];
    }

    memcpy(tempBoard, (*mBoard), BOARDSIZE*BOARDSIZE*sizeof(*mBoard));

    getValidMoves(tempBoard, symbol,getOppoSymbol(symbol),false,true);

    int mMoves[simNMoves][2];
    for(int i = 0; i < simNMoves; i++){
        mMoves[i][0] = simValidMoves[i][0];
        mMoves[i][1] = simValidMoves[i][1];
    }

    for (int i = 0; i < simNMoves; i++){
    
        flipOthers(mMoves[i],symbol, &tempBoard);
        value_Move_Pair2 = maxValue(&tempBoard,alpha,beta,getOppoSymbol(symbol));

        if(value_Move_Pair2[0] < v){
            v = value_Move_Pair2[0];
            a = i;
            alpha = max(beta, v);
        }
        if( v <= alpha){
            value_Move_Pair[0] = v;
            value_Move_Pair[1] = a;
            return value_Move_Pair;
        }
    }

    return &value_Move_Pair[0];
    
}



int alphaBetaSearch(char symbol){

    int moveChosen;

    char tempboard[BOARDSIZE][BOARDSIZE];
    boardCopy(&tempboard, &board);

    moveChosen = maxValue(&tempboard, -INFINITE, INFINITE, symbol)[0];

    return moveChosen;

}

void getComputerAction(char symbol){
    
    time(&startTime);
    cout << "Time spent on searching:" << endl;
    alphaBetaSearch(symbol);
    cout << "Max depth have been searched:" << endl;

    // TODO: if the time limit caused the next depth limit to be cut off after a partial search,
    // Display the information as well
    // If half of the time limit is used up after a search to some specific depth limit

}


void gameCoreLoop(){
    
    if(ended) return;
    
    if(playerX == HUMAN){
        getValidMoves(board, PLAYERX, PLAYERO);
        getHumanAction(PLAYERX);
    }else{
        getComputerAction(PLAYERX);
    }

    cout << "TURN for "<< PLAYERO << ":" << endl;
    drawBoard();

   
    if(ended) return;

    if(playerO == HUMAN){
        getValidMoves(board, PLAYERO, PLAYERX); 
        getHumanAction(PLAYERO);
    }else{
        getComputerAction(PLAYERO);
    }

    cout << "TURN for "<< PLAYERX << ":" << endl;
    drawBoard();
}

void gameSession(){
    while(!ended){
        gameCoreLoop();
    }
    
}

int main(){
    string toEnd;

    initialAttempt();
    gameSession();
    
    cout << "Enter anything to exit" << endl;
    cin >> toEnd;
    return 0;
}

