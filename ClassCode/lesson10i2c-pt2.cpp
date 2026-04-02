#include "mbed.h"

I2C i2c(p9, p10);
const int tc74addrW = 0b10011010;
const int tc74addrR = 0b10011011;

int main()
{
    int ack, data;
    while (true) {
        i2c.start();
        ack = i2c.write(0b10011010);
        if (!ack) {                          
            printf("no ack!\n");
        }
        i2c.write(0);                       
        i2c.start();
        i2c.write(tc74addrR);
        data = i2c.read(0);
        i2c.stop();
        printf("Temperature raw: %d\n", data);  
    }
}