#ifndef CONTROL_H
#define CONTROL_H

#include "STD_Types.h"

typedef enum {
    CONTROL_OK = 0,
    CONTROL_ERROR
} CONTROL_Status_t;

CONTROL_Status_t CTRL_Init(void);
CONTROL_Status_t CTRL_Update(void);

#endif // CONTROL_H