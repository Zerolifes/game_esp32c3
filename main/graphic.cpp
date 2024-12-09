/*
 * graphic.cpp
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */
#include "ssd1306.h" 
#include <stdint.h>
#include <string.h>

#define SDA_GPIO 0
#define SCL_GPIO 1
#define RESET_GPIO -1
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define BLACK 0
#define WHITE 1

void showText(SSD1306_t *screen,char *text, uint8_t page, uint8_t background)
{
	ssd1306_display_text(screen, page, text, strlen(text), background);
}

void during(int ms)
{
	vTaskDelay(ms / portTICK_PERIOD_MS);
}

void clearScreen(SSD1306_t *screen, uint8_t color)
{
	ssd1306_clear_screen(screen, color);
}

void initScreen(SSD1306_t *screen)
{
	i2c_master_init(screen, SDA_GPIO, SCL_GPIO, RESET_GPIO);
	ssd1306_init(screen, OLED_WIDTH, OLED_HEIGHT);	
	ssd1306_contrast(screen, 0xff);
}
