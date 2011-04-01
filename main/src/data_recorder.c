#include "data_recorder.h"

static volatile uint8_t bytes[50];
static volatile uint8_t byte_count;

void data_recorder_reset() {
    byte_count = 0;
}

void data_recorder_record_byte(const uint8_t byte, const uint8_t status) {
    bytes[byte_count++] = byte;
    bytes[byte_count++] = status;
}

uint8_t data_recorder_get_byte_count() {
    return byte_count;
}

const uint8_t *data_recorder_get_bytes() {
    const uint8_t *_bytes = (uint8_t *)bytes;
    return _bytes;
}
