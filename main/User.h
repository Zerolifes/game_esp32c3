/*
 * User.h
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */

#ifndef MAIN_USER_H_
#define MAIN_USER_H_

#include "Maze.h"
#include "ssd1306.h"
#include <stdint.h>

struct Node 
{
	int8_t x;
	int8_t y;
	Node (int8_t x = 0, int8_t y = 0)
	{
		this->x = x;
		this->y = y;
	}
};


class User {
public:
	Node us1;
	Node us2;
	Node des;
	uint8_t far = 0;
	bool fair = 0;
	User();
	void gen(Maze &maze);
	void encode(char *loc);
	void decode(char *loc);
	void bfs(Maze &maze, Node start);
	void getLoc(uint8_t dis, Node start, Node end);
	bool checkMap(Maze &maze, Node start, Node end);
	void draw(SSD1306_t *screen, Maze &maze);
	void move(SSD1306_t *screen);
	void getDes(Maze &maze, Node start, Node end);
	bool win();
	bool lose();
	virtual ~User();
};

#endif /* MAIN_USER_H_ */
