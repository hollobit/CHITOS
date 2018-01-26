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
#include "scale.h"


// Write here the MAC address of BLE device to find
char MAC_SCALE[14] = "881B9909610E";

//Scale User profile
uint8_t user_gender = 1;   //1-Male    0-Female
uint8_t user_height = 172; //in cm (0-255)
uint8_t user_age = 39;     //(0-255)

uint8_t available_scale = 0;
uint8_t connected_scale = 0;
uint8_t connection_handle_scale = 0;

#define SCALE_HANDLE 83
#define SCALE_PROFILE_HANDLE 85

//!Struct to store data of the glucometer.
struct scaleDataVector
{
  uint16_t weight;
  uint16_t bodyfat;
  uint16_t musclemass;
  uint16_t water;
  uint16_t calories;
  uint8_t visceralfat;
  uint8_t bonemass;
};

//!Vector to store the glucometer measures and dates.
scaleDataVector scaleData;

void setBasicData (uint8_t gender, uint8_t height, uint8_t age) {
    user_gender = gender;
    user_height = height;
    user_age = age;
}

double getBMI() {
    double w = getWeight();
    double h = (getHeight() * getHeight());
    return (w / h);
}

double getBodyFat() {
    double bf_percentage = (double(scaleData.bodyfat) / 10);
    return (getWeight() * (bf_percentage/100.0f));
}

double getBodyFatFreeMass() {
    double w = getWeight();
    return (w - getBodyFat());
}

double getBodySoftLeanMass() {
    return getBodyFatFreeMass();
}

double getBodyWater() {
    double w_percentage = (double(scaleData.water) / 10);
    return (getWeight() * (w_percentage/100.0f));
}

double getHeight() {
    return (double(user_height) / 100);
}
  
void setHeight(double height) {
  user_height = height * 100;
}

uint8_t getAge() {
  return user_age;
}

void setAge(uint8_t age) {
  user_age = age;
}

uint8_t getGender() {
  return user_gender;
}

void setGender(uint8_t gender) {
  user_gender = gender;
}


double getWeight () {
    return (double(scaleData.weight) / 10);  
}

