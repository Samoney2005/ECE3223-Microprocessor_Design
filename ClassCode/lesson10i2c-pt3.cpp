#include "mbed.h"

// I2C bus: SDA = p9, SCL = p10
I2C i2c(p9, p10);

// TC74 temperature sensor I2C addresses
// 7-bit base address: 0b1001101 (0x4D)
// Write address (bit 0 = 0): 0b10011010
// Read  address (bit 0 = 1): 0b10011011
const int tc74addrW = 0b10011010;
const int tc74addrR = 0b10011011;
const int tc74addr  = 0b1001101;   // 7-bit form (used when mbed shifts internally)

int main()
{
    int ack, data;

    // --- I2C Bus Scanner ---
    // Probe all 128 possible 7-bit addresses.
    // mbed's i2c.write() expects the full 8-bit address (7-bit addr << 1),
    // so we multiply addr by 2 to form the write address.
    for (int addr = 0; addr < 128; addr++) {
        i2c.start();
        ack = i2c.write(addr * 2);   // send write address; ACK=1 means device responded
        i2c.stop();

        if (ack) {
            printf("Found device at address 0x%x \n", addr);  // FIX: addr must be outside the string
        }
    }

    /*
    // --- Example: Write 0x55 to EEPROM address 0, then read it back ---
    // Assumes a 24LC-series EEPROM with 7-bit address 0b1010000 (0x50)

    // --- Write phase ---
    i2c.start();
    ack = i2c.write(0b1010000 << 1);   // send write address (8-bit)
    if (!ack) {
        printf("no ack on write\n");
    }
    i2c.write(0);      // memory address high byte (or single byte for small EEPROMs)
    i2c.write(0x55);   // data byte to store
    i2c.stop();
    wait_us(1000);     // allow EEPROM write cycle to complete (~1 ms)

    // --- Read phase ---
    // First do a dummy write to set the memory pointer back to address 0
    i2c.start();
    ack = i2c.write(0b1010000 << 1);   // write address again to set pointer
    if (!ack) {
        printf("no ack on write\n");
    }
    i2c.write(0);      // memory address to read from

    // Repeated START, then switch to read mode
    i2c.start();
    i2c.write((0b1010000 << 1) | 1);   // send read address (LSB = 1)
    data = i2c.read(0);                // read one byte, send NACK (0) to end transfer
    i2c.stop();

    printf("Read back: 0x%x\n", data);
    */
}