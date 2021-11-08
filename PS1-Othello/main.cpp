#include <iostream>
#include<string>
#include <fstream>
#include <windows.h>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <ctime> 

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
static int tempNmoves = INVALID;
int nMoves = 0;

time_t startTime;
time_t endTime;
double timeUsed;
int searchDepth = -1;
int searchedMax = 0;
bool partialSearched = false;

bool ended = false;

// A weight grid for the board from https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.108.5111&rep=rep1&type=pdf
int weights[8][8] = {{100,-25,10,5,5,10,-25,100},
                     {-25,-25,1,1,1,1,-25,-25},
                     {10,1,5,2,2,5,1,10},
                     {5,1,2,1,1,2,1,5},
                     {5,1,2,1,1,2,1,5},
                     {10,1,5,2,2,5,1,10},
                     {-25,-25,1,1,1,1,-25,-25},
                     {100,-25,10,5,5,10,-25,100}};

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

int calcScore( char mBoard[8][8],char symbol){
    int score = 0;

    for(int row = 0; row < BOARDSIZE; row++){
        for(int column = 0; column < BOARDSIZE; column++){
            if(mBoard[row][column] == symbol){
                score++;
            }
        }
    }

    return score;
}


void gameEnd(){

    int playerX_Score = calcScore(board,PLAYERX);
    int playerO_Score = calcScore(board,PLAYERO);
    char winner;
    
    cout << "Game end!" << endl;
    
    if(playerX_Score > playerO_Score){
        winner = PLAYERX;      
        cout << "The winner is " << winner << "!"<< endl;
    }else if(playerX_Score != playerO_Score){
        winner = PLAYERO;
        cout << "The winner is " << winner << "!"<< endl;
    }else{
        cout << "The game is a Draw" << endl;
    }


    cout << "The score for " << PLAYERX << " is "<< calcScore(board,PLAYERX)<< endl;
    cout << "The score for " << PLAYERO << " is " << calcScore(board,PLAYERO)<< endl;    

    ended = true;

}

void getValidMoves(char mBoard[BOARDSIZE][BOARDSIZE], char playerSymbol, char oppoSymbol, bool recursive = false, bool simulation=false){
    nMoves = 0;
    simNMoves = 0;
    checkHorizontal(mBoard, playerSymbol, oppoSymbol, simulation);
    checkVertical(mBoard,playerSymbol, oppoSymbol, simulation);
    checkUpDia(mBoard,playerSymbol, oppoSymbol, simulation);
    checkDownDia(mBoard,playerSymbol, oppoSymbol, simulation);


    if(nMoves == 0 && !recursive && !simulation) {
        getValidMoves(mBoard, oppoSymbol, playerSymbol, true);
    }else if(nMoves == 0 && !simulation){
        // No available moves for both Sides
        calcScore(board, playerSymbol);
        gameEnd();
    }
}

