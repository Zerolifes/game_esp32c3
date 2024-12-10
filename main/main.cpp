/*
 * main.cpp
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */
 
#include "control.c"

#include "esp_system.h"
#include "esp_log.h"
#include <stdio.h>
#include "portmacro.h"
#include "ssd1306.h"
#include <stdint.h>
#include <string.h>
#include "graphic.cpp"

#include "Cell.h"
#include "Maze.h"
#include "User.h"

extern "C" void app_main()
{	
	remoteConfig();
	SSD1306_t oled;	
	initScreen(&oled);
	clearScreen(&oled, BLACK);

	
	showText(&oled, "     Start", 3, BLACK);
	showText(&oled, "      Pair", 4, BLACK);
	during(1000);
	clearScreen(&oled, BLACK);
	
	Maze maze;
	
	User user(maze);
	usx = user.us1.x;
	usy = user.us1.y; 
	
	while (gameLoop)
	{
		if (user.checkMap(maze, user.us1, Node (usx, usy)))
		{
			user.move(&oled, maze, user.us1, Node(usx, usy));
		}
		else 
		{
			usx = user.us1.x;
			usy = user.us1.y;
		}
		user.draw(&oled, maze);
		if (user.us1.x == user.des.x && user.us1.y == user.des.y) gameLoop = 0;
	}
	
	clearScreen(&oled, BLACK);
	showText(&oled, "     You're win!", 3, BLACK);
	during(2000);
	
	esp_restart();
}










