#include <stdio.h>
#include <stdlib.h>
#include "func.h"

void write_to_file(uint8_t animal_type[])
{
    FILE * file = fopen("output.txt","a");
    fprintf(file,"%s\n",animal_type);
    fclose(file);
}

int get_random()
{
    int res = 0;
    res = MIN_RAND + (int)(rand()*(MAX_RAND - MIN_RAND + 1.0)/(1.0 + RAND_MAX));
    return res;
}

/********* END OF FILE *********/