void flipOthers(int position[2], char mSymbol, char (*mBoard)[8][8]){
    //This function is a bit redundant

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

    if(ended){
        return;
    }

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

char getOppoSymbol(char symbol){
    if(symbol != PLAYERX){
        return PLAYERX;
    }else{
        return PLAYERO;
    }
}


bool cutOff(char mBoard[8][8], char symbol, int depthLimit){

    time(&endTime);
    timeUsed = abs(difftime(startTime, endTime));

    if(searchDepth > searchedMax){
        searchedMax = searchDepth;
    }

    if( timeUsed >= timeLimit){
        if(!partialSearched){
            cout << "Partial searched to depth:" << searchedMax << endl;
            partialSearched = true;
        }
        return true;
    }


    if(searchDepth > depthLimit){
        return true;
    }

    // When the terminal state is reached
    if(simNMoves == 0){
        getValidMoves(mBoard,getOppoSymbol(symbol), symbol,false, true);
        if(simNMoves == 0){
            return true;
        }else{
            return false;
        }
    }

    return false;

}



// The evaluation function
int evaluation(char mBoard[8][8], char symbol){
    int mScore = 0;
    float totalMoves = 0;
    float totalMobility = 0;
    float moveScore = 0;

    float mobilities = 0;
    float mobilityScore = 0;
    float totalWeights = 0;
    float mWeights = 0;
    float weightScore = 0;

    totalMoves = calcScore(mBoard, symbol) + calcScore(mBoard, getOppoSymbol(symbol));
    
    moveScore = (calcScore(mBoard, symbol) - calcScore(mBoard, getOppoSymbol(symbol)))/totalMoves;



    // Evaluate mobility
    getValidMoves(mBoard,symbol,getOppoSymbol(symbol),false,true);

    mobilities += simNMoves;
    totalMobility += simNMoves;

    getValidMoves(mBoard,getOppoSymbol(symbol),symbol,false,true);

    mobilities -= simNMoves;
    totalMobility+=simNMoves;
    
    mobilityScore = mobilities/totalMobility;

    // Apply Sweet 16
    if(totalMobility < 16){
        for(int row = 2; row < 6; row++){
            for(int column = 2; column < 6; column++){
                if(mBoard[row][column] == symbol){
                    mWeights += 20;
                    totalWeights += 20;
                }
                if(mBoard[row][column] == getOppoSymbol(symbol)){
                    mWeights -= 20;
                    totalWeights += 20;
                }
            }
        }

    }

    // Using the weight matrix
    for(int row = 0; row < BOARDSIZE; row++){
        for(int column = 0; column < BOARDSIZE; column++){
            if(mBoard[row][column] == symbol){
                mWeights += weights[row][column];
                totalWeights += abs(weights[row][column]);
            }

            if(mBoard[row][column] == getOppoSymbol(symbol)){
                mWeights -= weights[row][column];
                totalWeights += abs(weights[row][column]);
            }
        }
    }

    
    weightScore = mWeights/totalWeights;

    mScore = int((moveScore*4 + mobilityScore*6 + weightScore*90)*100);

    return mScore;

}



int* minValue(char (*mBoard)[8][8], int alpha, int beta, char symbol, int depthLimit);
void boardCopy(char (*destBoard)[8][8], char (*sourceBoard)[8][8]){
    for(int row = 0; row < BOARDSIZE; row++){
        for(int column = 0; column < BOARDSIZE; column++){
            (*destBoard)[row][column] = (*sourceBoard)[row][column];
        }
    }
}

int* maxValue(char (*mBoard)[8][8], int alpha, int beta, char symbol, int depthLimit){    

    // board represent the current state
    int* value_Move_Pair = new int[2];
    int* value_Move_Pair2 = new int[2];
    char tempBoard[8][8];
    int v = -10000;
    int a = 0;

    searchDepth++;

    boardCopy(&tempBoard, mBoard);
    getValidMoves(tempBoard, symbol,getOppoSymbol(symbol),false,true);

    if(cutOff(tempBoard, symbol, depthLimit)){
        *value_Move_Pair= evaluation(tempBoard, symbol);
        *(value_Move_Pair + 1) = INVALID;
        searchDepth--;
        return value_Move_Pair;
    }
   

    int mMoves[simNMoves][2];
    int mNMoves = simNMoves;
    for(int i = 0; i < simNMoves; i++){
        mMoves[i][0] = simValidMoves[i][0];
        mMoves[i][1] = simValidMoves[i][1];
    }
    

    for (int i = 0; i < mNMoves; i++){
        char subBoard[8][8];
        boardCopy(&subBoard, &tempBoard);
        subBoard[mMoves[i][0]][mMoves[i][1]] = symbol;
        flipOthers(mMoves[i],symbol, &subBoard);

        value_Move_Pair2 = minValue(&subBoard,alpha,beta,getOppoSymbol(symbol),depthLimit);
        if(*value_Move_Pair2 == v){
            int chose;
            // Switch to the new value or not
            chose = rand()%(2);

            // chose between original a or i
            a = a*chose + (1-chose)*i;
        }
        
        if(*value_Move_Pair2 > v){
            v = *value_Move_Pair2;
            a = i;
            alpha = max(alpha, v);
        }

      

        if( v >= beta){
            *value_Move_Pair = v;
            *(value_Move_Pair+1) = a;
            searchDepth--;
            return value_Move_Pair;
        }
    }

    *value_Move_Pair = v;
    *(value_Move_Pair+1) = a;

    searchDepth--;
    return value_Move_Pair;
    
}

int* minValue(char (*mBoard)[8][8], int alpha, int beta, char symbol, int depthLimit){

    int* value_Move_Pair = new int[2];
    int* value_Move_Pair2 = new int[2];
    

    char tempBoard[8][8];
    int v = 10000;
    int a;

    searchDepth++;

    boardCopy(&tempBoard, mBoard);

    if(cutOff(tempBoard, symbol, depthLimit)){
        *value_Move_Pair = evaluation(tempBoard, getOppoSymbol(symbol));
        *(value_Move_Pair + 1) = INVALID;
        searchDepth--;
        return value_Move_Pair;
    }

    getValidMoves(tempBoard, symbol,getOppoSymbol(symbol),false,true);

    int mMoves[simNMoves][2];
    int mNMoves = simNMoves;
    for(int i = 0; i < mNMoves; i++){
        mMoves[i][0] = simValidMoves[i][0];
        mMoves[i][1] = simValidMoves[i][1];
    }

    for (int i = 0; i < mNMoves; i++){
        char subBoard[8][8];
        boardCopy(&subBoard, &tempBoard);
        subBoard[mMoves[i][0]][mMoves[i][1]] = symbol;
        flipOthers(mMoves[i],symbol, &subBoard);

        value_Move_Pair2 = maxValue(&subBoard,alpha,beta,getOppoSymbol(symbol), depthLimit);
        if(*value_Move_Pair2 == v){
            int chose;
            // Switch to the new value or not
            chose = rand()%(2);

            // chose between original a or i
            a = a*chose + (1-chose)*i;
        }


        if(*value_Move_Pair2 < v){
            v = *value_Move_Pair2;
            a = i;
            alpha = max(beta, v);
        }

        
        if( v <= alpha){
            *value_Move_Pair = v;
            *(value_Move_Pair+1) = a;
            searchDepth--;
            return value_Move_Pair;
        }
    }

    *value_Move_Pair = v;
    *(value_Move_Pair+1) = a;

    searchDepth--;
    return value_Move_Pair;
    
}



int alphaBetaSearch(char symbol, int depthLimit){

    int moveChosen;
    searchDepth = -1;
    searchedMax = 0;
    partialSearched = false;

    char tempboard[BOARDSIZE][BOARDSIZE];
    boardCopy(&tempboard, &board);

    moveChosen = *(maxValue(&tempboard, -10000, 10000, symbol, depthLimit)+1);

    return moveChosen;

}

void getComputerAction(char symbol){

    int tempMoveChosen;
    int moveChosen = INVALID;
    int targetPosi[2];
    time(&startTime);
    if(ended){
        return;
    }
    
    for(int depthLimit = 7; depthLimit < 50; depthLimit++){
        time(&endTime);
        timeUsed = abs(difftime(startTime, endTime));
        if(timeUsed >= timeLimit){
            break;
        }
        
        tempMoveChosen = alphaBetaSearch(symbol, depthLimit);
        if(!partialSearched){
            moveChosen = tempMoveChosen;
        }else{
            searchedMax = searchedMax -1;
        }
    }

    getValidMoves(board,symbol, getOppoSymbol(symbol));

    // In case none of the search finishes
    if(moveChosen = INVALID){
        moveChosen = rand()%(nMoves);
    }

    cout << "Time spent on searching: "  << timeUsed<< endl;
    cout << "Max depth have been searched: " << searchedMax << endl;

    
    outputMoves();
    targetPosi[0] = validMoves[moveChosen][0];
    targetPosi[1] = validMoves[moveChosen][1];

    board[targetPosi[0]][targetPosi[1]] = symbol;

    flipOthers(targetPosi, symbol, &board);

    if(ended){
        return;
    }

    cout << "Move Chosen by AI:" << endl;
    cout << moveChosen+1 << endl;
            

}


void gameCoreLoop(){
    
    
    if(playerX == HUMAN){
        getValidMoves(board, PLAYERX, PLAYERO);
        getHumanAction(PLAYERX);
    }else{
        getComputerAction(PLAYERX);
    }
    
    if(ended) return;
    cout << "TURN for "<< PLAYERO << ":" << endl;
    drawBoard();

    if(playerO == HUMAN){
        getValidMoves(board, PLAYERO, PLAYERX); 
        getHumanAction(PLAYERO);
    }else{
        getComputerAction(PLAYERO);
    }

    if(ended) return;
    cout << "TURN for "<< PLAYERX << ":" << endl;
    drawBoard();
}

void gameSession(){
    while(!ended){
        gameCoreLoop();
    }
    
}

int main(){
    
    srand(time(NULL));
    
    string toEnd;

    initialAttempt();
    gameSession();
    
    cout << "Enter anything to exit" << endl;
    cin >> toEnd;
    return 0;
}

