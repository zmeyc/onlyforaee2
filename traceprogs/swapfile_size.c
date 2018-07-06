//
// Created by Yuhan on 2018-07-05.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 256

typedef unsigned long addr_t;

/*
 * This program used to calculate the number of
 * unique virtual page in the trace file.
 */
int main(int argc, char *argv[]) {
    char *tracefile = argv[1];
    FILE *tfp = NULL;

    // Open the trace file
    if(tracefile != NULL) {
        if((tfp = fopen(tracefile, "r")) == NULL) {
            perror("Error opening tracefile:");
            exit(1);
        }
    }

    // Calculate the unique number of virtual address
    int unique = 0; // Number of unique virtual address

    int size = 0; // size of trace file
    char buf[MAXLINE]; // Maxline in sim.h is 256

    // Keep track of the size of the tracefile
    while(fgets(buf, MAXLINE, tfp) != NULL) {
        if(buf[0] != '=') {
            size++;
        }
    }

    addr_t vaddr = 0;
    char type;
    addr_t unique_vaddr[size];

    fseek(tfp, 0, SEEK_SET);

    while (fgets(buf, MAXLINE, tfp) != NULL) {
        if (strcmp(&buf[0], "=") != 0) {
            sscanf(buf, "%c %lx", &type, &vaddr);

            if (unique == 0) {
                unique_vaddr[unique] = vaddr;
                unique++;
            } else {
                int flag = 0;

                for (int i = 0; i < unique; i++) {
                    if (unique_vaddr[i] == vaddr) {
                        flag = 1;
                    }
                }

                if (flag == 0) {
                    unique_vaddr[unique] = vaddr;
                    unique++;
                }
            }
        }
    }

    // Close the trace file
    if(fclose(tfp) != 0) {
        perror("Error opening tracefile:");
        exit(1);
    }

    // Print the number of unique virtual address in the trace file
    printf("The number of unique virtual address: %d\n", unique);
}


