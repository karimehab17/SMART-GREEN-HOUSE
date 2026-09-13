#ifndef STD_TYPES_H
#define STD_TYPES_H

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * Shared standard types for AVR_NTI LIB / MCAL / HAL / Logic.
 */

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef signed char sint8;
typedef signed short sint16;
typedef signed long sint32;

typedef unsigned char uint8_h;

typedef enum
{
    E_OK = 0,
    E_NOK = 1,
    E_port_not_valid = 2,
    E_pin_not_valid = 3,
} STD_ReturnType;

#endif /* STD_TYPES_H */
