/*
 * User.cpp
 *
 *  Created on: Dec 9, 2024
 *      Author: ro
 */

#include "User.h"
#include "esp_mesh.h"
#include "ssd1306.h"
#include <stdint.h>
#include <string.h>
#include "Cell.h"


User::User(Maze &maze) 
{
	for (uint8_t x = 0; x<8; x++)
	{
		for (uint8_t y = 0; y<16; y++)
		{
			bfs(maze, Node(x, y));	
		}
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
	cell.row = us2.x;
	cell.col = us2.y;
	cell.draw(screen);
}

User::~User() {}