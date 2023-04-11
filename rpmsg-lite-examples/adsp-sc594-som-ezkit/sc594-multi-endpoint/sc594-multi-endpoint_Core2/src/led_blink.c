/* SPDX-License-Identifier: Apache-2.0 */
/*
  Copyright (c) 2021-2023, Analog Devices, Inc.  All rights reserved.
*/

#include "led_blink.h"
#include <rpmsg_platform.h>
#include <services/gpio/adi_gpio.h>

#define ADI_LED9_PORT ADI_GPIO_PORT_C
#define ADI_LED10_PORT ADI_GPIO_PORT_C
#define ADI_LED_PORT ADI_GPIO_PORT_C
#define ADI_LED9_PIN ADI_GPIO_PIN_3
#define ADI_LED10_PIN ADI_GPIO_PIN_2
#define ADI_LED_PIN ADI_GPIO_PIN_1

int init_leds(void){

	ADI_GPIO_RESULT ret;

	ret = adi_gpio_SetDirection(ADI_LED_PORT, ADI_LED_PIN, ADI_GPIO_DIRECTION_OUTPUT);
	if(ret!= ADI_GPIO_SUCCESS){
		return ret;
	}

	ret = adi_gpio_SetDirection(ADI_LED9_PORT, ADI_LED9_PIN, ADI_GPIO_DIRECTION_OUTPUT);
	if(ret!= ADI_GPIO_SUCCESS){
		return ret;
	}

	ret = adi_gpio_SetDirection(ADI_LED10_PORT, ADI_LED10_PIN, ADI_GPIO_DIRECTION_OUTPUT);
	if(ret!= ADI_GPIO_SUCCESS){
		return ret;
	}

	turn_off_led(LED);
	turn_off_led(LED9);
	turn_off_led(LED10);

	return 0;
}

void toggle_led(int led){
	switch(led){
	case LED:
		adi_gpio_Toggle(ADI_LED_PORT, ADI_LED_PIN);
		break;
	case LED9:
		adi_gpio_Toggle(ADI_LED9_PORT, ADI_LED9_PIN);
		break;
	case LED10:
		adi_gpio_Toggle(ADI_LED10_PORT, ADI_LED10_PIN);
		break;
	default:
		break;
	}
}

void turn_on_led(int led){
	switch(led){
	case LED:
		adi_gpio_Set(ADI_LED_PORT, ADI_LED_PIN);
		break;
	case LED9:
		adi_gpio_Set(ADI_LED9_PORT, ADI_LED9_PIN);
		break;
	case LED10:
		adi_gpio_Set(ADI_LED10_PORT, ADI_LED10_PIN);
		break;
	default:
		break;
	}
}

void turn_off_led(int led){
	switch(led){
	case LED:
		adi_gpio_Clear(ADI_LED_PORT, ADI_LED_PIN);
		break;
	case LED9:
		adi_gpio_Clear(ADI_LED9_PORT, ADI_LED9_PIN);
		break;
	case LED10:
		adi_gpio_Clear(ADI_LED10_PORT, ADI_LED10_PIN);
		break;
	default:
		break;
	}
}

void test_leds(void){
	volatile int i;
	turn_on_led(LED);
	turn_on_led(LED9);
	turn_on_led(LED10);
	platform_time_delay(500);
	turn_off_led(LED);
	turn_off_led(LED9);
	turn_off_led(LED10);
	platform_time_delay(500);
	turn_on_led(LED);
	turn_on_led(LED9);
	turn_on_led(LED10);
	platform_time_delay(500);
	turn_off_led(LED);
	turn_off_led(LED9);
	turn_off_led(LED10);
	platform_time_delay(500);
}
