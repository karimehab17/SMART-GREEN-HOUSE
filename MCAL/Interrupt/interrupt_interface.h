#ifndef INTERRUPT_INTERFACE_H
#define INTERRUPT_INTERFACE_H

#include "../../LIB/STD_TYPES.h"

/* ==================== External Interrupt Sources ==================== */

#define EXTI_INT0              0U
#define EXTI_INT1              1U
#define EXTI_INT2              2U

/* ==================== Sense Control ==================== */

#define EXTI_LOW_LEVEL         0U
#define EXTI_ANY_CHANGE        1U
#define EXTI_FALLING_EDGE      2U
#define EXTI_RISING_EDGE       3U

/* ==================== Callback ==================== */

typedef void (*EXTI_CallbackType)(void);

extern EXTI_CallbackType EXTI_pfCallBackArr[3];

/* ==================== Global Interrupt ==================== */

STD_ReturnType INTERRUPT_EnableGlobal(void);

STD_ReturnType INTERRUPT_DisableGlobal(void);

/* ==================== External Interrupt ==================== */

STD_ReturnType EXTI_SetSense(
    uint8 Copy_u8Int,
    uint8 Copy_u8Sense
);

STD_ReturnType EXTI_Enable(
    uint8 Copy_u8Int
);

STD_ReturnType EXTI_Disable(
    uint8 Copy_u8Int
);

STD_ReturnType EXTI_ClearFlag(
    uint8 Copy_u8Int
);

STD_ReturnType EXTI_SetCallback(
    uint8 Copy_u8Int,
    EXTI_CallbackType Copy_pfCallback
);

/* ==================== Timer0 Scheduler Tick ==================== */

void INTERRUPT_Timer0CompareCallback(void);

#endif /* INTERRUPT_INTERFACE_H */