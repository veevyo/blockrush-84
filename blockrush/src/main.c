#include <ti/getcsc.h>
#include <sys/util.h>
#include <graphx.h>
#include <keypadc.h>
#include <sys/timers.h>
#include <compression.h>
#include <ti/screen.h>
#include "fonts/fonts.h"

#define BACKGROUND 41
#define ENEMY 124
#define PLAYER 188
#define HIT 242
#define START_PLAYER_X 150
#define START_PLAYER_Y 210
#define START_ENEMY_X 50
#define START_ENEMY_Y 0
#define TIME (*(volatile uint32_t*)0xF30044)

void moveDown(int *x, int *y, int *tempX, int *tempY, int *score);
void input(int *x, int *y, int *tempX, int *tempY);
int inRange(int min, int max, int value);
int collisionCheck(int x1, int y1, int x2, int y2);
char * itoa(int num, char * str, int base);

int main(void)
{
	srandom(TIME);

	int score = 0;
	int lives = 3;

	int e_x = START_ENEMY_X;
	int e_y = START_ENEMY_Y;
	int enemyTempY = START_ENEMY_Y;
	int enemyTempX = START_ENEMY_X;
	
	int p_x = START_PLAYER_X;
	int p_y = START_PLAYER_Y;
	int playerTempY = START_PLAYER_Y;
	int playerTempX = START_PLAYER_X;

	gfx_Begin();
	gfx_FillScreen(BACKGROUND);
	gfx_SetColor(ENEMY);
	gfx_FillRectangle(e_x, e_y, 30, 30);
	gfx_SetColor(PLAYER);
	gfx_FillRectangle(p_x, p_y, 20, 20);


	char buffer[20];

	fontlib_SetFont(test_font, 0);
        fontlib_SetWindow(300, 0, 10, 10);
        fontlib_SetCursorPosition(10, 10);
        fontlib_SetColors(ENEMY, BACKGROUND);
        fontlib_DrawString("Lives: ");
        fontlib_DrawString(itoa(lives, buffer, 10));

        fontlib_SetFont(test_font, 0);
        fontlib_SetCursorPosition(250, 10);
        fontlib_SetColors(ENEMY, BACKGROUND);
        fontlib_DrawString("Score: ");
        fontlib_DrawString(itoa(score, buffer, 10));
	
	int collided = 0;
	do {
		if (collided == 1) {
			gfx_FillScreen(BACKGROUND);
			collided = 0;
		}
		fontlib_ClearWindow();
		fontlib_SetCursorPosition(10, 10);
		fontlib_DrawString("Lives: ");
		fontlib_DrawString(itoa(lives, buffer, 10));

		fontlib_ClearWindow();
		fontlib_SetCursorPosition(250, 10);
		fontlib_DrawString("Score: ");
		fontlib_DrawString(itoa(score, buffer, 10));
		
		input(&p_x, &p_y, &playerTempX, &playerTempY);
                gfx_SetColor(BACKGROUND);
                gfx_FillRectangle(playerTempX, playerTempY, 20, 20);
                gfx_SetColor(PLAYER);
                gfx_FillRectangle(p_x, p_y, 20, 20);
		
		moveDown(&e_x, &e_y, &enemyTempX, &enemyTempY, &score);
		gfx_SetColor(BACKGROUND);
                gfx_FillRectangle(enemyTempX, enemyTempY, 30, 30);
		gfx_SetColor(ENEMY);
		gfx_FillRectangle(e_x, e_y, 30, 30);

		if (collisionCheck(e_x, e_y, p_x, p_y)) {
			lives--;
			gfx_SetColor(HIT);
			gfx_FillRectangle(0, 0, 320, 240);
			gfx_SetColor(BACKGROUND);
			gfx_FillRectangle(e_x, e_y, 30, 30);
			e_x = randInt(0, 300);
			e_y = 0;
			collided = 1;
		}


	} while(lives);
}

void reverseString(char str[], int length) {
        int start = 0;
        int end = length - 1;
        while (start < end) {
                char temp = str[start];
                str[start] = str[end];
                str[end] = temp;
                end--;
                start++;
        }
}

char* itoa(int num, char* str, int base) {
        int i = 0;
        int isNegative = 0;
        if (num == 0) {
                str[i++] = '0';
                str[i] = '\0';
                return str;
        }
        if (num < 0 && base == 10) {
                isNegative = 1;
                num = -num;
        }
        while (num != 0) {
                int rem = num % base;
                str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
                num = num / base;
        }
        if (isNegative) {
                str[i++] = '-';
        }
        str[i] = '\0';
        reverseString(str, i); 

        return str;
}

void moveDown(int *x, int *y, int *tempX, int *tempY, int *score) {
	static int oldY = START_ENEMY_Y;
	static int oldX = START_ENEMY_X;
	oldY = *y;
	oldX = *x;
	if (*y < 230) {
		int newY = *y;
		newY += 2;
		*y = newY;
	}
	else {
		int newY = 0;
		int newX = *x;
		int newScore = *score;
		newX = randInt(0, 300);
		newScore += 1;
		*y = newY;
		*x = newX;
		*score = newScore;
	}
	*tempY = oldY;
	*tempX = oldX;
}

void input(int *x, int *y, int *tempX, int *tempY) {
	static int oldY = START_PLAYER_Y;
	static int oldX = START_PLAYER_X;
	oldY = *y;
	oldX = *x;

	kb_key_t arrows;
	kb_Scan();
	arrows = kb_Data[7];

	if (arrows) {
		if (arrows & kb_Right) {
			if (*x < 290) {
				*x += 1;
			}
		}

		if (arrows & kb_Left) {
			if (*x > 10) {
				*x -= 1;
			}
		}
	}
	*tempY = oldY;
	*tempX = oldX;
}

int inRange(int min, int max, int value) {
        if (min <= value && value <= max) {
                return 1;
        }
        return 0;
}

int collisionCheck(int x1, int y1, int x2, int y2) {
        delay(1);
        if (inRange(0, 30, abs(x1 - x2)) && inRange(0, 30, abs(y1 - y2))) {
                return 1;
        }
        return 0;
}
