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
#include "linenoise/linenoise.h"
#include "portmacro.h"
#include "ssd1306.h"
#include "pairing.c"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "graphic.cpp"

#include "Cell.h"
#include "Maze.h"
#include "User.h"

static SSD1306_t oled;	

enum STATE {MENU, PLAY};
static enum STATE state = MENU;

enum CHOICE {START = 4, PAIR_GEN, PAIR_FIND};
uint8_t choice = CHOICE::START; 


static void play()
{
	sync_pos_recv[0] = '0';
	sync_pos_recv[1] = '0';
	clearScreen(&oled, BLACK);
	showText(&oled, " Waiting...", 4, BLACK);
	Maze maze;
	User user;
	
	switch(choice)
	{
		case (START):
		{
			maze.gen();
			user.gen(maze);
			Node t = user.des;
			user.des = user.us2;
			user.us2 = t;
			break;
		}
		case (PAIR_GEN):
		{
			maze.gen();
			maze.encode(map_maze);
			user.gen(maze);
			user.encode(sync_pos_send);
			map_maze[128] = (char) (user.des.x + 48);
			map_maze[129] = (char) (user.des.y / 10 + 48);
			map_maze[130] = (char) (user.des.y % 10 + 48);
			map_maze[131] = (char) (user.us2.x + 48);
			map_maze[132] = (char) (user.us2.y / 10 + 48);
			map_maze[133] = (char) (user.us2.y % 10 + 48);
			during(2000);
			pair_with_another(map_maze);
			break;
		}
		case (PAIR_FIND):
		{
			user.fair = 1;
			for (uint8_t i = 0; i < 134;i++) map_maze[i] = '0';
			during(1000);
			pair_with_another(map_maze);
			during(1000);
			maze.decode(map_maze);
			user.des.x = ((int) map_maze[128]) - 48;
			user.des.y = (((int) map_maze[129]) - 48) * 10 + ((int) map_maze[130]) - 48;
			user.us1.x = ((int) map_maze[131]) - 48; 
			user.us1.y = (((int) map_maze[132]) - 48) * 10 + ((int) map_maze[133]) - 48;
			user.encode(sync_pos_send);
			break;
		}
	}
	
	clearScreen(&oled, BLACK);
//	maze.draw(&oled);
	while (state == PLAY)
	{
		user.draw(&oled, maze);
		switch (key_press)
		{
			case (UP):
			{
				if (user.checkMap(maze, user.us1, Node (user.us1.x-1, user.us1.y))) 
				{
					user.move(&oled);
					user.us1.x--;
					user.encode(sync_pos_send);
				}
				key_press = FREE;
				break;
			}
			case (DOWN):
			{
				if (user.checkMap(maze, user.us1, Node (user.us1.x+1, user.us1.y))) 
				{
					user.move(&oled);
					user.us1.x++;
					user.encode(sync_pos_send);
				}
				key_press = FREE;
				break;
			}
			case (RIGHT):
			{
				if (user.checkMap(maze, user.us1, Node (user.us1.x, user.us1.y+1))) 
				{
					user.move(&oled);
					user.us1.y++;
					user.encode(sync_pos_send);
				}
				key_press = FREE;
				break;
			}
			case (LEFT):
			{
				if (user.checkMap(maze, user.us1, Node (user.us1.x, user.us1.y-1))) 
				{
					user.move(&oled);
					user.us1.y--;
					user.encode(sync_pos_send);
				}
				key_press = FREE;
				break;
			}
			case (SETTING):
			{
				state = MENU;
				key_press = FREE;
				esp_restart();
				break;
			}
			default:
			{
				key_press = FREE;
				break;
			}
		}
		
		if (user.win()) 
		{
			state = MENU;
			clearScreen(&oled, BLACK);
			showText(&oled, "   You're win!", 4, BLACK);
			during(2000);
			esp_restart();
		}
		if (sync_pos_recv[0] == '1') 
		{
			if ((user.fair && sync_pos_recv[1] == '0') || (!user.fair && sync_pos_recv[1] == '1'))
			{
				state = MENU;
				clearScreen(&oled, BLACK);
				showText(&oled, "   You're lose!", 4, BLACK);
				during(2000);
				esp_restart();
			}
		}
	}
}

static void menu()
{
	clearScreen(&oled, BLACK);
	while (state == MENU)
	{
		showText(&oled, "   ---MENU---", 1, BLACK);
		showText(&oled, "   Start", 4, BLACK);
		showText(&oled, "   Pair-gen", 5, BLACK);
		showText(&oled, "   Pair-find", 6, BLACK);
		showText(&oled, " >", choice, BLACK);
		switch (key_press)
		{
			case (UP):
			{
				if (choice > START) choice--;
				key_press = FREE;
				break;		
			}
			case (DOWN):
			{
				if (choice < PAIR_FIND) choice++;
				key_press = FREE;
				break;		
			}
			case (SETTING):
			{
				switch (choice)
				{
					case (START):
					{
						state = PLAY;
						key_press = FREE;
						break;
					}
					case (PAIR_GEN):
					{
						state = PLAY;
						key_press = FREE;
						break;
					}
					case (PAIR_FIND):
					{
						state = PLAY;
						key_press = FREE;
						break;
					}
					default:
					{
						break;
					}
				}
				key_press = FREE;
				break;		
			}
			default:
			{
				break;
			}
		}
	}
	clearScreen(&oled,BLACK);
}

extern "C" void app_main()
{	
	initScreen(&oled);
	remoteConfig();
	connect_wifi();
	sync();
	
	while (1)
	{
		switch (state)
		{
			case (MENU):
			{
				menu();
				break;
			}
			case (PLAY):
			{
				play();
				break;
			}
			default:
			{
				break;
			}
		}
	}
}









