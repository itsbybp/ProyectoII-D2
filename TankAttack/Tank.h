#pragma once
#include "raylib.h"

struct Tank
{
	int row;
	int col;
	int user;
	int health;
	Color color;

	Tank() : row(0), col(0), user(0), health(100), color(BLACK) {}
	Tank(int r, int c, int u, int h, Color col) : row(r), col(c), user(u), health(h), color(col) {}
};

