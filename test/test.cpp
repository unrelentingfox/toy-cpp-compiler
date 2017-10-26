#include <iostream>
// #include <fstream>

using namespace std;

int int1 = 1 + 1;
int int2 = 2;
float float1 = 1.0;
double double1 = 1.0;
char char1 = "c1";
char char2 = "c2";
char *cstring1 = "cstring1";
// string string1 = "string1";
int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

class testclass{
  int classvariable;
};

int main() {

  testclass instance;
  
  int1 = int1 + int2;

  int1 = int1 + (int2 + 1);

  int1 + int2;
  // int1 + float1;
  // int1 + char1;
  // int1 + cstring1;
  // int1 + string1;

  int1 - int2;
  // int1 - float1;
  // int1 - char1;
  // int1 - cstring1;
  // int1 - string1;

  int1 *int2;
  // int1 *float1;
  // int1 *char1;
  // int1 *cstring1;
  // int1 *string1;

  int newvariable = 1;

  int1 / int2;
  // int1 / float1;
  // int1 / char1;
  // int1 / cstring1;
  // int1 / string1;

  int1 = int2;
  // int1 = float1;
  // int1 = char1;
  // int1 = cstring1;
  // int1 = string1;

  int1 > int2;
  // int1 > float1;
  // int1 > char1;
  // int1 > cstring1;
  // int1 > string1;

  int1 < int2;
  // int1 < float1;
  // int1 < char1;
  // int1 < cstring1;
  // int1 < string1;

  array[int1];
  // array[float1];
  // array[char1];
  // array[cstring1];
  // array[string1];

  array[11];
}