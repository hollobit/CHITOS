#include "MySignals.h"
#include "MySignals_BLE.h"

uint8_t initMySignals() {
    uint8_t ret = 0;
    Serial.begin(115200);
    MySignals.begin();   
  
    MySignals.initSensorUART();
    MySignals.enableSensorUART(BLE);

     //Enable BLE module power -> bit6: 1
    bitSet(MySignals.expanderState, EXP_BLE_POWER); // 6
    MySignals.expanderWrite(MySignals.expanderState);

    //Enable BLE UART flow control -> bit5: 0
    bitClear(MySignals.expanderState, EXP_BLE_FLOW_CONTROL); //5
    MySignals.expanderWrite(MySignals.expanderState);

    //Disable BLE module power -> bit6: 0
    bitClear(MySignals.expanderState, EXP_BLE_POWER);  //6
    MySignals.expanderWrite(MySignals.expanderState);
  
    delay(500);
  
    //Enable BLE module power -> bit6: 1
    bitSet(MySignals.expanderState, EXP_BLE_POWER);  //6
    MySignals.expanderWrite(MySignals.expanderState);
  
    delay(1000);

    MySignals_BLE.initialize_BLE_values();
    if (MySignals_BLE.initModule() == 1)
    {
      if (MySignals_BLE.sayHello() == 1)
      {
        MySignals.println("BLE init ok");
        ret = 1;
      }
      else
      {
        MySignals.println("BLE init fail#2");        
      }
    }
    else
    {
      MySignals.println("BLE init fail#1");
      
    }

    return ret;
}