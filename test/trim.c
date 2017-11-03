#include <stdio.h>
#include <string.h>

char* trimLeadingSpaces(char*);

int main(int argc, char* argv[]){
  char* str = " abc 123 ";
  printf("|%s|\n", trimLeadingSpaces(str));
  printf("|%s|\n", str);
  return 0;
}

char* trimLeadingSpaces(char* str){
  //count begining spaces
  while('\0' != str[0] && ' ' == str[0]){
    putchar(str[0]);
    str++;
  }
  printf("trimed: |%s|\n", str);
  return str;
}
