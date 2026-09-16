#ifndef REPORT_H
#define REPORT_H

#include "../../LIB/STD_TYPES.h"
#include "config.h"

STD_ReturnType RPT_Init(SysData_t *pSysData);

STD_ReturnType RPT_Update(void);

STD_ReturnType RPT_SendStatus(void);

STD_ReturnType RPT_SendEvent(const uint8 *pEvent);

uint16 RPT_GetPumpRuntime(void);

#endif /* REPORT_H */