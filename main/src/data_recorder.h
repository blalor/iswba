#ifndef DATA_RECORDER_H
#define DATA_RECORDER_H

#include <stdint.h>

void data_recorder_reset(void);
void data_recorder_record_byte(const uint8_t byte, const uint8_t status);
uint8_t data_recorder_get_byte_count(void);
const uint8_t *data_recorder_get_bytes(void);

#endif
