/*
 * Maze.cpp
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */

#include "Maze.h"
#include "Cell.h"
#include "ssd1306.h"
#include <stdint.h>
#include <time.h>

Maze::Maze() 
{
	srand(time(NULL));
	for (uint8_t i=0;i<8;i++)
	{
		for (uint8_t j=0;j<16;j++)
		{
			grid[i][j] = rand() % 4; 
		}
	}
}

void Maze::draw(SSD1306_t *screen)
{
	for (uint8_t i=0;i<8;i++)
	{
		for (uint8_t j=0;j<16;j++)
		{
			Cell cell(i, j, grid[i][j]);
			cell.draw(screen);
		}
	}	
}

Maze::~Maze() {}

