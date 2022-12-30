#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "conio2.h"
#include <stdlib.h>
#include <cmath>

#define INFO_X 2
#define INFO_Y 5
#define BOARD_X 48
#define BOARD_Y 5
#define BLACK_STONE 1
#define WHITE_STONE -1
#define UP 0x48
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d
#define BACKGROUND BROWN
#define FONT BLACK

void draw_cursor(int x, int y, int stone, int max_x=118, int max_y=30) {
	if (x < max_x and y < max_y) {
		if (stone == BLACK_STONE) textcolor(BLACK);
		else textcolor(WHITE);
		gotoxy(x, y);
		putch('0');
	}
}

void input(char* string) {
	int button, pos = 0;
	do {
		button = getche();
		if (button != 0x0d) {
			string[pos] = button;
			pos++;
		}
	} while (button != 0x0d);
	string[pos] = '\0';
}

int int_input() {
	char string[20];
	input(string);
	return atoi(string);
}

int choose_size() {
	int y = INFO_Y+1, button;
	do {
		clrscr();
		textcolor(FONT);
		gotoxy(INFO_X + 2, INFO_Y);
		cputs("Please choose the size");
		gotoxy(INFO_X+2, INFO_Y+1);
		cputs("9X9");
		gotoxy(INFO_X + 2, INFO_Y+2);
		cputs("13X13");
		gotoxy(INFO_X + 2, INFO_Y+3);
		cputs("19X19");
		gotoxy(INFO_X + 2, INFO_Y + 4);
		cputs("Write myself");
		draw_cursor(INFO_X, y, BLACK_STONE);
		button = getch();
		if (button==0){                  // arrows
			button = getch();
			switch (button)
			{
			case UP: 
				if (y > INFO_Y+1)
					y--;
				break;
			
			case DOWN: 
				if (y < INFO_Y + 4)
					y++;
				break;
			}
		}
	} while (button != 0x0d);

	switch (y)
	{
	case INFO_Y+1:
		return 9;
		break;

	case INFO_Y+2:
		return 13;
		break;

	case INFO_Y+3:
		return 19;
		break;

	case INFO_Y + 4:
		int size = 0;
		do {
			clrscr();
			gotoxy(INFO_X, INFO_Y);
			cputs("Input size ");
			size = int_input();
		} while (size < 1 or size > 100);
		return size;
		break;
	}
	return -1;
}

bool is_stone(int stone) {
	if (stone == BLACK_STONE or stone == WHITE_STONE)
		return true;
	return false;
}

int get_size(int** board) {
	return (_msize(board[0]) / sizeof(board[0][0]));
}

int** make_board(int rozmiar) {
	int** board = (int**)malloc(rozmiar * sizeof(int*));
	for (int x = 0; x < rozmiar; x++) {
		board[x] = (int*)malloc(rozmiar * sizeof(int));
		for (int y = 0; y < rozmiar; y++) {
			board[x][y] = 0;
		}
	}
	return board;
}

int** start_new_game(int& step) {
	int** board;
	int board_size = choose_size();
	board = make_board(board_size);
	int handicap=0;
	do {
		clrscr();
		textcolor(FONT);
		gotoxy(INFO_X, INFO_Y);
		cputs("Please amound of handicap ");
		handicap = int_input();
	} while (handicap < 1 or handicap > 9);
	step = 1 - handicap;
	clrscr();
	textcolor(FONT);
	textbackground(BACKGROUND);
	return board;
	}

int get_liberties(int x, int y, int stone, int** board) {
	int libertiers = 4, rozmiar = get_size(board);
	if (y == 0 or board[x][y + 1] + stone == 0) libertiers--;
	if (y == rozmiar - 1 or board[x][y - 1] + stone == 0) libertiers--;
	if (x == 0 or board[x - 1][y] + stone == 0) libertiers--;
	if (x == rozmiar - 1 or board[x + 1][y] + stone == 0) libertiers--;
	return libertiers;
}

void draw_board(int rozmiar,int max_x,int max_y) {
	clrscr();
	textcolor(FONT);
	textbackground(BACKGROUND);
	gotoxy(BOARD_X, BOARD_Y);
	for (int y = 1; y <= rozmiar; y++) {
		for (int x = 1; x < rozmiar; x++) {
			if (BOARD_X +x*4<=max_x and BOARD_Y + y * 2 <= max_y+1)
				cputs(" ---");
		}
		gotoxy(BOARD_X, BOARD_Y + y*2);
	}
	for (int x = 0; x < 4 * rozmiar; x += 4) {
		for (int y = 1; y < 2 * (rozmiar - 1); y += 2) {
			if (BOARD_Y + y < max_y and BOARD_X + x < max_x) {
				gotoxy(BOARD_X + x, BOARD_Y + y);
				putch('|');
			}
		}
	}
	textcolor(FONT);
	gotoxy(BOARD_X - 1,BOARD_Y -1);
	for (int x = 0; x < rozmiar; x++) 
		if (wherex() <= max_x - 4) cputs("####");
	if (BOARD_Y + rozmiar * 2 - 1<max_y) {
		gotoxy(BOARD_X - 1, BOARD_Y + rozmiar * 2 - 1);
		for (int x = 0; x < rozmiar; x++)
			if (wherex() <= max_x - 4) cputs("####");
	}
	for (int y = -1; y < 2 * (rozmiar); y++) {
		if (BOARD_Y + y < max_y and BOARD_X - 2 < max_x) {
			gotoxy(BOARD_X - 2, BOARD_Y + y);
			putch('#');
		}
		if (BOARD_Y + y < max_y and BOARD_X + rozmiar * 4 - 2 < max_x) {
			gotoxy(BOARD_X + rozmiar * 4 - 2, BOARD_Y + y);
			putch('#');
		}
	}
}

