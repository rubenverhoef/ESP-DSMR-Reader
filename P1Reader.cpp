#include <Arduino.h>
#include "P1Reader.h"
#include "settings.h"

// **********************************
// * P1 DSMR Reader                 *
// **********************************

String formatDate(String input)
{
    char output[25];
    int year, month, day, hour, minute, seconds;
    int TimeZone;
    char period[1];

    sscanf(input.c_str(), "%2d%2d%2d%2d%2d%2d%1s", &year, &month, &day, &hour, &minute, &seconds, period);

    // DST 
    TimeZone = TIMEZONE;
    if (String (period) == "S")
    {
        TimeZone += 1;
    }
    
    sprintf(output, "20%02d-%02d-%02dT%02d:%02d:%02d%%2B%02d", year, month, day, hour, minute, seconds, TimeZone);

    String strout(output);
    
    return strout;
}
