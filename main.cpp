#include <iostream>
using namespace std;

static char board[8][8];

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
}

int main(){
    cout << "Welcome to the Othello paradise!" << endl;
    initializeBoard();
    drawBoard();
    return 0;
}

