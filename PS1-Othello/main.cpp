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
        playerX = HUMAN;
    }else{
        // If user entered invalid then playerO is the computer by default
        playerX = COMPUTER;
    }

    cout << "Enter the time limit in second for each game round:" << endl;
    cin >> timeLimitValue;

    if(stof(timeLimitValue) > 100 || stoi(timeLimitValue) < 0.0001){
        cout << "Invalid time limit value, time limit set to 5s by default";
        timeLimit = 5;
    }else{
        timeLimit = stof(timeLimitValue);
    }

    drawBoard();
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

            if(column -1 > 0){
                if(board[row][column] == oppoSymbol && board[row][column-1] == ' '){
                    tempEnd[0] = row;
                    tempEnd[1] = column-1;
                }
            }
            
            
            if(board[row][column] == ' '){
                
                if(tempStart[0] != INVALID){
                     // empty not directly after the player symbol
                    if(column - tempStart[1] !=1){
                        
                        // A valid position is found
                        validMoves[nMoves][0] = row;
                        validMoves[nMoves][1] = column;
                        
                        nMoves++;
                    }
                    
                }
                
                tempStart[0] = INVALID;
                tempStart[1] = INVALID;

                tempEnd[0] = INVALID;
                tempEnd[1] = INVALID;
               
            }

            if(tempEnd[0]!= INVALID && board[row][column] == playerSymbol){
                validMoves[nMoves][0] = tempEnd[0];
                validMoves[nMoves][1] = tempEnd[1];
                nMoves++;

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
           
            if(row - 1 > 0){
                if(board[row][column] == oppoSymbol && board[row-1][column] == ' '){
                    tempEnd[0] = row-1;
                    tempEnd[1] = column;
                }
            }
            
            if(board[row][column] == ' '){
                if(tempStart[0] != INVALID){
                    // empty not directly after the player symbol
                    if(row - tempStart[0] ==1){
                        tempStart[0] = INVALID;
                        tempStart[1] = INVALID;
                    }else{
                        
                        // A valid position is found
                        validMoves[nMoves][0] = row;
                        validMoves[nMoves][1] = column;
                        nMoves++;
                    }
                }

                tempStart[0] = INVALID;
                tempStart[1] = INVALID;

                tempEnd[0] = INVALID;
                tempEnd[1] = INVALID;
            }

            if(tempEnd[0]!= INVALID && board[row][column] == playerSymbol){
                validMoves[nMoves][0] = tempEnd[0];
                validMoves[nMoves][1] = tempEnd[1];
                nMoves++;
                
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

// check whether valid moves in the up diagonal
void checkUpDia(char playerSymbol, char oppoSymbol){
    int tempStart[] = {INVALID,INVALID};

    for (int k; k < BOARDSIZE*2; k++){
        for (int column = 0; column < k; column++){
            int row = k - column;

            if(row < BOARDSIZE && column < BOARDSIZE){
                if(board[row][column] == playerSymbol){
                    tempStart[0] = row;
                    tempStart[1] = column;
                }

                if(tempStart[0] != INVALID && board[row][column] == oppoSymbol){
                    continue;
                }else if(tempStart[0] != INVALID && board[row][column] == ' '){
                    
                    // empty not directly after the player symbol
                    if(row - tempStart[0] ==1){
                        tempStart[0] = INVALID;
                        tempStart[1] = INVALID;
                    }else{
                        
                        // A valid position is found
                        validMoves[nMoves][0] = row;
                        validMoves[nMoves][1] = column;
                    }
                }
            }
            
        }
    }
}

void getValidMoves(char playerSymbol, char oppoSymbol){
    nMoves = 0;
    checkHorizontal(playerSymbol, oppoSymbol);
    checkVertical(playerSymbol, oppoSymbol);
    // checkUpDia(playerSymbol, oppoSymbol);
    
}

void outputMoves(){
    for(int i = 0; i < nMoves; i++){
        cout << i+1 << ". ("<< validMoves[i][0] + 1 << "," << validMoves[i][1] + 1 << ")"<< endl;
    }
}

void getHumanAction(char symbol){
    outputMoves();
    
    int moveChosen;
    while(true){
        cout << "Which Position do you want to place?" << endl;
        cin >> moveChosen;
        if(moveChosen <= nMoves && moveChosen > 0 ){
            board[validMoves[moveChosen-1][0]][validMoves[moveChosen-1][1]] = symbol;
            // Valid input
            break;
        }
    }

}

void getComputerAction(char symbol){
    outputMoves();
}

void gameCoreLoop(){
    
    getValidMoves(PLAYERX, PLAYERO);
    
    if(playerX == HUMAN){
        getHumanAction(PLAYERX);
    }else{
        getComputerAction(PLAYERX);
    }

    drawBoard();

   
    getValidMoves(PLAYERO, PLAYERX); 
    if(playerO == HUMAN){
        getHumanAction(PLAYERO);
    }else{
        getComputerAction(PLAYERO);
    }

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

