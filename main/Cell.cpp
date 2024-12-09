/*
 * cell.cpp
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */

#include "cell.h"
#include "ssd1306.h"
#include <stdint.h>
#include <string.h>

Cell::Cell(uint8_t row, uint8_t col, uint8_t type) 
{
	this->row = row;
	this->col = col;
	switch (type)
	{
		case 0:
		{
			for (uint8_t i=1; i<8; i++) shape[i] = 0b00000001;
			shape[0] = 0b11111111;
			break;
		}	
		case 1:
		{
			for (uint8_t i=1;i<8;i++) shape[i] = 0b00000000;
			shape[0] = 0b11111111;
			break;
		}
		case 2:
		{
			for (uint8_t i=0;i<8;i++) shape[i] = 0b00000001;
			break;
		}
		case 3:
		{
			memset(shape, 0, sizeof(shape));
			break;
		}
		default:
		{
			break;
		}
	}
}

void Cell::draw(SSD1306_t *screen)
{
	ssd1306_display_image(screen, row, col*8, shape, 8);
}

Cell::~Cell() {}

