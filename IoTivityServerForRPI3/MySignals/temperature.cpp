#include "../common.h"
#if USE_HW
    #include "common.h"
    #include "MySignals.h"
    #define USE_MYSIGNALS 1
#else 
    #include <string.h>
    #include <inttypes.h>
    #define USE_MYSIGNALS 0
#endif
#include "temperature.h"

double readTemperature() {
#if USE_MYSIGNALS
    // init MySignal Temperature
    Serial.begin(115200);  
    MySignals.begin();

    // Get MySignal Temperature
    double temperature = (double)MySignals.getTemperature();

    return temperature;
#else
    return -0.0f;
#endif
}