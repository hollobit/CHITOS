#include "controlboard.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include <signal.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#include "MySignals.h"
#include "MySignals_BLE.h"
#include "../bcm2835-1.46/src/bcm2835.h"
#include "common.h"
#include "bloodpressure.h"
#include "glucose.h"
#include "scale.h"

int led1 = 13;
int led2 = 19;
int led3 = 26;
int btn1 = 6;
int btn2 = 5;
int btn3 = 0;

#define LED_ON LOW2
#define LED_OFF HIGH2
#define BTN_PRESSED 0

char command;

void setupControlBoard(char c) {
    command = c;
    bcm2835_init();
    if(command == 'P') {
        bcm2835_gpio_fsel(led1, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(led1, LED_OFF);
        bcm2835_gpio_fsel(btn1, BCM2835_GPIO_FSEL_INPT);
    } else if(command == 'G') {
        bcm2835_gpio_fsel(led2, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(led2, LED_OFF);
        bcm2835_gpio_fsel(btn2, BCM2835_GPIO_FSEL_INPT);
    } else if(command == 'S') {
        bcm2835_gpio_fsel(led3, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(led3, LED_OFF);
        bcm2835_gpio_fsel(btn3, BCM2835_GPIO_FSEL_INPT);
    }    
}

int pre_btn1state = 1;
int pre_btn2state = 1;
int pre_btn3state = 1; 

int blink_run = 0;
int blink_led = 0;
int blink_state = HIGH2;

int isRunBlink() {
    return blink_run;
}

void blinkLed() {
    if(blink_state == HIGH2) {
        blink_state = LOW2;
    } else {
        blink_state = HIGH2;
    }
    bcm2835_gpio_write(blink_led, blink_state);
}

void runBloodPressure() {
    // led thread start
    blink_run = 1;
    blink_led = led1;        

    // ble work
    startBloodPressure();

    // result led
    if(getSystolic() > 0) {
        bcm2835_gpio_write(led1, LED_ON);
    } else {
        bcm2835_gpio_write(led1, LED_OFF);
    }

    // end
    blink_run = 0;
    blink_state = HIGH2;
}

void runGlucose() {
    // led thread start
    blink_run = 1;
    blink_led = led2;

    // ble work
    startGlucose();

    // result led
    if(getGlucose() > 0) { 
        bcm2835_gpio_write(led2, LED_ON);
    } else {
        bcm2835_gpio_write(led2, LED_OFF);
    }

    // end
    blink_run = 0;
    blink_state = HIGH2;
}

void runScale() {
    // led thread start
    blink_run = 1;
    blink_led = led3;
    
    // ble work
    startScale();

    // result led
    if(getWeight() > 0) {
        bcm2835_gpio_write(led3, LED_ON);
    } else {
        bcm2835_gpio_write(led3, LED_OFF);
    }

    // end
    blink_run = 0;
    blink_state = HIGH2;
}

void runControlBoardMainLoop() {
    int btn1state;
    int btn2state;
    int btn3state;

    if(command == 'P') {
        btn1state = bcm2835_gpio_lev(btn1);
        if (btn1state == BTN_PRESSED && pre_btn1state != BTN_PRESSED) {
            runBloodPressure();
        }
        pre_btn1state = btn1state;
    } else if(command == 'G') {
        btn2state = bcm2835_gpio_lev(btn2);
        if (btn2state == BTN_PRESSED && pre_btn2state != BTN_PRESSED) {
            runGlucose();
        }
        pre_btn2state = btn2state;
    } else if(command == 'S') {
        btn3state = bcm2835_gpio_lev(btn3);
        if (btn3state == BTN_PRESSED && pre_btn3state != BTN_PRESSED) {
            runScale();
        }
        pre_btn3state = btn3state;
    } 
}