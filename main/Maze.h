/*
 * Maze.h
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */

#ifndef MAIN_MAZE_H_
#define MAIN_MAZE_H_

#include "ssd1306.h"
#include <stdint.h>
class Maze {
public:
	Maze();
	uint8_t grid[8][16];
	void draw(SSD1306_t *screen);
	virtual ~Maze();
};

#endif /* MAIN_MAZE_H_ */
