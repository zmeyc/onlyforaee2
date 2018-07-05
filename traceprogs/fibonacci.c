#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  /* Markers used to bound trace regions of interest */
  volatile char MARKER_START, MARKER_END;
  /* Record marker addresses */
  FILE* marker_fp = fopen("simpleloop.marker","w");
  if(marker_fp == NULL ) {
    perror("Couldn't open marker file:");
    exit(1);
  }
  fprintf(marker_fp, "%p %p", &MARKER_START, &MARKER_END );
  fclose(marker_fp);

  if (argc != 2) {
    printf("Please give a positive integer.\n");
    return -1;
  }

  long int num = strtol(argv[1], NULL, 10);

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
