extern "C" {
    #include "8bit_binary.h"
    #include "mute_button.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(MuteButton) {
    uint8_t virtualDdr;
    uint8_t virtualPort;
    uint8_t virtualPin;
    
    void setup() {
        virtualDdr = 0x00;
        virtualPort = 0xff;
        virtualPin = 4;
        
        mute_button_init(&virtualDdr, &virtualPort, virtualPin);
    }
    
    // void teardown() {
    //     
    // }
};

TEST(MuteButton, ButtonReleasedAfterInitialization) {
    virtualDdr = 0x00;
    virtualPort = 0xff;
    
    mute_button_init(&virtualDdr, &virtualPort, virtualPin);
    
    BYTES_EQUAL(B00010000, virtualDdr);
    BYTES_EQUAL(B11101111, virtualPort);
}

TEST(MuteButton, ButtonPress) {
    mute_button_press();
    
    BYTES_EQUAL(B11111111, virtualPort);
}

TEST(MuteButton, ButtonReleaseAfterPress) {
    mute_button_press();
    mute_button_release();
    
    BYTES_EQUAL(B11101111, virtualPort);
}
