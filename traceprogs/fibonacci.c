#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Please give a positive integer.\n");
    return -1;
  }

  int num = argv[1];

  int pre = 0;
  int result = 1;

  if (num == 1) {
    return pre;
  } else {
    while (num != 1) {
      result = result + pre;
      pre = result;
      num--;
    }
    return result;
  }
}