uint8_t getMySignalScale() {
    uint8_t ret = 0;
    #if USE_HW
    char strbuf[30] = {0x00, };
    available_scale = MySignals_BLE.scanDevice(MAC_SCALE, 1000, TX_POWER_MAX);
  
    MySignals.disableMuxUART();
    memset(strbuf, 0x00, 30);
    sprintf(strbuf, "Scale available: %d\n", available_scale);  
    printf(strbuf);
  //   MySignals.print("Scale available:");
  //   MySignals.println(available_scale);
    MySignals.enableMuxUART();
  
    printf ("start access \n");
    if (available_scale == 1)
    {
  
      if (MySignals_BLE.connectDirect(MAC_SCALE) == 1)
      {
        MySignals.println("Connected");
  
        connected_scale = 1;
        delay(500);
  
        uint8_t attributeData[2] =
        {
          0x01 , 0x00
        };
        if (MySignals_BLE.attributeWrite(MySignals_BLE.connection_handle, SCALE_HANDLE, attributeData, 2) == 0)
        {
  
          MySignals.println("Subscribed");
  
          delay(4000);
  
          uint8_t scale_user_profile[8] =
          {
            0xfe,  // 0  BYTE 1
            0x00,  // 1  BYTE 2 -> user group
            0x00,  // 2  BYTE 3 -> gender
            0x00,  // 3  BYTE 4 -> level
            0x00,  // 4  BYTE 5 -> height
            0x00,  // 5  BYTE 6 -> age
            0x00,  // 6  BYTE 7 -> unit
            0x00   // 7  BYTE 8 -> xor
          };
  
          scale_user_profile[1] = 1;              // User group
          scale_user_profile[2] = user_gender;    // gender: 1=male, 0=female
          scale_user_profile[3] = 0;              // level 0=normal
          scale_user_profile[4] = user_height;    // height
          scale_user_profile[5] = user_age;       // age
          scale_user_profile[6] = 1;              // unit KG
  
  
          uint8_t xor_result = scale_user_profile[1] xor scale_user_profile[2];
          xor_result = xor_result xor scale_user_profile[3];
          xor_result = xor_result xor scale_user_profile[4];
          xor_result = xor_result xor scale_user_profile[5];
          scale_user_profile[7] = xor_result xor scale_user_profile[6];
  
  
          if (MySignals_BLE.attributeWrite(MySignals_BLE.connection_handle, SCALE_PROFILE_HANDLE, scale_user_profile, 8) == 0)
          {
            delay(500);
  
            unsigned long previous = millis();
            do
            {
              if (MySignals_BLE.waitEvent(1000) == BLE_EVENT_ATTCLIENT_ATTRIBUTE_VALUE)
              {
  
                uint8_t scale_weight_high = MySignals_BLE.event[13];
                uint8_t scale_weight_low = MySignals_BLE.event[14];
  
                uint8_t scale_bodyfat_high = MySignals_BLE.event[15];
                uint8_t scale_bodyfat_low = MySignals_BLE.event[16];
  
                uint8_t scale_musclemass_high = MySignals_BLE.event[18];
                uint8_t scale_musclemass_low = MySignals_BLE.event[19];
  
                scaleData.visceralfat = MySignals_BLE.event[20];
  
                uint8_t scale_water_high = MySignals_BLE.event[21];
                uint8_t scale_water_low = MySignals_BLE.event[22];
  
                uint8_t scale_calories_high = MySignals_BLE.event[23];
                uint8_t scale_calories_low = MySignals_BLE.event[24];
  
  
                scaleData.weight = (scale_weight_high * 256) + scale_weight_low;
                scaleData.bodyfat = (scale_bodyfat_high * 256) + scale_bodyfat_low;
                scaleData.musclemass = (scale_musclemass_high * 256) + scale_musclemass_low;
                scaleData.water = (scale_water_high * 256) + scale_water_low;
                scaleData.calories = (scale_calories_high * 256) + scale_calories_low;
  
                scaleData.bonemass = MySignals_BLE.event[17] * 1000 / scaleData.weight;
  
                MySignals.disableMuxUART();
                
                MySignals.println();
                memset(strbuf, 0x00, 30);
                sprintf(strbuf, "Weight: %.2fKg", (float(scaleData.weight) / 10));
                MySignals.println(&(strbuf[0]));
  
                memset(strbuf, 0x00, 30);
                sprintf(strbuf, "Body fat: %.2f%%", (float(scaleData.bodyfat) / 10));
                MySignals.println(&(strbuf[0]));
  
                memset(strbuf, 0x00, 30);
                sprintf(strbuf, "Bone mass: %.2f%%", (float(scaleData.bonemass) / 10));
                MySignals.println(&(strbuf[0]));
  
                memset(strbuf, 0x00, 30);
                sprintf(strbuf, "Muscle mass: %.2f%%", (float(scaleData.musclemass) / 10));
                MySignals.println(&(strbuf[0]));
  
                memset(strbuf, 0x00, 30);
                sprintf(strbuf, "Visceral fat: %.2f%%", scaleData.visceralfat);
                MySignals.println(&(strbuf[0]));
  
                memset(strbuf, 0x00, 30);
                sprintf(strbuf, "Water percentage: %.2f%%", (float(scaleData.water) / 10));
                MySignals.println(&(strbuf[0]));
  
                memset(strbuf, 0x00, 30);
                sprintf(strbuf, "Calories: %dKcal", scaleData.calories);
                MySignals.println(&(strbuf[0]));
                
                MySignals.enableMuxUART();
  
                delay(1000);
  
                uint8_t shutdown_command[8] =
                {
                  0xfd, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x35
                };
  
                MySignals_BLE.attributeWrite(MySignals_BLE.connection_handle, SCALE_PROFILE_HANDLE, shutdown_command, 8);
  
                MySignals_BLE.disconnect(MySignals_BLE.connection_handle);
  
                delay(200);
  
                connected_scale = 0;

                ret = 1;
              }
            }
            while ((connected_scale == 1) && ((millis() - previous) < 20000));
  
            connected_scale = 0;
  
          }
          else
          {
            MySignals.println("Error subscribing");
          }
        }
        else
        {
          MySignals.println("Error subscribing");
        }
      }
      else
      {
        connected_scale = 0;
        MySignals.println("Not Connected");
      }
    }
    else if (available_scale == 0)
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

uint8_t startScale() {
  memset(&scaleData, 0x00, sizeof(scaleDataVector));
#if USE_MYSIGNALS
    uint8_t ret = initMySignals();
    if(ret == 1) {
        ret = getMySignalScale();
    }
    return ret;
#else
    return 0;
#endif
}