void draw_stones(int** board,int max_x,int max_y) {
	int rozmiar = get_size(board);
	for (int x = 0; x < rozmiar; x++) {
		for (int y = 0; y < rozmiar; y++) {
			if (is_stone(board[x][y])) {
				if (board[x][y] == BLACK_STONE)
					textcolor(BLACK);
				else textcolor(WHITE);
				if (get_liberties(x, y, board[x][y], board) == 1) textbackground(RED);
				
				if (BOARD_X + 4 * x < max_x and BOARD_Y + 2 * y < max_y) {
					gotoxy(BOARD_X + 4 * x, BOARD_Y + 2 * y);
					putch('O');
				}
				textbackground(BACKGROUND);
			}
		}
	}
}

void draw_test_stone(int x, int y, int stone,int max_x, int max_y) {
	if (x != -1 and x < max_x and y < max_y) {
		if (stone == BLACK_STONE) textcolor(BLACK);
		else textcolor(WHITE);
		gotoxy(x, y);
		putch('0');
	}
}

void add_stone(int x, int y, int stone, int** board) {
	x = (x - BOARD_X) / 4;
	y = (y - BOARD_Y) / 2;
	board[x][y] = stone;
}

bool is_empty(int x, int y, int** board) {
	x = (x - BOARD_X) / 4;
	y = (y - BOARD_Y) / 2;
	if (board[x][y] != WHITE_STONE and board[x][y] != BLACK_STONE)
		return true;
	return false;
}

void draw_text(int x, int y, float wh_points, float bl_points) {
	textcolor(FONT);
	char info[][28] = {
		"Aktualna pozycja: x=%d y=%d",
		"White score: %.1lf",
		"Black score: %.1lf",
	};
	x = (x - BOARD_X) / 4 + 1;
	y = (y - BOARD_Y) / 2 + 1;
	sprintf(info[0], info[0], x, y);
	gotoxy(INFO_X, INFO_Y);
	cputs(info[0]);

	gotoxy(INFO_X, INFO_Y + 1);
	sprintf(info[1], info[1], wh_points);
	cputs(info[1]);

	gotoxy(INFO_X, INFO_Y + 2);
	sprintf(info[2], info[2], bl_points);
	cputs(info[2]);

	char coms[9][44] = {
		"Available commands",
		"arrows: moving the cursor over the board",
		"q: quit the program",
		"n: start a new game",
		"enter: confirm choice and end player turn",
		"esc: cancel current action",
		"i: place a stone on the board",
		"s: save the game state",
		"l: load the game state",
	};
	for (int i = 0; i < 9; i++) {
		gotoxy(INFO_X, INFO_Y + 4 + i);
		cputs(coms[i]);
	}
}

bool is_ko(int& x, int& y, int stone, int** board) {
	int rozmiar = get_size(board);
	int curr_point = board[x][y];
	int curr_x = x, curr_y = y;
	bool ko = false, down = false, up = false, left = false, right = false;
	if (y == 0) up = true;
	if (x == 0) left = true;
	if (y == rozmiar - 1) down = true;
	if (x == rozmiar - 1) right = true;
	board[x][y] = stone;
	if (!left and get_liberties(x - 1, y,-stone,board)==0) {
		ko = true;
		x--;
	}
	else if (!up and get_liberties(x, y + 1, -stone, board) == 0) {
		ko = true;
		y++;
	}
	else if (!right and get_liberties(x + 1, y, -stone, board)==0) {
		ko = true;
		x++;
	}
	else if (!down and get_liberties(x, y - 1, -stone, board)==0) {
		ko = true;
		y--;
	}
	board[curr_x][curr_y] = curr_point;
	return ko;
}

