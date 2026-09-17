#ifndef INTERRUPT_PRIVATE_H
#define INTERRUPT_PRIVATE_H

#include "../../LIB/STD_TYPES.h"

/* ==================== Interrupt Registers ==================== */

#define INTERRUPT_MCUCR_REG     (*(volatile uint8*)0x55U)
#define INTERRUPT_MCUCSR_REG    (*(volatile uint8*)0x54U)
#define INTERRUPT_GICR_REG      (*(volatile uint8*)0x5BU)
#define INTERRUPT_GIFR_REG      (*(volatile uint8*)0x5AU)
#define INTERRUPT_SREG_REG      (*(volatile uint8*)0x5FU)

/* ==================== SREG ==================== */

#define INTERRUPT_I_BIT        7U

/* ==================== MCUCR ==================== */

/* INT0 */
#define INTERRUPT_ISC00_BIT    0U
#define INTERRUPT_ISC01_BIT    1U

/* INT1 */
#define INTERRUPT_ISC10_BIT    2U
#define INTERRUPT_ISC11_BIT    3U

/* ==================== MCUCSR ==================== */

/* INT2 */
#define INTERRUPT_ISC2_BIT     6U

/* ==================== GICR ==================== */

#define INTERRUPT_INT2_BIT     5U
#define INTERRUPT_INT0_BIT     6U
#define INTERRUPT_INT1_BIT     7U

/* ==================== GIFR ==================== */

#define INTERRUPT_INTF2_BIT    5U
#define INTERRUPT_INTF0_BIT    6U
#define INTERRUPT_INTF1_BIT    7U

volatile uint8_t g_tickFlag = 0;

#endif /* INTERRUPT_PRIVATE_H */