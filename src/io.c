#include "io.h"

void error() {
    // TXLED
    DDRD |= PD5;
    PORTD &= ~PD5;
}

void errorOff() {
    // TXLED
    DDRD |= PD5;
    PORTD |= PD5;
}

void error2() {
    // TXLED
    DDRB |= PB0;
    PORTB &= ~PB0;
}

void error2Off() {
    // TXLED
    DDRB |= PB0;
    PORTB |= PB0;
}

void ledOn() {
    DDRD |= PD0;
    PORTD |= PD0;
}

void ledOff() {
    DDRD |= PD0;
    PORTD &= ~PD0;
}
