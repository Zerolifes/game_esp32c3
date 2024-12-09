/*
 * cell.h
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */

#ifndef MAIN_CELL_H_
#define MAIN_CELL_H_
#include "ssd1306.h"
#include <stdint.h>
#include <string.h>

class Cell 
{
public:
	Cell(uint8_t row = 0, uint8_t col = 0, uint8_t type = 0);
	uint8_t row;
	uint8_t col;
	uint8_t shape[8];
	void draw(SSD1306_t *screen);
	virtual ~Cell();
};

#endif /* MAIN_CELL_H_ */
