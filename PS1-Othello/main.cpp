#include <iostream>
#include<string>
#include <fstream>
#include <windows.h>
#include <vector>

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

static bool gameEnd;

HANDLE hConsole;
int validMoves[64][2];
int nMoves = 0;

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

bool addValidPosition(int row, int column){
    for(int i = 0; i < nMoves; i++){
        if(validMoves[i][0] == row && validMoves[i][1]  == column){
            // A repeated solution
            return false;            
        }
    }

    validMoves[nMoves][0] = row;
    validMoves[nMoves][1] = column;
                        
    nMoves++;
    return true;
}


void checkHorizontal(char playerSymbol, char oppoSymbol){
    int tempStart[] = {INVALID,INVALID};
    int tempEnd[] = {INVALID,INVALID};

    for (int row = 0; row < BOARDSIZE; row++){
        for (int column = 0; column < BOARDSIZE; column++){
            if(board[row][column] == playerSymbol){
                tempStart[0] = row;
                tempStart[1] = column;
            }

            if(column -1 >= 0){
                if(board[row][column] == oppoSymbol && board[row][column-1] == ' '){
                    tempEnd[0] = row;
                    tempEnd[1] = column-1;
                }
            }
            
            
            if(board[row][column] == ' '){
                
                if(tempStart[0] != INVALID){
                     // empty not directly after the player symbol
                    if(column - tempStart[1] !=1){
                        
                        addValidPosition(row, column);
                        
                    }
                    
                }
                
                tempStart[0] = INVALID;
                tempStart[1] = INVALID;

                tempEnd[0] = INVALID;
                tempEnd[1] = INVALID;
               
            }

            if(tempEnd[0]!= INVALID && board[row][column] == playerSymbol){
                addValidPosition(tempEnd[0], tempEnd[1]);

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
void checkVertical(char playerSymbol, char oppoSymbol){
    int tempStart[] = {INVALID,INVALID};
    int tempEnd[] = {INVALID,INVALID};
    

    for (int column = 0; column < BOARDSIZE; column++){
        for (int row = 0; row < BOARDSIZE; row++){
            if(board[row][column] == playerSymbol){
                tempStart[0] = row;
                tempStart[1] = column;
            }
           
            if(row - 1 >= 0){
                if(board[row][column] == oppoSymbol && board[row-1][column] == ' '){
                    tempEnd[0] = row-1;
                    tempEnd[1] = column;
                }
            }
            
            if(board[row][column] == ' '){
                if(tempStart[0] != INVALID){
                    // empty not directly after the player symbol
                    if(abs(row - tempStart[0]) ==1){
                        tempStart[0] = INVALID;
                        tempStart[1] = INVALID;
                    }else{
                        addValidPosition(row, column);
                    }
                }

                tempStart[0] = INVALID;
                tempStart[1] = INVALID;

                tempEnd[0] = INVALID;
                tempEnd[1] = INVALID;
            }

            if(tempEnd[0]!= INVALID && board[row][column] == playerSymbol){
                addValidPosition(tempEnd[0], tempEnd[1]);
                
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
void checkUpDia(char playerSymbol, char oppoSymbol){
    int tempStart[] = {INVALID,INVALID};
    int tempEnd[] = {INVALID, INVALID};

    for (int k = 2; k <= 12; k++){
        int column = k - 7 > 0 ? k - 7 : 0;
        for (column; column <= k; column++){
            int row = k - column;

            // Might outside the boundary
            if(row < BOARDSIZE && column < BOARDSIZE){
                if(board[row][column] == playerSymbol){
                    tempStart[0] = row;
                    tempStart[1] = column;
                }
           
                if(row + 1 < BOARDSIZE && column - 1 >= 0 ){
                    if(board[row][column] == oppoSymbol && board[row+1][column-1] == ' '){
                        tempEnd[0] = row + 1;
                        tempEnd[1] = column - 1;
                    }
                }
                
                if(board[row][column] == ' '){
                    if(tempStart[0] != INVALID){
                        // empty not directly after the player symbol
                        if(abs(row - tempStart[0]) ==1){
                            tempStart[0] = INVALID;
                            tempStart[1] = INVALID;
                        }else{
                            addValidPosition(row, column);
                        }
                    }

                    tempStart[0] = INVALID;
                    tempStart[1] = INVALID;

                    tempEnd[0] = INVALID;
                    tempEnd[1] = INVALID;
                }

                if(tempEnd[0]!= INVALID && board[row][column] == playerSymbol){
                    addValidPosition(tempEnd[0], tempEnd[1]);
                    
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
void checkDownDia(char playerSymbol, char oppoSymbol){
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
                if(board[row][column] == playerSymbol){
                    tempStart[0] = row;
                    tempStart[1] = column;
                }
           
                if(row - 1 >= 0 && column - 1 >= 0 ){
                    if(board[row][column] == oppoSymbol && board[row-1][column-1] == ' '){
                        tempEnd[0] = row - 1;
                        tempEnd[1] = column - 1;
                    }
                }
                
                if(board[row][column] == ' '){
                    if(tempStart[0] != INVALID){
                        // empty not directly after the player symbol
                        if(abs(row - tempStart[0]) ==1){
                            tempStart[0] = INVALID;
                            tempStart[1] = INVALID;
                        }else{
                            addValidPosition(row, column);
                        }
                    }

                    tempStart[0] = INVALID;
                    tempStart[1] = INVALID;

                    tempEnd[0] = INVALID;
                    tempEnd[1] = INVALID;
                }

                if(tempEnd[0]!= INVALID && board[row][column] == playerSymbol){
                    addValidPosition(tempEnd[0], tempEnd[1]);
                    
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

void getValidMoves(char playerSymbol, char oppoSymbol){
    nMoves = 0;
    checkHorizontal(playerSymbol, oppoSymbol);
    checkVertical(playerSymbol, oppoSymbol);
    checkUpDia(playerSymbol, oppoSymbol);
    checkDownDia(playerSymbol, oppoSymbol);
}

void flipOthers(int position[2], char mSymbol){
    // TODO: This function is a bit redundant

    // Check Flip in the Horizontal
    for(int column = position[1] -1; column >= 0; column --){
        if(board[position[0]][column] == mSymbol){
            for( int flipStart = column+1 ; flipStart < position[1]; flipStart++){
                board[position[0]][flipStart] = mSymbol;
            }
            break;
        }else if(board[position[0]][column] == ' '){
            break;
        }
    }


    for(int column = position[1] + 1; column < BOARDSIZE; column ++){
        if(board[position[0]][column] == mSymbol){
            for( int flipStart = column-1 ; flipStart > position[1]; flipStart--){
                board[position[0]][flipStart] = mSymbol;
            }
            break;
        }else if(board[position[0]][column] == ' '){
            break;
        }
    }
    

    // Flip verticals
    for(int row = position[0] -1; row >= 0; row --){
        if(board[row][position[1]] == mSymbol){
            for( int flipStart = row+1 ; flipStart < position[0]; flipStart++){
                board[flipStart][position[1]] = mSymbol;
            }
            break;
        }else if(board[row][position[1]] == ' '){
            break;
        }
    }


    for(int row = position[0] + 1; row < BOARDSIZE; row ++){
        if(board[row][position[1]] == mSymbol){
            for( int flipStart = row-1 ; flipStart > position[0]; flipStart--){
                board[flipStart][position[1]] = mSymbol;
            }
            break;
        }else if(board[row][position[1]] == ' '){
            break;
        }
    }

    // Check for \ direction
    for(int row = position[0]+1, column = position[1] + 1; row < BOARDSIZE && column < BOARDSIZE; row++, column++){
        if(board[row][column] == mSymbol){
            for(int flipStart[2] = {row-1, column-1}; flipStart[0] > position[0]; flipStart[0]--, flipStart[1]-- ){
                board[flipStart[0]][flipStart[1]] = mSymbol;
            }
            break;
        }else if(board[row][column] == ' '){
            break;
        }
    }

    
    for(int row = position[0] - 1,column = position[1] - 1 ; row >= 0 && column >= 0; row--, column--){
        if(board[row][column] == mSymbol){
            for(int flipStart[2] = {row+1, column+1}; flipStart[0] < position[0]; flipStart[0]++, flipStart[1]++ ){
                board[flipStart[0]][flipStart[1]] = mSymbol;
            }
            break;
        }else if(board[row][column] == ' '){
            break;
        }
    }

    // Check for / direction
    for(int row = position[0]-1, column = position[1] + 1; row >= 0 && column < BOARDSIZE; row--, column++){
        if(board[row][column] == mSymbol){
            for(int flipStart[2] = {row+1, column-1}; flipStart[0] < position[0]; flipStart[0]++, flipStart[1]-- ){
                board[flipStart[0]][flipStart[1]] = mSymbol;
            }
            break;
        }else if(board[row][column] == ' '){
            break;
        }
    }

    for(int row = position[0] + 1, column = position[1] - 1; row < BOARDSIZE && column >= 0; row++, column--){
        if(board[row][column] == mSymbol){
            for(int flipStart[2] = {row-1, column+1}; flipStart[0] > position[0]; flipStart[0]--, flipStart[1]++ ){
                board[flipStart[0]][flipStart[1]] = mSymbol;
            }
            break;
        }else if(board[row][column] == ' '){
            break;
        }
    }
    

}

void outputMoves(){
    for(int i = 0; i < nMoves; i++){
        cout << i+1 << ". ("<< validMoves[i][0]<< "," << validMoves[i][1]<< ")"<< endl;
    }
}

void getHumanAction(char symbol){
    outputMoves();
    
    int moveChosen;
    while(true){
        cout << "Which Position do you want to place?" << endl;
        cin >> moveChosen;
        if(moveChosen <= nMoves && moveChosen > 0 ){
            int targetPosi[2] =  {validMoves[moveChosen-1][0], validMoves[moveChosen-1][1]};         
            board[targetPosi[0]][targetPosi[1]] = symbol;

            flipOthers(targetPosi, symbol);
            
            return;
        }
    }

}

void getComputerAction(char symbol){
    // outputMoves();
}



void gameCoreLoop(){
    
    getValidMoves(PLAYERX, PLAYERO);
    
    if(playerX == HUMAN){
        getHumanAction(PLAYERX);
    }else{
        getComputerAction(PLAYERX);
    }

    cout << "TURN for "<< PLAYERO << ":" << endl;
    drawBoard();

   
    getValidMoves(PLAYERO, PLAYERX); 
    if(playerO == HUMAN){
        getHumanAction(PLAYERO);
    }else{
        getComputerAction(PLAYERO);
    }

    cout << "TURN for "<< PLAYERX << ":" << endl;
    drawBoard();
}

void gameSession(){
    while(true){
        gameCoreLoop();
    }
    
}

int main(){
    
    initialAttempt();
    gameSession();
    return 0;
}

