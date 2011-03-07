extern "C" {
    #include "8bit_binary.h"
    #include "relay.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(Relay) {
    uint8_t virtualDdr;
    uint8_t virtualPort;
    uint8_t virtualPin;

    void setup() {
        virtualDdr = 0x00;
        virtualPort = 0xff;
        virtualPin = 3;
        
        relay_init(&virtualDdr, &virtualPort, virtualPin);
    }
    
    void teardown() {
        
    }
}; // <-- that ";" is required!!

TEST(Relay, RelayOffAfterInitialization) {
    virtualDdr = 0x00;
    virtualPort = 0xff;
    
    relay_init(&virtualDdr, &virtualPort, virtualPin);
    
    BYTES_EQUAL(B00001000, virtualDdr);
    BYTES_EQUAL(B11110111, virtualPort);
}

TEST(Relay, RelayPowerOff) {
    virtualPort = 0xff;
    
    relay_power_off();
    
    BYTES_EQUAL(B11110111, virtualPort);
}

TEST(Relay, RelayPowerOn) {
    virtualPort = 0x00;
    
    relay_power_on();
    
    BYTES_EQUAL(B00001000, virtualPort);
}

