#include "mbed.h"
#include <cmath>
using namespace std::chrono_literals;

AnalogIn  tempPin(p19);
DigitalIn modePin(p5, PullUp);
// A, B, C, D, E, F, G, DP
BusOut display(p26, p27, p15, p16, p17, p25, p24, p14);

// ---------- Convert symbol to segments ----------
int convertDisplay(int sym)
{
    switch (sym)
    {
        case 0:   return 0b00111111; // A B C D E F
        case 1:   return 0b00000110; // B C
        case 2:   return 0b01011011; // A B D E G
        case 3:   return 0b01001111; // A B C D G
        case 4:   return 0b01100110; // B C F G
        case 5:   return 0b01101101; // A C D F G
        case 6:   return 0b01111101; // A C D E F G
        case 7:   return 0b00000111; // A B C
        case 8:   return 0b01111111; // A B C D E F G
        case 9:   return 0b01101111; // A B C D F G
        case '-': return 0b01000000; // G
        case 'C': return 0b00111001; // A D E F
        case 'F': return 0b01110001; // A E F G
        default:  return 0x00;       // blank
    }
}

// ---------- Voltage sampling ----------
float collectVoltage(int samples)
{
    float sum = 0.0f;
    for (int i = 0; i < samples; i++) {
        sum += tempPin.read();
    }
    return (sum / samples) * 3.3f;
}

// ---------- Show one symbol (no decimal point) ----------
void showSymbol(int sym)
{
    display = convertDisplay(sym);
    ThisThread::sleep_for(700ms);
    display = 0;
    ThisThread::sleep_for(100ms);
}

// ---------- Show a digit, optionally with decimal point ----------
void showDigit(int digit, bool withDot = false)
{
    int seg = convertDisplay(digit);
    if (withDot) seg |= 0b10000000; // OR in the DP bit
    display = seg;
    ThisThread::sleep_for(700ms);
    display = 0;
    ThisThread::sleep_for(100ms);
}

// ---------------- Main ----------------
int main()
{
    while (true) {
        // Fahrenheit if pin grounded, Celsius if floating (pulled high)
        char unit = (modePin.read() == 0) ? 'F' : 'C';

        // Blank display before sampling
        display = 0;
        float voltage = collectVoltage(1000);

        // MCP9701 equation
        float tempC = (voltage - 0.400f) / 0.0195f;

        // Convert units
        float tempVal = (unit == 'F')
                        ? (tempC * 9.0f / 5.0f + 32.0f)
                        : tempC;

        bool  negative = (tempVal < 0.0f);
        float absVal   = fabsf(tempVal);
        int   tenths   = (int)roundf(absVal * 10.0f);
        int   intPart  = tenths / 10;
        int   fracPart = tenths % 10;

        // Minus sign
        if (negative) {
            showSymbol('-');
        }

        // Integer part
        if (intPart == 0) {
            showDigit(0);
        } else {
            int pow10 = 1;
            while (pow10 * 10 <= intPart) {
                pow10 *= 10;
            }
            int remaining = intPart;
            while (pow10 > 0) {
                int digit = remaining / pow10;
                showDigit(digit);
                remaining %= pow10;
                pow10 /= 10;
            }
        }

        // Fraction digit with decimal point combined
        showDigit(fracPart, true);

        // Unit symbol
        showSymbol(unit);
    }
}