#include <iostream>
#include<string>

#include <cstdlib>
#include <ctime> 

using namespace std;

int* hi(){
     int* a = new int[2];
    *a = 1;
    *(a+1) = 5;

    return a;
}

int main(){
    srand(time(NULL));
    cout << float(-3/15)<<endl;
    return 0;
}