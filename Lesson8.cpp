#pragma once
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <vector>
#include <random>

#include "PoolAllocater.h"
#include <string>

using namespace std;
using vv_int = vector<vector<int>>;

const int YSIZE = 15;
const int XSIZE = 21;

const int Path = 0;
const int Wall = 1;
const int Player = 2;
const int Goal = 3;
const int Fuel = 4;
const int LargeFuel = 5;

random_device rd;
mt19937 gen(rd());
int random(int low, int high)
{
	uniform_int_distribution<> dist(low, high);
	return dist(gen);
}

struct Point
{
	int x;
	int y;
};

vv_int GenerateField(int stageNum)
{
	vv_int _field(YSIZE, vector<int>(XSIZE, Path));

	//外周に壁
	for (int y = 0; y < YSIZE; y++)
	{
		for (int x = 0; x < XSIZE; x++)
		{
			if (y == 0 || x == 0 || y == YSIZE - 1 || x == XSIZE - 1)
			{
				_field[y][x] = Wall;
			}
			else
			{
				_field[y][x] = Path;
			}
		}
	}

	//スタート・ゴールの設置
	int maxOptionNumber = ((XSIZE - 1) / 2) * ((YSIZE - 1) / 2);
	int startNumber = random(1, maxOptionNumber);
	int goalNumber = random(1, maxOptionNumber - 1);
	if (startNumber == goalNumber) goalNumber++;

	int optionNumber = 1;
	for (int y = 1; y < YSIZE - 1; y += 2)
	{
		for (int x = 1; x < XSIZE - 1; x += 2)
		{
			if (optionNumber == startNumber) _field[y][x] = Player;
			if (optionNumber == goalNumber) _field[y][x] = Goal;
			optionNumber++;
		}
	}

	//棒を倒す
	for (int y = 2; y < YSIZE - 1; y += 2)
	{
		for (int x = 2; x < XSIZE - 1; x += 2)
		{
			_field[y][x] = Wall;

			while (true)
			{
				int direction;
				if (x == 2) direction = random(1, 4);
				else direction = random(1, 3);

				int wallY = y;
				int wallX = x;
				switch (direction)
				{
				case 0:
					wallX++;
					break;
				case 1:
					wallY++;
					break;
				case 2:
					wallX--;
					break;
				case 3:
					wallY--;
					break;
				}

				if (_field[wallY][wallX] != Wall)
				{
					_field[wallY][wallX] = Wall;
					break;
				}

			}
		}
	}

	//燃料の設置
	for (int y = 0; y < YSIZE; y++)
	{
		for (int x = 0; x < XSIZE; x++)
		{
			if (_field[y][x] == Path)
			{
				int r = random(0, 1000);
				if (r <= 60 - stageNum)
				{
					_field[y][x] = Fuel;
				}
				else if (990 + stageNum <= r)
				{
					_field[y][x] = LargeFuel;
				}
			}
		}
	}

	return _field;
}

void Show(vv_int _field, int _stageNum, int _energy)
{
	for (int y = 0; y < YSIZE; y++)
	{
		for (int x = 0; x < XSIZE; x++)
		{
			if (_field[y][x] == Path)
			{
				printf("\x1b[%d;%dH　", y + 1, x + 1);
			}
			else if (_field[y][x] == Wall)
			{
				printf("\x1b[%d;%dH■", y + 1, x + 1);
			}
			else if (_field[y][x] == Player)
			{
				printf("\x1b[%d;%dH\x1b[31m@\x1b[0m", y + 1, x + 1);
			}
			else if (_field[y][x] == Goal)
			{
				printf("\x1b[%d;%dH\x1b[31mG\x1b[0m", y + 1, x + 1);
			}
			else if (_field[y][x] == Fuel)
			{
				printf("\x1b[%d;%dH\x1b[33mf\x1b[0m", y + 1, x + 1);
			}
			else if (_field[y][x] == LargeFuel)
			{
				printf("\x1b[%d;%dH\x1b[33mF\x1b[0m", y + 1, x + 1);
			}
		}
	}
	
	string disc = "第" + to_string(_stageNum) + "階層";
	printf("\x1b[%d;%dH%s", YSIZE + 1, 1, disc.c_str());

	disc = "残りエネルギー " + to_string(_energy);
	printf("\x1b[%d;%dH%s", YSIZE + 2, 1, disc.c_str());

	disc = "============================";
	printf("\x1b[%d;%dH%s", YSIZE + 3, 1, disc.c_str());

	disc = "迷路をクリアして最高階層を目指しましょう";
	printf("\x1b[%d;%dH%s", YSIZE + 4, 1, disc.c_str());

	disc = "歩くとエネルギーを消費します。エネルギーが切れるとゲームオーバーになります";
	printf("\x1b[%d;%dH%s", YSIZE + 5, 1, disc.c_str());

	disc = "エネルギー切れにならないように燃料を集めながらゴールを目指すといいでしょう";
	printf("\x1b[%d;%dH%s", YSIZE + 6, 1, disc.c_str());

	disc = "============================";
	printf("\x1b[%d;%dH%s", YSIZE + 7, 1, disc.c_str());

	disc = "操作方法:WASDキー or 2486キー";
	printf("\x1b[%d;%dH%s", YSIZE + 8, 1, disc.c_str());

	disc = "G:ゴール  @:プレイヤー ";
	printf("\x1b[%d;%dH%s", YSIZE + 9, 1, disc.c_str());

	disc = "f:燃料(+6エネルギー)  F:燃料(+20エネルギー)";
	printf("\x1b[%d;%dH%s", YSIZE + 10, 1, disc.c_str());
}

