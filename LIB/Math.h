#ifndef MATH_H
#define MATH_H
#define SET_BIT(REG, BIT) ((REG) |= (1U << (BIT)))
#define CLEAR_BIT(REG, BIT) ((REG) &= ~(1U << (BIT)))
#define GET_BIT(REG, BIT) (((REG) >> (BIT)) & 1U)
#define TOGGLE_BIT(REG, BIT) ((REG) ^= (1U << (BIT)))
#endif
