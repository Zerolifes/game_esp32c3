/*
 * User.cpp
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */

#include "User.h"
#include "Maze.h"
#include "esp_mesh.h"
#include "ssd1306.h"
#include <stdint.h>
#include <string.h>
#include "Cell.h"

User::User() {}

void User::gen(Maze &maze)
{
	for (uint8_t x = 0; x<8; x++)
	{
		for (uint8_t y = 0; y<16; y++)
		{
			bfs(maze, Node(x, y));	
		}
	}
	getDes(maze, us1, us2);
}


void User::encode(char *loc)
{
	loc[0] = (char) (us1.x+48); loc[1] = (char) (us1.y / 10 + 48); loc[2] = (char) (us1.y % 10 + 48);
	if (fair) loc[3] = '1'; else loc[3] = '0';
}

void User::decode(char *loc)
{
	
}


void User::getDes(Maze &maze, Node start, Node end)
{
	Node que[128];
	que[0] = start;
	uint8_t l = 0;
	uint8_t r = 0;
	uint8_t dis[8][16];
	Node par[8][16];
	bool vis[8][16];
	memset(dis, 0, sizeof(dis));
	memset(vis, 0, sizeof(vis));
	vis[start.x][start.y] = 1;
	while (l<=r)
	{
		Node u = que[l]; l++;
		Node v;
		v.x = u.x + 1; v.y = u.y;
		if (!vis[v.x][v.y] && v.x < 8 && checkMap(maze, u, v))
		{
			vis[v.x][v.y] = 1;
			dis[v.x][v.y] = dis[u.x][u.y] + 1;
			par[v.x][v.y] = Node(u.x, u.y);
			if (v.x == end.x && v.y == end.y) break;
			r++;
			que[r] = v;
		}
		v.x = u.x - 1; v.y = u.y;
		if (!vis[v.x][v.y] && v.x >= 0 && checkMap(maze, u, v))
		{
			vis[v.x][v.y] = 1;
			dis[v.x][v.y] = dis[u.x][u.y] + 1;
			par[v.x][v.y] = Node(u.x, u.y);
			if (v.x == end.x && v.y == end.y) break;
			r++;
			que[r] = v;
		}
		v.x = u.x; v.y = u.y + 1;
		if (!vis[v.x][v.y] && v.y < 16 && checkMap(maze, u, v))
		{
			vis[v.x][v.y] = 1;
			dis[v.x][v.y] = dis[u.x][u.y] + 1;
			par[v.x][v.y] = Node(u.x, u.y);
			if (v.x == end.x && v.y == end.y) break;
			r++;
			que[r] = v;
		}
		v.x = u.x; v.y = u.y - 1;
		if (!vis[v.x][v.y] && v.y >= 0 && checkMap(maze, u, v))
		{
			vis[v.x][v.y] = 1;
			dis[v.x][v.y] = dis[u.x][u.y] + 1;
			par[v.x][v.y] = Node(u.x, u.y);
			if (v.x == end.x && v.y == end.y) break;
			r++;
			que[r] = v;
		}
	}
	des = end;
	for (uint8_t i=0;i<dis[end.x][end.y]/2;i++)
	{
		des = par[des.x][des.y];
	}
}

void User::bfs(Maze &maze, Node start)
{
	Node que[128];
	que[0] = start;
	uint8_t l = 0;
	uint8_t r = 0;
	uint8_t dis[8][16];
	bool vis[8][16];
	memset(dis, 0, sizeof(dis));
	memset(vis, 0, sizeof(vis));
	vis[start.x][start.y] = 1;
	while (l<=r)
	{
		Node u = que[l]; l++;
		Node v;
		v.x = u.x + 1; v.y = u.y;
		if (!vis[v.x][v.y] && v.x < 8 && checkMap(maze, u, v))
		{
			vis[v.x][v.y] = 1;
			dis[v.x][v.y] = dis[u.x][u.y] + 1;
			getLoc(dis[v.x][v.y], start, v);
			r++;
			que[r] = v;
		}
		v.x = u.x - 1; v.y = u.y;
		if (!vis[v.x][v.y] && v.x >= 0 && checkMap(maze, u, v))
		{
			vis[v.x][v.y] = 1;
			dis[v.x][v.y] = dis[u.x][u.y] + 1;
			getLoc(dis[v.x][v.y], start, v);
			r++;
			que[r] = v;
		}
		v.x = u.x; v.y = u.y + 1;
		if (!vis[v.x][v.y] && v.y < 16 && checkMap(maze, u, v))
		{
			vis[v.x][v.y] = 1;
			dis[v.x][v.y] = dis[u.x][u.y] + 1;
			getLoc(dis[v.x][v.y], start, v);
			r++;
			que[r] = v;
		}
		v.x = u.x; v.y = u.y - 1;
		if (!vis[v.x][v.y] && v.y >= 0 && checkMap(maze, u, v))
		{
			vis[v.x][v.y] = 1;
			dis[v.x][v.y] = dis[u.x][u.y] + 1;
			getLoc(dis[v.x][v.y], start, v);
			r++;
			que[r] = v;
		}
	}
}

