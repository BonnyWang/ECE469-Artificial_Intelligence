#include <iostream>
#include<string>
#include <fstream>
#define COMPUTER 0
#define HUMAN 1
using namespace std;


// Player 1 is X, player 2 is O
static char board[8][8];
static int turn;
static int timeLimit;
static int playerX;
static int playerO;
static bool gameEnd;

void drawBoard(){
    cout<< "      1   2   3   4   5   6   7   8 " << endl;
    cout<< "    ---------------------------------"<<endl;
    for (int i = 0; i < 8; i++){
        cout << i + 1 << "   |";
        for (int j = 0; j < 8; j++){
            cout <<' '<< board[i][j] << ' '<< "|";
        }
        cout<< endl<< "    ---------------------------------"<<endl;
    }
}

void initializeBoard(){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            board[i][j] = ' ';
        }
    }

    board[3][3] = 'O';
    board[3][4] = 'X';
    board[4][3] = 'X';
    board[4][4] = 'O';
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
                            board[row][column] = 'X';
                            column++;
                            break;
                        case '2':
                            board[row][column] = 'O';
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

    drawBoard();
}

void gameCoreLoop(){
    
}

int main(){
    initialAttempt();
    return 0;
}

