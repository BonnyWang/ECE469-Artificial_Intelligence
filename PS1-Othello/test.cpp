#include <iostream>
#include<string>
using namespace std;

int* hi(){
     int* a = new int[2];
    *a = 1;
    *(a+1) = 5;

    return a;
}

int main(){
    int i = *(hi()+1);
    cout << i << endl;
}