void write_game(int** board, int step, bool was_ko)
{
	char file_name[20];
	clrscr();
	textcolor(FONT);
	textbackground(BACKGROUND);
	gotoxy(INFO_X, INFO_Y);
	cputs("Input file name: ");
	input(file_name);
	int rozmiar = get_size(board);
	FILE* file;
	file = fopen(file_name, "w");
	fprintf(file, "%d %d %d \n", step, was_ko, rozmiar); 
	for (int y = 0; y < rozmiar; y++) {
		for (int x = 0; x < rozmiar; x++) {
			fprintf(file, "%d ", board[x][y]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

int** read_game(int* step, bool &was_ko)
{
	char file_name[20];
	int rozmiar,ko;
	int** board;
	clrscr();
	textcolor(FONT);
	textbackground(BACKGROUND);
	gotoxy(INFO_X, INFO_Y);
	cputs("Input file name: ");
	input(file_name);
	FILE* file;
	file = fopen(file_name, "r");
	if (file) {
		fscanf(file, "%d %s %d \n", step, &ko, &rozmiar);
		board = make_board(rozmiar);
		for (int y = 0; y < rozmiar; y++) {
			for (int x = 0; x < rozmiar; x++) {
				fscanf(file, "%d ", &board[x][y]);
			}
		}
		was_ko = (bool)ko;
		fclose(file);
		return board;
	}
	return 0;
}

void check_board(int** board) {
	int rozmiar = get_size(board);
	for (int x = 0; x < rozmiar; x++) {
		for (int y = 0; y < rozmiar; y++) {
			if (is_stone(board[x][y]) and get_liberties(x, y, board[x][y], board) == 0) board[x][y] = 0;
		}
	}
}

void get_window_size(int& x, int& y) {
	x = 1;
	y = 1;
	gotoxy(x, 1);
	while (x == wherex()) {
		x++;
		gotoxy(x, 1);
	}
	gotoxy(1, y);
	while (y == wherey()) {
		y++;
		gotoxy(1, y);
	}
}

int main() {
#ifndef __cplusplus
	Conio2_Init();
#endif
	settitle("Pavel, Harelik, 196766");
	_setcursortype(_NOCURSOR);
	textbackground(BACKGROUND);
	int** board, **load_board=0;
	int step, button, current_x = BOARD_X, current_y = BOARD_Y,current_stone = BLACK_STONE;
	int test_x = -1, test_y = -1, board_x, board_y, max_x, max_y;
	bool was_ko = false;
	float white_points = 0.5, black_points = 0;
	board = start_new_game(step); 
	if (step == 0) white_points += 6;

	do {
		get_window_size(max_x, max_y);
		if (step % 2 == 1)current_stone = WHITE_STONE;
		else current_stone = BLACK_STONE;
		board_x = (current_x - BOARD_X) / 4;
		board_y = (current_y - BOARD_Y) / 2;
		draw_board(get_size(board),max_x,max_y);
		draw_stones(board,max_x,max_y);
		draw_test_stone(test_x, test_y, current_stone,max_x,max_y);
		draw_cursor(current_x, current_y, current_stone,max_x,max_y);
		draw_text(current_x, current_y, white_points, black_points);
		button = getch();
		switch (button)
		{
		case 'n':
			board = start_new_game(step);
			if (step == 0) white_points += 6;
			was_ko = false;
			test_x = -1, current_x = BOARD_X, current_y = BOARD_Y;
			white_points = 0.5, black_points = 0;
			if (step == 0) white_points += 6;
			break;

		case 0x1b:                   // esc
			test_x = -1;
			break;

		case 'l':
			load_board = read_game(&step, was_ko);
			if (load_board)
				board = load_board;
			load_board = 0;
			break;

		case 's':
			write_game(board, step, was_ko);
			break;

		case 'i':
			if (is_empty(current_x, current_y, board)) {
				if (get_liberties(board_x, board_y, current_stone, board) != 0 or !was_ko and is_ko(board_x, board_y, current_stone, board)) {
					test_x = current_x;
					test_y = current_y;
				}
			} 
			break;

		case 0x0d:                    // enter
			if (test_x != -1) {
				board_x = (test_x - BOARD_X) / 4;
				board_y = (test_y - BOARD_Y) / 2;
				if (get_liberties(board_x, board_y, current_stone, board) == 0) {
					is_ko(board_x, board_y, current_stone, board);
					if (board[board_x][board_y] == BLACK_STONE) white_points++;
					else black_points++;
					board[board_x][board_y] = 0;
					was_ko = 1;
				}
				else was_ko = 0;
				add_stone(test_x, test_y, current_stone, board);
				check_board(board);
				test_x = -1;
				step++;
			}
			break;

		case 0: {                   // arrows
			button = getch();
			switch(button)
			{
			case UP: {
				if (current_y > BOARD_Y)
					current_y -= 2;
				break;
			}
			case DOWN: {
				if (current_y < BOARD_Y + (get_size(board)-1) * 2)
					current_y += 2;
				break;
			}
			case LEFT: {
				if (current_x > BOARD_X)
					current_x -= 4;
				break;
			}
			case RIGHT: {
				if (current_x < BOARD_X + (get_size(board) - 1) * 4)
					current_x += 4;
				break;
			}
			}
		}
		}
	} while (button != 'q');

	_setcursortype(_NORMALCURSOR);
	return 0;
}