void User::getLoc(uint8_t dis, Node start, Node end)
{
	if (dis > far)
	{
		far = dis;
		us1 = start;
		us2 = end;
	}
}

bool User::checkMap(Maze &maze, Node start, Node end)
{
	if (end.x < 0 || end.x > 7 || end.y < 0 || end.y > 15) return 0;
	if (start.x == end.x && start.y == end.y) return 0;
	if (start.x == end.x)
	{
		if (start.y < end.y)
		{
			if (maze.grid[end.x][end.y] < 2) return 0;
		}
		else 
		{
			if (maze.grid[start.x][start.y] < 2) return 0;
		}
	}
	else 
	{
		if (start.x < end.x)
		{
			if (maze.grid[end.x][end.y] % 2 == 0) return 0;
		}
		else 
		{
			if (maze.grid[start.x][start.y] % 2 == 0) return 0;
		}
	}
	return 1;
}

void User::draw(SSD1306_t *screen, Maze &maze)
{
	Cell cell(us1.x, us1.y, maze.grid[us1.x][us1.y]);
	cell.shape[3] = cell.shape[3] | 0b00011000;
	cell.shape[4] = cell.shape[4] | 0b00111100;
	cell.shape[5] = cell.shape[5] | 0b00111100;
	cell.shape[6] = cell.shape[6] | 0b00011000;
	cell.draw(screen);
	if (us1.x > 0)
	{
		Cell cell(us1.x-1, us1.y, maze.grid[us1.x-1][us1.y]);
		cell.draw(screen);
	} 
	if (us1.x < 7)
	{
		Cell cell(us1.x+1, us1.y, maze.grid[us1.x+1][us1.y]);
		cell.draw(screen);
	}
	if (us1.y > 0)
	{
		Cell cell(us1.x, us1.y-1, maze.grid[us1.x][us1.y-1]);
		cell.draw(screen);
	} 
	if (us1.x < 15)
	{
		Cell cell(us1.x, us1.y+1, maze.grid[us1.x][us1.y+1]);
		cell.draw(screen);
	}
	if (abs(des.x-us1.x) + abs(des.y-us1.y) <=1)
	{
		Cell cell(des.x, des.y, maze.grid[des.x][des.y]);
		cell.shape[3] = cell.shape[3] | 0b00100100;
		cell.shape[4] = cell.shape[4] | 0b00011000;
		cell.shape[5] = cell.shape[5] | 0b00011000;
		cell.shape[6] = cell.shape[6] | 0b00100100;
		cell.draw(screen);
	}
}

void User::move(SSD1306_t *screen)
{
	Cell cell(us1.x, us1.y, 3);
	cell.draw(screen);
	if (us1.x > 0)
	{
		Cell cell(us1.x-1, us1.y,3);
		cell.draw(screen);
	} 
	if (us1.x < 7)
	{
		Cell cell(us1.x+1, us1.y, 3);
		cell.draw(screen);
	}
	if (us1.y > 0)
	{
		Cell cell(us1.x, us1.y-1, 3);
		cell.draw(screen);
	} 
	if (us1.x < 15)
	{
		Cell cell(us1.x, us1.y+1, 3);
		cell.draw(screen);
	}
}

bool User::win()
{
	if (us1.x != des.x || us1.y != des.y) return 0;
	return 1;
}

bool User::lose()
{
	if (us2.x != des.x || us2.x != des.y) return 0;
	return 1;
}

User::~User() {}