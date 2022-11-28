/* SPDX-License-Identifier: Apache-2.0 */
/*
  Copyright (c) 2021-2022, Analog Devices, Inc.  All rights reserved.
*/

#include "led_blink.h"
#include <rpmsg_platform.h>
#include <services/gpio/adi_gpio.h>

#define ADI_LED1_PORT ADI_GPIO_PORT_E
#define ADI_LED2_PORT ADI_GPIO_PORT_E
#define ADI_LED3_PORT ADI_GPIO_PORT_E
#define ADI_LED1_PIN ADI_GPIO_PIN_1
#define ADI_LED2_PIN ADI_GPIO_PIN_2
#define ADI_LED3_PIN ADI_GPIO_PIN_3

int init_leds(void){

	ADI_GPIO_RESULT ret;

	ret = adi_gpio_SetDirection(ADI_LED1_PORT, ADI_LED1_PIN, ADI_GPIO_DIRECTION_OUTPUT);
	if(ret!= ADI_GPIO_SUCCESS){
		return ret;
	}

	ret = adi_gpio_SetDirection(ADI_LED2_PORT, ADI_LED2_PIN, ADI_GPIO_DIRECTION_OUTPUT);
	if(ret!= ADI_GPIO_SUCCESS){
		return ret;
	}

	ret = adi_gpio_SetDirection(ADI_LED3_PORT, ADI_LED3_PIN, ADI_GPIO_DIRECTION_OUTPUT);
	if(ret!= ADI_GPIO_SUCCESS){
		return ret;
	}
	turn_off_led(0);
	turn_off_led(1);
	turn_off_led(2);
	return 0;
}

void toggle_led(int led){
	switch(led){
	case 0:
		adi_gpio_Toggle(ADI_LED1_PORT, ADI_LED1_PIN);
		break;
	case 1:
		adi_gpio_Toggle(ADI_LED2_PORT, ADI_LED2_PIN);
		break;
	case 2:
		adi_gpio_Toggle(ADI_LED3_PORT, ADI_LED3_PIN);
		break;
	default:
		break;
	}
}

void turn_on_led(int led){
	switch(led){
	case 0:
		adi_gpio_Set(ADI_LED1_PORT, ADI_LED1_PIN);
		break;
	case 1:
		adi_gpio_Set(ADI_LED2_PORT, ADI_LED2_PIN);
		break;
	case 2:
		adi_gpio_Set(ADI_LED3_PORT, ADI_LED3_PIN);
		break;
	default:
		break;
	}
}

void turn_off_led(int led){
	switch(led){
	case 0:
		adi_gpio_Clear(ADI_LED1_PORT, ADI_LED1_PIN);
		break;
	case 1:
		adi_gpio_Clear(ADI_LED2_PORT, ADI_LED2_PIN);
		break;
	case 2:
		adi_gpio_Clear(ADI_LED3_PORT, ADI_LED3_PIN);
		break;
	default:
		break;
	}
}

void test_leds(void){
	volatile int i;
	turn_on_led(0);
	turn_on_led(1);
	turn_on_led(2);
	platform_time_delay(500);
	turn_off_led(0);
	turn_off_led(1);
	turn_off_led(2);
	platform_time_delay(500);
	turn_on_led(0);
	turn_on_led(1);
	turn_on_led(2);
	platform_time_delay(500);
	turn_off_led(0);
	turn_off_led(1);
	turn_off_led(2);
	platform_time_delay(500);
}
