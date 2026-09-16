#ifndef CONSOLE_H
#define CONSOLE_H

#include "../../LIB/STD_TYPES.h"
#include "config.h"

typedef enum
{
    CONSOLE_OK = 0,
    CONSOLE_ERROR
} CONSOLE_Status_t;

CONSOLE_Status_t CON_Init(Config_t *pConfig);
CONSOLE_Status_t CON_Process(void);

#endif /* CONSOLE_H */