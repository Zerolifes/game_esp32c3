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
#include <string.h>

Maze::Maze() {}

void Maze::gen()
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

void Maze::encode(char *map)
{
	for (uint8_t i=0;i<128;i++) map[i] = '0';
	for (uint8_t i = 0;i<128; i++)
	{
		map[i] = (char) (grid[i/16][i%16] + 48);
	}
}

void Maze::decode(char *map)
{
	for (uint8_t i = 0; i<128;i++) grid[i/16][i%16] = ((int)map[i]) - 48;
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

