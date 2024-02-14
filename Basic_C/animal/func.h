#ifndef FUNC_H
#define FUNC_H

/********* DEFINITION ********/
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

#define    MIN_RAND    0u
#define    MAX_RAND    2u

#define GET_RANDOM

/********* END OF DEFINITION *********/

/********* PROTOTYPE *********/
void    write_to_file(uint8_t animal_type[]);
int     get_random();

/********* END OF PROTOTYPE *********/

#endif
/********* END OF FILE *********/