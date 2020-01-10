#include <Arduino.h>

#include "LED.h"

// * Include settings
#include "settings.h"

// * Initiate led blinker library
Ticker ticker;

// * Blink on-board Led
void tick()
{
    // * Toggle state
    int state = digitalRead(LED_BUILTIN);    // * Get the current state of GPIO1 pin
    digitalWrite(LED_BUILTIN, !state);       // * Set pin to the opposite state
}


void LED_Blink(float TimeSeconds)
{
    if (TimeSeconds <= 0)
    {
        ticker.detach();
        digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
        ticker.attach(TimeSeconds, tick);
    }
}
