#include "../common.h"
#if USE_HW
    #include "common.h"
    #include "MySignals.h"
    #include "MySignals_BLE.h"
    #define USE_MYSIGNALS 1
#else 
    #include <string.h>
    #include <inttypes.h>
    #define USE_MYSIGNALS 0
#endif
#include "glucose.h"

#define GLUCO_HANDLE1 99
#define GLUCO_HANDLE2 83

// Write here the MAC address of BLE device to find
char MAC_GLUCO[14] = "187A93090BE6";

uint8_t available_gluco = 0;
uint8_t connected_gluco = 0;
uint8_t connection_handle_gluco = 0;

//!Struct to store data of the glucometer.
struct glucometerBLEDataVector
{
  uint16_t glucose;
  uint8_t info;
};

//!Vector to store the glucometer measures and dates.
glucometerBLEDataVector glucometerBLEData;

uint16_t getGlucose() {
    return glucometerBLEData.glucose;
}

uint8_t getMySignalGlucose() {
    uint8_t ret = 0;
    #if USE_HW
    char strbuf[30] = {0x00, };

    available_gluco = MySignals_BLE.scanDevice(MAC_GLUCO, 1000, TX_POWER_MAX);
    
      MySignals.disableMuxUART();      
      memset(strbuf, 0x00, 30);
      sprintf(strbuf, "Gluco available: %d\n", available_gluco);  
      printf(strbuf);

      MySignals.enableMuxUART();
    
      printf ("start access \n");
      if (available_gluco == 1)
      {
    
        if (MySignals_BLE.connectDirect(MAC_GLUCO) == 1)
        {
          MySignals.println("Connected");
          connected_gluco = 1;
    
          uint8_t gluco_subscribe_message[2] = { 0x01 , 0x00 };
          delay(200);
    
          MySignals_BLE.attributeWrite(MySignals_BLE.connection_handle, GLUCO_HANDLE1, gluco_subscribe_message, 2);
          MySignals_BLE.attributeWrite(MySignals_BLE.connection_handle, GLUCO_HANDLE2, gluco_subscribe_message, 2);
          
          delay(200);
          MySignals.println("Insert blood stripe (20s)");
    
          unsigned long previous = millis();
          do
          {
            if (MySignals_BLE.waitEvent(1000) == BLE_EVENT_ATTCLIENT_ATTRIBUTE_VALUE)
            {
              MySignals.disableMuxUART();
              MySignals.println("Received Result!");
              
              if (MySignals_BLE.event[8] == 0x0c)
              {
                uint8_t gh = MySignals_BLE.event[12] - 48;
                uint8_t gl = MySignals_BLE.event[13] - 48;
                glucometerBLEData.glucose = (gh * 10) + gl;
                glucometerBLEData.info = 0;
    
                memset(strbuf, 0x00, 30);
                sprintf(strbuf, "Glucose(mg/dl): %d", glucometerBLEData.glucose);
                MySignals.println(&(strbuf[0]));
              }
              if (MySignals_BLE.event[8] == 0x0d)
              {
    
                uint8_t gh = MySignals_BLE.event[12] - 48;
                uint8_t gm = MySignals_BLE.event[13] - 48;
                uint8_t gl = MySignals_BLE.event[14] - 48;
                glucometerBLEData.glucose = (gh * 100) + (gm * 10) + gl;
                glucometerBLEData.info = 0;
    
                memset(strbuf, 0x00, 30);
                sprintf(strbuf, "Glucose(mg/dl): %d", glucometerBLEData.glucose);
                MySignals.println(&(strbuf[0]));
              }
              if (MySignals_BLE.event[8] == 0x0e)
              {
                if (MySignals_BLE.event[12] == 0x4c)
                {
                  glucometerBLEData.glucose = 0;
                  glucometerBLEData.info = 0xAA;
                  
                  memset(strbuf, 0x00, 30);
                  sprintf(strbuf, "Low glucose");
                  MySignals.println(&(strbuf[0]));
                }
                else if (MySignals_BLE.event[12] == 0x48)
                {
                  glucometerBLEData.glucose = 360;
                  glucometerBLEData.info = 0xBB;

                  memset(strbuf, 0x00, 30);
                  sprintf(strbuf, "High glucose");
                  MySignals.println(&(strbuf[0]));
                }
    
              }
              
              ret = 1;
              MySignals_BLE.disconnect(MySignals_BLE.connection_handle);
              connected_gluco = 0;
            }
    
          }
          while ((connected_gluco == 1) && ((millis() - previous) < 20000)); //Timeout 20 seconds
    
          connected_gluco = 0;
        }
        else
        {
          connected_gluco = 0;
          MySignals.println("Not Connected");
        }
    
    
      }
      else if (available_gluco == 0)
      {
        //Do nothing
      }
      else
      {
        MySignals_BLE.hardwareReset();
        MySignals_BLE.initialize_BLE_values();    
      }

    printf ("end access \n");
    #endif
    return ret;
}

uint8_t startGlucose() {
  memset(&glucometerBLEData, 0x00, sizeof(glucometerBLEDataVector));
#if USE_MYSIGNALS
    uint8_t ret = initMySignals();
    if(ret == 1) {
        ret = getMySignalGlucose();
    }
    return ret;
#else
    return 0;
#endif
}