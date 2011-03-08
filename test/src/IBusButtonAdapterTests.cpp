extern "C" {
    #include "8bit_binary.h"
    #include "ibus_message_parser.h"
    #include "ibus_button_adapter.h"
    
    #include "relay.h"
    #include "mute_button.h"
}

#include "CppUTest/TestHarness.h"


const IBusMessage sendEndPressed     = {0x50, 0xC8, 2, (uint8_t *)"\x3B\x80"};
// const IBusMessage sendEndPressedLong = {0x50, 0xC8, 2, (uint8_t *)"\x3B\x90"};
const IBusMessage sendEndReleased    = {0x50, 0xC8, 2, (uint8_t *)"\x3B\xA0"};

const IBusMessage rtPressed          = {0x50, 0xC8, 2, (uint8_t *)"\x3B\x02"};
// const IBusMessage rtPressedLong      = {0x50, 0xC8, 2, (uint8_t *)"\x3B\x12"};
const IBusMessage rtReleased         = {0x50, 0xC8, 2, (uint8_t *)"\x3B\x22"};

uint8_t virtualDdr;
uint8_t virtualPort;

TEST_GROUP(IBusButtonAdapter) {
    void setup() {
        relay_init(&virtualDdr, &virtualPort, 3);
        mute_button_init(&virtualDdr, &virtualPort, 4);
        button_adapter_init();

        virtualPort = 0xff;
    }
};

TEST(IBusButtonAdapter, PowerOnAndMuteReleasedAfterInitialization) {
    virtualPort = B00010000;
    button_adapter_init();
    BYTES_EQUAL(B00001000, virtualPort);
}

TEST(IBusButtonAdapter, PowerOff) {
    // after initialization, power is on
    
    button_adapter_handle_message(&rtPressed);
    button_adapter_handle_message(&rtReleased);
    
    BYTES_EQUAL(B11110111, virtualPort);
}

TEST(IBusButtonAdapter, TogglePowerOffAndBackOn) {
    button_adapter_handle_message(&rtPressed);
    button_adapter_handle_message(&rtReleased);
    
    BYTES_EQUAL(B11110111, virtualPort);

    button_adapter_handle_message(&rtPressed);
    button_adapter_handle_message(&rtReleased);
    
    BYTES_EQUAL(B11111111, virtualPort);
}

TEST(IBusButtonAdapter, MutePress) {
    virtualPort = 0x00;
    button_adapter_handle_message(&sendEndPressed);
    BYTES_EQUAL(B00010000, virtualPort);
    
    // button_adapter_handle_message(&sendEndReleased);
    // BYTES_EQUAL(B11110111, virtualPort);
}

TEST(IBusButtonAdapter, MutePressAndRelease) {
    virtualPort = 0x00;
    button_adapter_handle_message(&sendEndPressed);
    BYTES_EQUAL(B00010000, virtualPort);
    
    button_adapter_handle_message(&sendEndReleased);
    BYTES_EQUAL(0, virtualPort);
}
