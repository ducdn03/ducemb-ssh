#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "animal.h"
#include "func.h"

int main(void)
{
    animal_t animal[MAX_ANIMAL];
    uint8_t type;
    uint8_t animal_num = 0;
    uint8_t input[10];
    for (animal_num = 0 ;animal_num < MAX_ANIMAL ;animal_num++)
    {
        #ifdef  GET_RANDOM
            type = get_random();
        #else
            type = gets(input);
        #endif
        if ( CAT == type)
        {
            strcpy(animal[animal_num].type,"cat");
            strcpy(animal[animal_num].price,"not for sale");
            animal[animal_num].action = &cat_speak;
            animal[animal_num].action();
            write_to_file(animal[animal_num].type);
        }
        else if ( DOG == type)
        {
            strcpy(animal[animal_num].type,"dog");
            strcpy(animal[animal_num].price,"i am your friend");
            animal[animal_num].action = &dog_speak;
            animal[animal_num].action();
            write_to_file(animal[animal_num].type);
        }
        else if ( PIG == type)
        {
            strcpy(animal[animal_num].type,"pig");
            strcpy(animal[animal_num].price,"500000");
            animal[animal_num].action = &pig_speak;
            animal[animal_num].action();
            write_to_file(animal[animal_num].type);
        }
    }
    return 0;
}

/********* END OF FILE *********/