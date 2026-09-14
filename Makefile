# ============================================================
# Smart Greenhouse Controller
# Target: ATmega32A @ 8 MHz
# Build system: GNU Make on Windows CMD
# ============================================================

MCU      = atmega32
F_CPU    = 8000000UL

TARGET   = main
BUILD    = build

CC       = avr-gcc
OBJCOPY  = avr-objcopy
SIZE     = avr-size

# ============================================================
# Compiler flags
# ============================================================

CFLAGS   = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -std=c99 -Wall -Wextra -Os

LDFLAGS  = -mmcu=$(MCU)

# ============================================================
# Include directories
# ============================================================

INCLUDES = \
	-Isrc \
	-IApp/Console_module \
	-IApp/Control_module \
	-IApp/Greenhouse_FSM \
	-IApp/Report_module \
	-IApp/Scheduler_module \
	-IHAl/Actuators \
	-IHAl/Buttons \
	-IHAl/LCD_i2c \
	-IHAl/Sensors \
	-ILIB \
	-IMCAL/ADC \
	-IMCAL/GPIO \
	-IMCAL/I2C \
	-IMCAL/Interrupt \
	-IMCAL/Timer \
	-IMCAL/UART

# ============================================================
# Source files
#
# sensor_demo.c is intentionally NOT included because the
# main application already contains main().
# ============================================================

SRCS = \
	src/main.c \
	App/Console_module/console.c \
	App/Control_module/control.c \
	App/Greenhouse_FSM/greenhouse_fsm.c \
	App/Report_module/report.c \
	App/Scheduler_module/scheduler.c \
	HAl/Actuators/Actuators_Driver.c \
	HAl/Buttons/Buttons_Driver.c \
	HAl/LCD_i2c/lcd_i2c.c \
	HAl/Sensors/Sensors_Driver.c \
	MCAL/ADC/ADC.c \
	MCAL/GPIO/GPIO.c \
	MCAL/I2C/i2c.c \
	MCAL/Interrupt/interrupt.c \
	MCAL/Timer/timer.c \
	MCAL/UART/uart.c

# ============================================================
# Object files
# ============================================================

OBJS = $(patsubst %.c,$(BUILD)/%.o,$(SRCS))

# ============================================================
# Default target
# ============================================================

all: $(BUILD)/$(TARGET).hex

# ============================================================
# Build HEX from ELF
# ============================================================

$(BUILD)/$(TARGET).hex: $(BUILD)/$(TARGET).elf
	@echo.
	@echo [HEX] Creating $(TARGET).hex
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# ============================================================
# Link all object files
# ============================================================

$(BUILD)/$(TARGET).elf: $(OBJS)
	@echo.
	@echo [LINK] Creating $(TARGET).elf
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(LDFLAGS) -o $@ $^

# ============================================================
# Compile C source files
# ============================================================

$(BUILD)/%.o: %.c
	@echo [CC] $<
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================
# Show memory usage
# ============================================================

size: $(BUILD)/$(TARGET).elf
	@echo.
	@echo ==========================================
	@echo Memory Usage
	@echo ==========================================
	$(SIZE) -A $<

# ============================================================
# Clean build directory
# ============================================================

clean:
	@echo [CLEAN] Removing build directory...
	@if exist "$(BUILD)" rmdir /S /Q "$(BUILD)"

# ============================================================
# Rebuild from scratch
# ============================================================

rebuild: clean all

# ============================================================
# Help
# ============================================================

help:
	@echo.
	@echo Smart Greenhouse Controller Build System
	@echo.
	@echo Available commands:
	@echo   make          - Build the project
	@echo   make clean   - Delete build files
	@echo   make rebuild - Clean and build again
	@echo   make size    - Show Flash/RAM usage
	@echo   make help    - Show this help
	@echo.

# ============================================================
# Phony targets
# ============================================================

.PHONY: all clean rebuild size help