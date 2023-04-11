/* SPDX-License-Identifier: Apache-2.0 */
/*
  Copyright (c) 2021-2023, Analog Devices, Inc.  All rights reserved.
*/
#ifndef LED_BLINK_H_
#define LED_BLINK_H_

typedef enum {
	LED,
	LED9,
	LED10
} LED_NUM;

int init_leds(void);
void test_leds(void);
void toggle_led(int led);
void turn_on_led(int led);
void turn_off_led(int led);

#endif /* LED_BLINK_H_ */
