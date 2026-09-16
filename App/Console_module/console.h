#ifndef CONSOLE_H
#define CONSOLE_H

#include "../../LIB/STD_TYPES.h"

typedef enum
{
    CONSOLE_OK = 0,
    CONSOLE_ERROR
} CONSOLE_Status_t;

/* ==================== Console Limits ==================== */

#define CONSOLE_MAX_COMMAND_LENGTH    24U

/* ==================== Console Initialization ==================== */

CONSOLE_Status_t CON_Init(void);

/* ==================== Console Processing ==================== */

CONSOLE_Status_t CON_Process(void);

#endif /* CONSOLE_H */