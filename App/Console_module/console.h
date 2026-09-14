#ifndef SMART_GREEN_HOUSE_APP_CONSOLE_MODULE_CONSOLE_H
#define SMART_GREEN_HOUSE_APP_CONSOLE_MODULE_CONSOLE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLE_BUFFER_SIZE 128

typedef struct {
    uint8_t light_level;
    bool fan_enabled;
    bool pump_enabled;
    bool heater_enabled;
} ConsoleState;

void console_init(void);
void console_clear(void);
void console_write(const char *message);
void console_writeln(const char *message);
char console_read_char(void);
bool console_read_line(char *buffer, size_t buffer_size);
void console_set_state(const ConsoleState *state);
void console_get_state(ConsoleState *state);

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */
