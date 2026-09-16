#ifndef CONSOLE_H
#define CONSOLE_H

#include "../../LIB/STD_TYPES.h"
#include "config.h"

typedef enum
{
    CONSOLE_OK = 0,
    CONSOLE_ERROR
} CONSOLE_Status_t;

#define CONSOLE_MAX_COMMAND_LENGTH    24U

CONSOLE_Status_t CON_Init(void);
CONSOLE_Status_t CON_Process(void);

#endif /* CONSOLE_H */