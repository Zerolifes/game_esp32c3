/*
 * main.cpp
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */
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
	SSD1306_t oled;	
	initScreen(&oled);
	clearScreen(&oled, BLACK);
	
	showText(&oled, "   Start", 3, BLACK);
	showText(&oled, "   Pair", 4, BLACK);
	during(1000);
	
	Maze maze;
	maze.draw(&oled);
	
	User user(maze);
	
	bool gameLoop = 1;
	
	while (gameLoop)
	{
		user.draw(&oled, maze);
		
	}
	
	
	esp_restart();
}










