#include "mbed.h"

I2C i2c(p9, p10);

int main()
{
    int ack, data;
    while (true) {
        i2c.start();
        ack = i2c.write(0b10011010);
        if (ack) {                          // fix 1: non-zero = NACK (failure)
            printf("no ack!\n");
        }
        i2c.write(0);                       // request current temperature register
        i2c.start();
        i2c.write(0b10011011);
        data = i2c.read(0);
        i2c.stop();
        printf("Temperature raw: %d\n", data);  // fix 2: actually use data
    }
}