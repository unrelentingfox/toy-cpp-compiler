#include <iostream>

using namespace std;

int testIntUninit;
int testIntInit = 2;
int testArray[2];
int *testPointer;
char x, y, z;
int test5param(char x1, int x2, int x3, int x4, char x5);
int test2param(int t, int t1);
int test1param(int t);
int noparams();
char testredeclared = 'c';

// int testfunction(int t, int t1);


int main() {
  testfunction();
  bool variableinmain;
  return 0;
  // testfunction(1,1);
}

int testfunction(int t, int t1) {
  notdeclared();
  int  functionscope = 0;
  if (t) {
    return 0;
  }
  return t1;
}