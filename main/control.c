/*
 * remote.c
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "portmacro.h"
#include "soc/gpio_num.h"

#define BUTTON_UP GPIO_NUM_6
#define BUTTON_RIGHT GPIO_NUM_4
#define BUTTON_DOWN GPIO_NUM_5
#define BUTTON_LEFT GPIO_NUM_7
#define BUTTON_SETTING GPIO_NUM_8

#define DEBOUNCE_DELAY_MS 200 

enum KEY {UP, DOWN, LEFT, RIGHT, SETTING, FREE};

static enum KEY key_press = FREE;

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    TickType_t last_interrupt_time = 0;
    for (;;) 
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) 
        {
			TickType_t current_time = xTaskGetTickCount();
			if ((current_time - last_interrupt_time) < pdMS_TO_TICKS(DEBOUNCE_DELAY_MS)) continue;
			last_interrupt_time = current_time;
           	switch (io_num)
           	{
				case (BUTTON_UP):
				{
					key_press = UP;
					break;
				}
				case (BUTTON_RIGHT):
				{
					key_press = RIGHT;
					break;
				}	
				case (BUTTON_DOWN):
				{
					key_press = DOWN;
					break;
				}
				case (BUTTON_LEFT):
				{
					key_press = LEFT;
					break;
				}
				case (BUTTON_SETTING):
				{
					key_press = SETTING;
					break;
				}   
				default:
				{
					key_press = FREE;
					break;
				}
			}
        }
    }
}

static void remoteConfig()
{
	gpio_config_t button = {};
	button.intr_type = GPIO_INTR_NEGEDGE;
	button.mode = GPIO_MODE_INPUT;
	button.pull_up_en = GPIO_PULLUP_ENABLE;
	button.pull_down_en = GPIO_PULLDOWN_DISABLE;
	button.pin_bit_mask = ((1ULL << BUTTON_UP) | (1 << BUTTON_DOWN) | (1ULL << BUTTON_LEFT) | (1ULL << BUTTON_RIGHT) | (1ULL << BUTTON_SETTING));
	
	gpio_config(&button);
	
	gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);
	gpio_install_isr_service(0);
	gpio_isr_handler_add(BUTTON_UP, gpio_isr_handler , (void*) BUTTON_UP);
	gpio_isr_handler_add(BUTTON_DOWN, gpio_isr_handler , (void*) BUTTON_DOWN);
	gpio_isr_handler_add(BUTTON_LEFT, gpio_isr_handler , (void*) BUTTON_LEFT);
	gpio_isr_handler_add(BUTTON_RIGHT, gpio_isr_handler , (void*) BUTTON_RIGHT);
	gpio_isr_handler_add(BUTTON_SETTING, gpio_isr_handler, (void*) BUTTON_SETTING);
}




















