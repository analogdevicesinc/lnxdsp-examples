/*
Copyright (c) 2021 Analog Devices, Inc.
All Rights Reserved.  This software is proprietary to Analog Devices, Inc. and its licensors.
*/

#include "led_blink.h"
#include <services/gpio/adi_gpio.h>

int init_leds(void){

	ADI_GPIO_RESULT ret;

	/*Configure the Port Pin PC_01 as output for LED blink*/
	ret = adi_gpio_SetDirection(ADI_GPIO_PORT_C, ADI_GPIO_PIN_1, ADI_GPIO_DIRECTION_OUTPUT);
	if(ret!= ADI_GPIO_SUCCESS){
		return ret;
	}

	/*Configure the Port Pin PC_02 as output for LED blink*/
	ret = adi_gpio_SetDirection(ADI_GPIO_PORT_C, ADI_GPIO_PIN_2, ADI_GPIO_DIRECTION_OUTPUT);
	if(ret!= ADI_GPIO_SUCCESS){
		return ret;
	}

	/*Configure the Port Pin PC_03 as output for LED blink*/
	ret = adi_gpio_SetDirection(ADI_GPIO_PORT_C, ADI_GPIO_PIN_3, ADI_GPIO_DIRECTION_OUTPUT);
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
		adi_gpio_Toggle(ADI_GPIO_PORT_C, ADI_GPIO_PIN_1);
		break;
	case 1:
		adi_gpio_Toggle(ADI_GPIO_PORT_C, ADI_GPIO_PIN_2);
		break;
	case 2:
		adi_gpio_Toggle(ADI_GPIO_PORT_C, ADI_GPIO_PIN_3);
		break;
	default:
		break;
	}
}

void turn_on_led(int led){
	switch(led){
	case 0:
		adi_gpio_Set(ADI_GPIO_PORT_C, ADI_GPIO_PIN_1);
		break;
	case 1:
		adi_gpio_Set(ADI_GPIO_PORT_C, ADI_GPIO_PIN_2);
		break;
	case 2:
		adi_gpio_Set(ADI_GPIO_PORT_C, ADI_GPIO_PIN_3);
		break;
	default:
		break;
	}
}

void turn_off_led(int led){
	switch(led){
	case 0:
		adi_gpio_Clear(ADI_GPIO_PORT_C, ADI_GPIO_PIN_1);
		break;
	case 1:
		adi_gpio_Clear(ADI_GPIO_PORT_C, ADI_GPIO_PIN_2);
		break;
	case 2:
		adi_gpio_Clear(ADI_GPIO_PORT_C, ADI_GPIO_PIN_3);
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
	for(i=0;i<0x1000000;i++);
	turn_off_led(0);
	turn_off_led(1);
	turn_off_led(2);
	for(i=0;i<0x1000000;i++);
	turn_on_led(0);
	turn_on_led(1);
	turn_on_led(2);
	for(i=0;i<0x1000000;i++);
	turn_off_led(0);
	turn_off_led(1);
	turn_off_led(2);
	for(i=0;i<0x1000000;i++);
}
