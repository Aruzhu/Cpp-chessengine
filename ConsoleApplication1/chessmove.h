#pragma once
struct chessmove {
	int x, y, fromY, fromX;
	char piece;
	float ev;
	bool operator < (const chessmove & a);
	bool operator == (const chessmove& a);
};