/*
Copyright (c) 2021 Analog Devices, Inc.
All Rights Reserved.  This software is proprietary to Analog Devices, Inc. and its licensors.
*/

#ifndef LED_BLINK_H_
#define LED_BLINK_H_

#define LED_HEARTBEAT 0
#define LED_COM 1
#define LED_ERROR 2

int init_leds(void);
void test_leds(void);
void toggle_led(int led);
void turn_on_led(int led);
void turn_off_led(int led);

#endif /* LED_BLINK_H_ */
