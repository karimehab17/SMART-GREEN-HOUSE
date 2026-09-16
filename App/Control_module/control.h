#ifndef CONTROL_H
#define CONTROL_H

#include "../../LIB/STD_TYPES.h"
#include "config.h"

typedef enum
{
    CONTROL_OK = 0,
    CONTROL_ERROR
} CONTROL_Status_t;

CONTROL_Status_t CTRL_Init(Config_t *pConfig);

CONTROL_Status_t CTRL_Update(void);

CONTROL_Status_t CTRL_UpdateThermal(void);
CONTROL_Status_t CTRL_UpdateIrrigation(void);
CONTROL_Status_t CTRL_UpdatePhoto(void);

#endif /* CONTROL_H */