Point GetPlayerPos(vv_int _field)
{
	Point newPoint = { -1, -1 };

	for (int y = 0; y < YSIZE; y++)
	{
		for (int x = 0; x < XSIZE; x++)
		{
			if (_field[y][x] == Player)
			{
				newPoint.x = x;
				newPoint.y = y;
				return newPoint;
			}
		}
	}

	return newPoint;
}

vv_int CheckAndSetPlayerPos(vv_int _field, Point playerPos, int* stageNumber, int* energy)
{
	int y = playerPos.y;
	int x = playerPos.x;
	if (_field[y][x] == Path)
	{
		_field[y][x] = Player;

		if (_field[y + 1][x] == Player) _field[y + 1][x] = Path;
		else if (_field[y - 1][x] == Player) _field[y - 1][x] = Path;
		else if (_field[y][x + 1] == Player) _field[y][x + 1] = Path;
		else if (_field[y][x - 1] == Player) _field[y][x - 1] = Path;

		(*energy)--;

		return _field;
	}
	if (_field[y][x] == Fuel)
	{
		_field[y][x] = Player;

		(*energy) += 6;

		if (_field[y + 1][x] == Player) _field[y + 1][x] = Path;
		else if (_field[y - 1][x] == Player) _field[y - 1][x] = Path;
		else if (_field[y][x + 1] == Player) _field[y][x + 1] = Path;
		else if (_field[y][x - 1] == Player) _field[y][x - 1] = Path;

		(*energy)--;

		return _field;
	}
	if (_field[y][x] == LargeFuel)
	{
		_field[y][x] = Player;

		(*energy) += 30;

		if (_field[y + 1][x] == Player) _field[y + 1][x] = Path;
		else if (_field[y - 1][x] == Player) _field[y - 1][x] = Path;
		else if (_field[y][x + 1] == Player) _field[y][x + 1] = Path;
		else if (_field[y][x - 1] == Player) _field[y][x - 1] = Path;

		(*energy)--;

		return _field;
	}
	if (_field[y][x] == Goal)
	{
		(*stageNumber)++;

		return GenerateField(*stageNumber);
	}
	else
	{
		return _field;
	}
}


//vv_int field(YSIZE, vector<int>(XSIZE, 0));

int main()
{
	// エスケープシーケンスを有効に
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hStdOut, &mode);
	SetConsoleMode(hStdOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	// カーソルを消す
	printf("\x1b[?25l");

	int i;

	PoolAllocator<int, 2> a;
	int* stageNumber = a.Alloc();
	*stageNumber = 1;
	int* energy = a.Alloc();
	*energy = 50;

	Point playerPos = {0, 0};

	vv_int field = GenerateField(*stageNumber);

	Show(field, *stageNumber, *energy);

	do
	{

		playerPos = GetPlayerPos(field);
		i = _getch(); // キー入力待ち
		// キー入力で座標更新
		switch (i) {
		case '2':
		case 's':
			playerPos.y++;
			break;
		case '4':
		case 'a':
			playerPos.x--;
			break;
		case '6':
		case 'd':
			playerPos.x++;
			break;
		case '8':
		case 'w':
			playerPos.y--;
			break;
		}
		field = CheckAndSetPlayerPos(field, playerPos, stageNumber, energy);
		system("cls");
		Show(field, *stageNumber, *energy);

		if (*energy <= 0)
		{
			return 0;
		}
	} while ('q' != i); // 'q'キーで終了
}
