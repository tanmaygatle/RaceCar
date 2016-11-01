/*****************************************************************************
 * Copyright (C) Tanmay Gatle. tanmaygatle@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string.h>
#define WIDTH 640
#define HEIGHT 600
#define MAX 100
#define EASY 20
#define MEDIUM 30
#define HARD 40

typedef struct mouse_click {
	int x, y;
} mouse_click;

typedef struct total_score {
	char name[20];
	int score;
} total_score;

mouse_click click();
void display_text(char*, int, int, int, int, int, int, int, int);
void display_text_loop(TTF_Font*, char*, int, int, int, int, int, int, int);
void inttostr(int, char*);
SDL_Rect set_dest(SDL_Surface*, int, int);
long int rand2();
void delay(unsigned int);
int checkCollision(SDL_Rect, SDL_Rect);
int check_score(char*, int);

void init();
void init_dest();
void display_startcar();
void menu();
void settings_menu();
void info_menu();
void move_player();
void jump_car();
void display_bg();
void display_cars();
void display_gameover();
void display_highscore();
void input_name();
void leaderboard();
void all_quit();

SDL_Surface *screen, *text, *player, *car[4], *jump_img[11], *bgimage, *bgm[3], *play, *quit, *replay, *quitover, *returnmain, *difficulty[3], *settings, *info, *back, *startcar;
SDL_Event event;
TTF_Font *font, *font_extra;
FILE *fp;
int startcar_counter = 0;
int jump = 0, jump_i = 0, jump_count = 0, jump_lim = 8, jump_timer = 0;
unsigned int time1, hscore, start, frameLimit;
int keyheld[323] = {0}, lim1 = 3, lim2 = 7, choose_diff = EASY, bgv = 4;
static int flag1 = 0, bgflag = 0;
long int v[4] = {0};
char time_str[MAX] = " ", hscore_str[MAX] = " ";
char name_str[17] = " ";
total_score max[5];
SDL_Rect destination, bgdest, bgdest_temp, bgm_dest[3], destination_c[4], play_dest, quit_dest, replay_dest, return_dest, quitover_dest, difficulty_dest[3], settings_dest, info_dest, startcar_dest;

/*Initialise SDL and Load Images*/
void init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		
		exit(1);
	}

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_HWSURFACE|SDL_DOUBLEBUF);
	
	player = SDL_LoadBMP("icon.bmp");
	car[0] = SDL_LoadBMP("car1.bmp");
	car[1] = SDL_LoadBMP("car2.bmp");
	car[2] = SDL_LoadBMP("car3.bmp");
	car[3] = SDL_LoadBMP("car4.bmp");
	
	jump_img[0] = jump_img[10] = SDL_LoadBMP("icon1.bmp");
	jump_img[1] = jump_img[9] = SDL_LoadBMP("icon2.bmp");
	jump_img[2] = jump_img[8] = SDL_LoadBMP("icon3.bmp");
	jump_img[3] = jump_img[7] = SDL_LoadBMP("icon4.bmp");
	jump_img[4] = jump_img[6] = SDL_LoadBMP("icon5.bmp");
	jump_img[5] = SDL_LoadBMP("icon6.bmp");

	if(!bgflag)
		bgimage = SDL_LoadBMP("bg1.bmp");

	play = SDL_LoadBMP("play.bmp");
	quit = SDL_LoadBMP("quit.bmp");

	quitover = SDL_LoadBMP("quitover.bmp");
	replay = SDL_LoadBMP("replay.bmp");
	returnmain = SDL_LoadBMP("returnmain.bmp");

	difficulty[0] = SDL_LoadBMP("easy.bmp");
	difficulty[1] = SDL_LoadBMP("medium.bmp");
	difficulty[2] = SDL_LoadBMP("hard.bmp");

	settings = SDL_LoadBMP("settings.bmp");
	info = SDL_LoadBMP("info.bmp");
	back = SDL_LoadBMP("back.bmp");

	bgm[0] = SDL_LoadBMP("bgm1.bmp");
	bgm[1] = SDL_LoadBMP("bgm2.bmp");
	bgm[2] = SDL_LoadBMP("bgm3.bmp");

	startcar = SDL_LoadBMP("start_car.bmp");

	font = NULL;
	font_extra = NULL;

	fp = fopen("scores_easy.txt", "a+");	

	if(TTF_Init() == -1) 
	{
		printf("Couldn't initialise TTF: %s\n", SDL_GetError());

		exit(1);	
	}
	if (screen == NULL)
	{
		printf("Couldn't set screen mode to 640 x 600: %s\n", SDL_GetError());

		exit(1);
	}

	SDL_SetColorKey(player, SDL_SRCCOLORKEY, SDL_MapRGB(player->format, 0, 0, 0));
	
	int i;
	for(i = 0; i < 4; i++)
		SDL_SetColorKey(car[i], SDL_SRCCOLORKEY, SDL_MapRGB(car[i]->format, 255, 255, 255));
	
	for(i = 0; i < 3; i++)
		SDL_SetColorKey(difficulty[i], SDL_SRCCOLORKEY, SDL_MapRGB(difficulty[i]->format, 255, 255, 255));

	for(i = 0; i < 11; i++)
		SDL_SetColorKey(jump_img[i], SDL_SRCCOLORKEY, SDL_MapRGB(jump_img[i]->format, 0, 0, 0));

	SDL_SetColorKey(settings, SDL_SRCCOLORKEY, SDL_MapRGB(settings->format, 255, 255, 255));
	SDL_SetColorKey(info, SDL_SRCCOLORKEY, SDL_MapRGB(info->format, 255, 255, 255));
	SDL_SetColorKey(back, SDL_SRCCOLORKEY, SDL_MapRGB(back->format, 255, 255, 255));	
		
	SDL_SetColorKey(quitover, SDL_SRCCOLORKEY, SDL_MapRGB(quitover->format, 255, 255, 255));
	SDL_SetColorKey(replay, SDL_SRCCOLORKEY, SDL_MapRGB(replay->format, 255, 255, 255));
	SDL_SetColorKey(returnmain, SDL_SRCCOLORKEY, SDL_MapRGB(returnmain->format, 255, 255, 255));

	SDL_SetColorKey(play, SDL_SRCCOLORKEY, SDL_MapRGB(play->format, 0, 0, 0));
	SDL_SetColorKey(quit, SDL_SRCCOLORKEY, SDL_MapRGB(quit->format, 0, 0, 0));
	
	SDL_SetColorKey(startcar, SDL_SRCCOLORKEY, SDL_MapRGB(startcar->format, 0, 0, 0));
	
	SDL_WM_SetCaption("RaceCar", 0);
}

/*Set Image Loactions or SDL_Rects*/
void init_dest() {
	destination = set_dest(player, 320, 480);
	bgdest = set_dest(bgimage, 0, 0);
	
	int i;
	for(i = 0; i < 4; i++)
		destination_c[i] = set_dest(car[i], 0, 0);
	
	settings_dest = set_dest(settings, 20, 20);
	info_dest = set_dest(info, WIDTH - 120, 20);

	difficulty_dest[0] = set_dest(difficulty[0], 50, 120);
	difficulty_dest[1] = set_dest(difficulty[1], 100 + difficulty[0]->w, 120);
	difficulty_dest[2] = set_dest(difficulty[2], 150 + difficulty[0]->w + difficulty[1]->w, 120);

	bgm_dest[0] = set_dest(bgm[0], 50, 340);
	bgm_dest[1] = set_dest(bgm[1], 100 + bgm[0]->w, 340);
	bgm_dest[2] = set_dest(bgm[2], 150 + bgm[0]->w + bgm[1]->w, 340);

	play_dest = set_dest(play, 220, 280);
	quit_dest = set_dest(quit, 220, play->h + 300);

	replay_dest = set_dest(replay, 170, 450);
	return_dest = set_dest(returnmain, 270 + replay->w, 450); 
	quitover_dest = set_dest(quitover, 170 + replay->w, replay->h + 475);

	startcar_dest = set_dest(startcar, -(startcar->w), 360);
}

/*Blits the horizontally moving car on the start up screen*/
void display_startcar() {
	if(startcar_counter >= 1000) {
		SDL_BlitSurface(startcar, NULL, screen, &startcar_dest);
		startcar_dest.x += 1;
		if(startcar_dest.x == WIDTH) {
			startcar_dest.x = -(startcar->w);
			startcar_counter = 0;
		}
	}
	startcar_counter++;
}

/*Shows the start menu and takes corresponding mouse input*/
void menu() {
	mouse_click m1;
	font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 90);
	font_extra = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 12);
	SDL_Rect info_rect = {WIDTH - 100, 50, 50, 50};

	while(!flag1)
	{	
		m1 = click();		
		if((m1.x > play_dest.x) && (m1.x < play_dest.x + play_dest.w) && (m1.y > play_dest.y) && (m1.y < play_dest.y + play_dest.h)) 		
			return;
		if((m1.x > quit_dest.x) && (m1.x < quit_dest.x + quit_dest.w) && (m1.y > quit_dest.y) && (m1.y < quit_dest.y + quit_dest.h)) {
			all_quit();
			exit(0);
		}
		if((m1.x > settings_dest.x) && (m1.x < settings_dest.x + settings_dest.w) && (m1.y > settings_dest.y) && (m1.y < settings_dest.y + settings_dest.h)) 
			settings_menu();
		if((m1.x > info_dest.x) && (m1.x < info_dest.x + info_dest.w) && (m1.y > info_dest.y) && (m1.y < info_dest.y + info_dest.h)) 
			info_menu();

		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_FillRect(screen, &info_rect, SDL_MapRGB(screen->format, 255, 255, 255));
		display_startcar();
		display_text_loop(font, "RaceCAR", 120, 140 , 0, 0, 255, 0, 0);
		SDL_BlitSurface(settings, NULL, screen, &settings_dest);
		SDL_BlitSurface(info, NULL, screen, &info_dest);
		SDL_BlitSurface(play, NULL, screen, &play_dest);
		SDL_BlitSurface(quit, NULL, screen, &quit_dest);
		display_text_loop(font_extra, "Tanmay Gatle", 20, 570, 0, 0, 255, 0, 0);
		
		SDL_Flip(screen);		
	}
}

/*Shows the settings menu and takes corresponding mouse input*/
void settings_menu() {
	int i;
	mouse_click m1;	
	font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 30);
	font_extra = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 12);
	
	while(1) 
	{	
		m1 = click();		
		if((m1.x > difficulty_dest[0].x) && (m1.x < difficulty_dest[0].x + difficulty_dest[0].w) && (m1.y > difficulty_dest[0].y) && (m1.y < difficulty_dest[0].y + difficulty_dest[0].h)) {
			lim1 = 4;
			lim2 = 8;
			bgv = 4;
			jump_lim = 8;
			choose_diff = EASY;
			font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 90);
			return;
		}
		if((m1.x > difficulty_dest[1].x) && (m1.x < difficulty_dest[1].x + difficulty_dest[1].w) && (m1.y > difficulty_dest[1].y) && (m1.y < difficulty_dest[1].y + difficulty_dest[1].h)) {
			lim1 = 5;
			lim2 = 9;
			bgv = 5;
			jump_lim = 6;	
			choose_diff = MEDIUM;
			font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 90);
			return;
		}
		if((m1.x > difficulty_dest[2].x) && (m1.x < difficulty_dest[2].x + difficulty_dest[2].w) && (m1.y > difficulty_dest[2].y) && (m1.y < difficulty_dest[2].y + difficulty_dest[2].h)) {
			lim1 = 7;
			lim2 = 10;
			bgv = 6;
			jump_lim = 5;
			choose_diff = HARD;
			font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 90);
			return;		
		}
		if((m1.x > bgm_dest[0].x) && (m1.x < bgm_dest[0].x + bgm_dest[0].w) && (m1.y > bgm_dest[0].y) && (m1.y < bgm_dest[0].y + bgm_dest[0].h)) {
			bgimage = SDL_LoadBMP("bg1.bmp");
			bgflag = 1;
			font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 90);
			return;
		}
		if((m1.x > bgm_dest[1].x) && (m1.x < bgm_dest[1].x + bgm_dest[1].w) && (m1.y > bgm_dest[1].y) && (m1.y < bgm_dest[1].y + bgm_dest[1].h)) {
			bgimage = SDL_LoadBMP("bg2.bmp");
			bgflag = 1;
			font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 90);
			return;
		}
		if((m1.x > bgm_dest[2].x) && (m1.x < bgm_dest[2].x + bgm_dest[2].w) && (m1.y > bgm_dest[2].y) && (m1.y < bgm_dest[2].y + bgm_dest[2].h)) {
			bgimage = SDL_LoadBMP("bg3.bmp");
			bgflag = 1;
			font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 90);
			return;
		}
		if((m1.x > info_dest.x) && (m1.x < info_dest.x + info_dest.w) && (m1.y > info_dest.y) && (m1.y < info_dest.y + info_dest.h)) {
			font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 90);
			return;
		}

		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

		display_text_loop(font, "SELECT DIFFICULTY: ", 20, 30 ,0, 0 ,255, 0, 0);
		for(i = 0; i < 3; i++)
			SDL_BlitSurface(difficulty[i], NULL, screen, &difficulty_dest[i]);

		display_text_loop(font, "SELECT BACKGROUND: ", 20, 250 ,0, 0 ,255, 0, 0);
		for(i = 0; i < 3; i++)
			SDL_BlitSurface(bgm[i], NULL, screen, &bgm_dest[i]);
		
		SDL_BlitSurface(back, NULL, screen, &info_dest);

		display_text_loop(font_extra, "Tanmay Gatle", 20, 570, 0, 0, 255, 0, 0);

		SDL_Flip(screen);		
	}
}

/*Shows the info menu and takes corresponding muse input*/
void info_menu() {
	mouse_click m1;
	font_extra = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 12);
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	
	display_text("HELP:", 55, 40, 100 , 0, 0, 255, 0, 0);
	display_text("1. USE LEFT AND RIGHT ARROW KEYS TO DODGE ONCOMING CARS", 16, 40, 200 ,0, 0 ,255, 0, 0);
	display_text("2. PRESS SPACE TO JUMP OVER ONCOMING CARS", 16, 40, 250 , 0, 0, 255, 0, 0);
	display_text("3. YOU CAN JUMP ONLY WHEN 'JUMP' IS CHARGED ie ", 16, 40, 300 , 0, 0, 255, 0, 0);
	display_text("JUMP", 16, 475, 300 , 0, 0, 0, 255, 0);
	display_text("4. SURVIVE FOR AS LONG AS POSSIBLE", 16, 40, 350, 0, 0, 255, 0, 0);
	display_text("5. TO ADJUST DIFFICULTY OR CHANGE THE BACKGROUND", 16, 40, 400 ,0, 0 ,255, 0, 0);
	display_text("   CLICK ON THE SETTTINGS BUTTON IN THE MAIN MENU", 16, 40, 420,0, 0 ,255, 0, 0);
	
	while(1) {
		m1 = click();
		if((m1.x > info_dest.x) && (m1.x < info_dest.x + info_dest.w) && (m1.y > info_dest.y) && (m1.y < info_dest.y + info_dest.h)) 
			return;

		SDL_BlitSurface(back, NULL, screen, &info_dest);
		display_text_loop(font_extra, "Tanmay Gatle", 20, 570, 0, 0, 255, 0, 0);
		
		SDL_Flip(screen);
	}
}

/*Moves the player car left or right or jump*/
void move_player() {
	if(SDL_PollEvent(&event)) {
		if(event.type == SDL_QUIT) {
			all_quit();
			exit(0);
		}
		if(event.type == SDL_KEYDOWN) {
			if(event.key.keysym.sym == SDLK_ESCAPE) {
				all_quit();
				exit(0);
			}
			else if ((event.key.keysym.sym == SDLK_SPACE) && (jump_timer >= 250)) {
				jump = 1;
				jump_timer = 0;
				return;
			}
			else
				keyheld[event.key.keysym.sym] = 1;
			}
			if(event.type == SDL_KEYUP) {
				keyheld[event.key.keysym.sym] = 0;
			}
		}
		if(keyheld[SDLK_LEFT] == 1) {
			destination.x -= 4.5f;
			if(destination.x < 70)
				destination.x = 70; 				      	
		}
		if(keyheld[SDLK_RIGHT] == 1) {
			destination.x += 4.5f;
			if((destination.x + player->w) > 570) 
				destination.x = 570 - player->w;
		}
}

/*Control's the jumping action ie whether jump is charged, speed of jump*/
void jump_car() {
	SDL_BlitSurface(jump_img[jump_i], NULL, screen, &destination);
	jump_count++;
	if(jump_count % jump_lim == 0 )
		jump_i++;
	if(jump_i == 10) 
		jump = jump_count = jump_i = 0;
}

/*Displays the scrolling background*/
void display_bg() {
	bgdest.y += bgv;
	if(bgdest.y >= 600)
		bgdest.y = 0;

	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	SDL_BlitSurface(bgimage, NULL, screen, &bgdest);
	bgdest_temp.y = bgdest.y - bgimage->h;  
	SDL_BlitSurface(bgimage, NULL, screen, &bgdest_temp);
}

/*Displays the player and oncoming cars*/
void display_cars() {
	if(!jump)
		SDL_BlitSurface(player, NULL, screen, &destination);

	int i;
	for(i = 0; i < 2; i++)
		SDL_BlitSurface(car[i], NULL, screen, &destination_c[i]);
	if((SDL_GetTicks() - start) >= 20000)
		SDL_BlitSurface(car[2], NULL, screen, &destination_c[2]);
	if((SDL_GetTicks() - start) >= 50000)
		SDL_BlitSurface(car[3], NULL, screen, &destination_c[3]);
}

/*Displays the final gameover screen, calculates the score, converts to a strings and displays the score
 and also stores the score in corresponding file*/
void display_gameover() {
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	bgdest.y = 0;
	SDL_BlitSurface(bgimage, NULL, screen, &bgdest);

	SDL_Rect gameover_rect = {198, 99, 255, 39};	
	SDL_FillRect(screen, &gameover_rect, SDL_MapRGB(screen->format, 0, 0, 0));
	display_text("GAME OVER", 40, 200, 100 , 0, 0, 255, 0, 0);
			
	inttostr(time1, time_str);
	SDL_Rect score_rect = {148, 168, 340, 30};	
	SDL_FillRect(screen, &score_rect, SDL_MapRGB(screen->format, 0, 0, 0));
	display_text("SCORE", 20, 150, 170, 0, 0, 255, 0, 0);
	if(choose_diff == EASY)
		display_text("( EASY ) : ", 20, 240, 170, 0, 0, 255, 0, 0);
	else if(choose_diff == MEDIUM)
		display_text("( MEDIUM ) : ", 20, 240, 170, 0, 0, 255, 0, 0);
	else if(choose_diff == HARD)
		display_text("( HARD ) : ", 20, 240, 170, 0, 0, 255, 0, 0);
	display_text(time_str, 20, 390, 170, 0, 0, 255, 0, 0);
			
	display_text("Tanmay Gatle", 12, 20, 570, 0, 0, 255, 0, 0);
}

/*Displays the highscore table*/
void display_highscore() {
	int i = 0;
	SDL_Rect l_rect = {95, 220, 400, 200};	
	char index[3], score_str[MAX];
	leaderboard();
	
	SDL_FillRect(screen, &l_rect, SDL_MapRGB(screen->format, 0, 0, 0));
	display_text("HIGH SCORES ", 20, 100, 225 ,0, 0 ,255, 0, 0);
	if(choose_diff == EASY)
		display_text("( EASY ) : ", 20, 250, 225 ,0, 0 ,255, 0, 0);
	else if(choose_diff == MEDIUM)
		display_text("( MEDIUM ) : ", 20, 250, 225 ,0, 0 ,255, 0, 0);
	else if(choose_diff == HARD)
		display_text("( HARD ) : ", 20, 250, 225 ,0, 0 ,255, 0, 0);
	
	font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 15);

	for(i = 0; i < 5; i++) {
		inttostr((i + 1), index);
		display_text_loop(font, index, 105, 255 + 30 * i, 0, 0, 255, 0, 0);
		if(max[i].score != 0) {
			display_text_loop(font, max[i].name, 155, 255 + 30 * i, 0, 0, 255, 0, 0);
			inttostr(max[i].score, score_str);
			display_text_loop(font, score_str, 250, 255 + 30 * i, 0, 0, 255, 0, 0);
		}
	}
}

/*Takes the player name from keyboard*/
void input_name() {
	SDL_EnableUNICODE(SDL_ENABLE);
	
	int i = 0;
	font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 20);
	SDL_Rect name_rect = {245, 249, 250, 25};
	SDL_FillRect(screen, &name_rect, SDL_MapRGB(screen->format, 255, 255, 255));
	SDL_Rect enter_rect = {99, 249, 143, 21};	
	SDL_FillRect(screen, &enter_rect, SDL_MapRGB(screen->format, 0, 0, 0));
	display_text("ENTER NAME: ", 20, 100, 250 ,0, 0 ,255, 0, 0);
	SDL_Flip(screen);

	while(1)
	{
		if(SDL_PollEvent(&event)) {	
			if(event.type == SDL_KEYDOWN)
			{
				if(strlen(name_str) <= 16)
				{
					if((event.key.keysym.unicode == ' ') || ((event.key.keysym.unicode >= '0') && (event.key.keysym.unicode <= '9')) || ((event.key.keysym.unicode >= 'A') && (event.key.keysym.unicode <= 'Z')) || ((event.key.keysym.unicode >= 'a') && (event.key.keysym.unicode <= 'z')))
					{
						name_str[i] = event.key.keysym.unicode;
						i++;
						name_str[i] = '\0';
						SDL_FillRect(screen, &name_rect, SDL_MapRGB(screen->format, 255, 255, 255));	
						display_text_loop(font, name_str, 250, 250, 0, 0, 255, 0, 0);
						SDL_Flip(screen);
					}
				}
				
				if((event.key.keysym.sym == SDLK_BACKSPACE) && (strlen(name_str) != 0))
				{	
					if(i > 1) {
						name_str[i - 1] = '\0';
						i--;
					}
					else if(i <= 1) {
						name_str[0] = ' ';
						name_str[1] = '\0';
						i = 0;
					}

					SDL_FillRect(screen, &name_rect, SDL_MapRGB(screen->format, 255, 255, 255));
					display_text_loop(font, name_str, 250, 250, 0, 0, 255, 0, 0);
					SDL_Flip(screen);
				}
			}

			if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_RETURN))
			{
				SDL_FillRect(screen, &name_rect, SDL_MapRGB(screen->format, 255, 255, 255));
				display_text_loop(font, name_str, 250, 250, 0, 0, 255, 0, 0);
				SDL_Flip(screen);

				SDL_EnableUNICODE(SDL_DISABLE);
				
				if(!check_score(name_str, time1))			
					fprintf(fp, "%s\t%d\n", name_str, time1);
	
				return;
			}

			if(event.type == SDL_QUIT) {
					SDL_EnableUNICODE(SDL_DISABLE);
					all_quit();
					exit(0);
			}

			if(event.type == SDL_KEYDOWN)
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					SDL_EnableUNICODE(SDL_DISABLE);
					all_quit();
					exit(0);
				}
	
		}
		
	}	
	
}

/*Checks whether the same score with same player name is present in the file or not*/ 
int check_score(char *name, int score) {
	rewind(fp);
	total_score temp;
	while(fscanf(fp, "%s%d", temp.name, &temp.score) != -1) 
		if((score == temp.score) && (strcmp(name, temp.name)))
			return 1;
	return 0;
}

/*Takes mouse input and returns the x,y coordinates of the click*/
mouse_click click() {
	mouse_click m;
	if(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				all_quit();
				exit(0);
			}
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					all_quit();
					exit(0);
				}
			}
			if(event.type == SDL_MOUSEBUTTONDOWN)
				if(event.button.button == SDL_BUTTON_LEFT) {	
					m.x = event.button.x;
					m.y = event.button.y;
					return m;
				}
	}
}

/*Display TTF*/
void display_text_loop(TTF_Font* f, char *str, int x, int y, int w, int h, int r, int g, int b) {

	SDL_Color textcolor = {r, g, b};
        SDL_Rect textlocation = {x, y, w, h};
	
	text = TTF_RenderText_Solid(f, str, textcolor);
	if(text == NULL) {
		printf("Text not rendered");
		exit(1);
	}

	SDL_BlitSurface(text, NULL, screen, &textlocation);
}

/*Display TTF*/
void display_text(char *str, int size, int x, int y, int w, int h, int r, int g, int b) {

	TTF_Font* font_temp = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", size);
	if (font_temp == NULL) {
		printf("Font not initialized");
		exit (1);
	}
 
	SDL_Color textcolor = {r, g, b};
        SDL_Rect textlocation = {x, y, w, h};
	
	text = TTF_RenderText_Solid(font_temp, str, textcolor);
	if(text == NULL) {
		printf("Text not rendered");
		exit(1);
	}

	SDL_BlitSurface(text, NULL, screen, &textlocation);
}

/*Searches the top 5 highest scores in the file along with the name*/
void leaderboard() {
	rewind(fp);
	int i,j;
	total_score temp1, temp2;
	for(i = 0; i < 5; i++)
		max[i].score = 0;
	while(fscanf(fp, "%s%d", temp1.name, &temp1.score) != -1) {
		for(i = 0; i < 5; i++)		
			if(temp1.score >= max[i].score)
				for(j = i; j < 5; j++) {
					temp2.score = max[j].score;
					max[j].score = temp1.score;
					temp1.score = temp2.score;
				
					strcpy(temp2.name, max[j].name);
					strcpy(max[j].name, temp1.name);
					strcpy(temp1.name, temp2.name);					
				}
	} 			
}

/*COnverts and integer intoa string*/
void inttostr(int n, char arr[]) {
	int count, i = 0, j;
	char temp;
	do {
		arr[i++] = (n % 10) + 48;
		n /= 10;
	} while(n != 0);
	arr[i] = '\0';
	i--;
	for(j = 0; j <= i/2; j++) {
		temp = arr[j];
		arr[j] = arr[i - j];
		arr[i - j] = temp; 
	}
}

/*Sets the SDL_Rect for a corresponding SDL_Surface*/
SDL_Rect set_dest(SDL_Surface *temp, int x, int y) {
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = temp->w;
	dest.h = temp->h;
	return dest;
}

/*Frees all surfaces and disables SDL*/
void all_quit() {
	int i;
	SDL_FreeSurface(player);
	SDL_FreeSurface(bgimage);
	SDL_FreeSurface(play);
	SDL_FreeSurface(quit);
	SDL_FreeSurface(replay);
	SDL_FreeSurface(quitover);
	SDL_FreeSurface(returnmain);
	SDL_FreeSurface(settings);
	SDL_FreeSurface(info);
	SDL_FreeSurface(back);
	SDL_FreeSurface(startcar);	
	
	for(i = 0; i < 4; i++)
		SDL_FreeSurface(car[i]);
	for(i = 0; i < 3; i++) {
		SDL_FreeSurface(bgm[i]);
		SDL_FreeSurface(difficulty[i]);
	}
	for(i = 0; i < 6; i++)
		SDL_FreeSurface(jump_img[i]);
	
	SDL_FreeSurface(text);
	TTF_CloseFont(font);
	TTF_CloseFont(font_extra);
	
	fclose(fp);
	TTF_Quit();
	SDL_Quit();
}

/*Returns a random value ie location between lim1 and lim2*/
long int rand2() {
	long int v;
	while(1) {
		if((v = (random() % lim2)) >= lim1)
			return v;
	}
}

/*Maintains the framerate*/
void delay(unsigned int frameLimit)
{
	unsigned int ticks = SDL_GetTicks();

	if (frameLimit < ticks)
		return;

	if (frameLimit > ticks + 16)
		SDL_Delay(16);
	
	else
		SDL_Delay(frameLimit - ticks);
}

/*Checks whether cars are colliding*/
int checkCollision(SDL_Rect r1, SDL_Rect r2) {
	int left_r1, right_r1, top_r1, bottom_r1;
	left_r1 = r1.x; 
	right_r1 = r1.x + r1.w;
	top_r1 = r1.y;
	bottom_r1 = r1.y + r1.h;

	int left_r2, right_r2, top_r2, bottom_r2;
	left_r2 = r2.x; 
	right_r2 = r2.x + r2.w;
	top_r2 = r2.y;
	bottom_r2 = r2.y + r2.h;

	if(top_r1 >= bottom_r2)
		return 0;
	else if(bottom_r1 <= top_r2)
		return 0;
	else if(left_r1 >= right_r2)
		return 0;
	else if(right_r1 <= left_r2)
		return 0; 
	else 
		return 1;
}

int main(int argc, char *argv[]) {
		if(argc == 2) 		//--help option
		if(!strcmp(argv[1], "--help")) {
			printf("Usage: ./project\n No arguments required.\n");
			return 0;
		}
		else {
			printf("Incorrect argument.\n'./project --help' for help \n");
			return 0;
		}
	if(argc > 2) {
		printf("Too many arguments.\n'./project --help' for help \n");
		return 0;
	}
	
	int i;
	
	init();
	
	init_dest();

	menu();

	if(choose_diff == EASY) 		//Open the repective file as per difficulty
		fp = fopen("scores_easy.txt", "a+");
	else if(choose_diff == MEDIUM)
		fp = fopen("scores_medium.txt", "a+");
	else if(choose_diff == HARD)
		fp = fopen("scores_hard.txt", "a+");
	
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	SDL_BlitSurface(bgimage, NULL, screen, &bgdest);
	SDL_Flip(screen);
	SDL_Delay(500);

	int gameOver = 1;
	for(i = 0; i < 4; i++)	
		v[i] = 0;
	for(i = 0; i < 323; i++)	
		keyheld[i] = 0;	
	mouse_click m1;
	bgdest_temp.x = 0;
	
	SDL_Rect topleft_rect = {34, 48, 185, 20}, topright_rect = {549, 48, 80, 20}, jump_rect = {449, 48, 70, 20};
	font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 18);
	int collision = 0;
	jump_timer = 0;	

	while(1)
	{
		if(gameOver) {
			start = SDL_GetTicks();
			frameLimit = SDL_GetTicks() + 16;
		}
		while(gameOver)
		{
			if(!jump)
				move_player();
			
			//The three different stages		
			if((SDL_GetTicks() - start) < 20000) {
				if(destination_c[0].y == 0) {
					destination_c[0].x = 85 + (random() % 200);
					v[0] = rand2();
				}
				if(destination_c[1].y == 0) {
					destination_c[1].x = 535 - (random() % 200);
					v[1] = rand2();
				}
	
				for(i = 0; i < 2; i++) {
					destination_c[i].y += v[i];
					if(destination_c[i].y >= 700) 
						destination_c[i].y = 0;
				}
				
				if(!jump) {
					for(i = 0; i < 2; i++) {
						if(checkCollision(destination, destination_c[i])) {
								time1 = SDL_GetTicks() - start;
								collision = 1;
								SDL_Delay(500);
								break;	
						}
					}
				}
				if(collision == 1) {
					collision = 0;					
					break;
				}
			}  
				
			else if((SDL_GetTicks() - start) < 50000) {  
				if(destination_c[0].y == 0) {
					destination_c[0].x = 80 + (random() % 100);
					v[0] = rand2();
				}
				if(destination_c[1].y == 0) {
					destination_c[1].x = (540 - car[1]->w) - (random() % 100);
					v[1] = rand2();
				}
				if(destination_c[2].y == 0) {
					destination_c[2].x = 200 + (random() % 150);
					v[2] = rand2();
				}

				for(i = 0; i < 3; i++) {
					destination_c[i].y += v[i];
					if(destination_c[i].y >= 700) 
						destination_c[i].y = 0;
				}
				if(!jump) {
					for(i = 0; i < 3; i++) {
						if(checkCollision(destination, destination_c[i])) {
								time1 = SDL_GetTicks() - start;
								collision = 1;
								SDL_Delay(500);
								break;	
						}
					}
				}
				if(collision == 1) {
					collision = 0;					
					break;
				}			
			}				
			else {	
				if(destination_c[0].y == 0) {
					destination_c[0].x = 80 + (random() % 100);
					v[0] = rand2();
				}
				if(destination_c[1].y == 0) {
					destination_c[1].x = 540 - (random() % 100);
					v[1] = rand2();
				}
				if(destination_c[2].y == 0) {
					destination_c[2].x = 180 + (random() % 100);
					v[2] = rand2();
				}
				if(destination_c[3].y == 0) {
					destination_c[3].x = 300 + (random() % 100);
					v[3] = rand2();
				}
	
				for(i = 0; i < 4; i++) {
					destination_c[i].y += v[i];
					if(destination_c[i].y >= 700) 
						destination_c[i].y = 0;
				}

				if(!jump) {
					for(i = 0; i < 4; i++) {
						if(checkCollision(destination, destination_c[i])) {
								time1 = SDL_GetTicks() - start;
								collision = 1;
								SDL_Delay(500);
								break;	
						}
					}
				}
				if(collision == 1) {
					collision = 0;					
					break;
				}			
			}
				
			display_bg();
			
			display_cars();

			SDL_FillRect(screen, &topleft_rect, SDL_MapRGB(screen->format, 0, 0, 0));
			SDL_FillRect(screen, &topright_rect, SDL_MapRGB(screen->format, 0, 0, 0));
			SDL_FillRect(screen, &jump_rect, SDL_MapRGB(screen->format, 0, 0, 0));
			
			inttostr((SDL_GetTicks() - start), time_str);
			display_text_loop(font, "SCORE: ", 35, 50, 0, 0, 255, 0, 0);
			display_text_loop(font, time_str, 150, 50, 0, 0, 255, 0, 0);
			
			if(choose_diff == EASY)
				display_text_loop(font, "EASY", 550, 50, 0, 0, 255, 0, 0);
			else if(choose_diff == MEDIUM)
				display_text_loop(font, "MEDIUM", 550, 50, 0, 0, 255, 0, 0);
			else if(choose_diff == HARD)
				display_text_loop(font, "HARD", 550, 50, 0, 0, 255, 0, 0);

			if(jump) 
				jump_car();

			jump_timer++;
			if(jump_timer >= 250)
				display_text_loop(font, "JUMP", 450, 50, 0, 0, 0, 255, 0);
			else
				display_text_loop(font, "JUMP", 450, 50, 0, 0, 255, 0, 0);

			SDL_Flip(screen);

			delay(frameLimit);
			frameLimit = SDL_GetTicks() + 16;
		}
		if(gameOver) {
			gameOver = 0;

			display_gameover();

			input_name();

			display_highscore();
		}

		m1 = click();	

		//Displays the final buttons and takes mouse input*/	
		if((m1.x > return_dest.x) && (m1.x < return_dest.x + return_dest.w) && (m1.y > return_dest.y) && (m1.y < return_dest.y + return_dest.h)) {
			flag1 = 0;
			main(0, NULL);					
		}					
		if((m1.x > replay_dest.x) && (m1.x < replay_dest.x + replay_dest.w) && (m1.y > replay_dest.y) && (m1.y < replay_dest.y + replay_dest.h)) {
			flag1 = 1;
			main(0, NULL);				
		}
		if((m1.x > quitover_dest.x) && (m1.x < quitover_dest.x + quitover_dest.w) && (m1.y > quitover_dest.y) && (m1.y < quitover_dest.y + quitover_dest.h)) {
			all_quit();
			exit(0);
		}
		
		SDL_BlitSurface(replay, NULL, screen, &replay_dest);
		SDL_BlitSurface(returnmain, NULL, screen, &return_dest);
		SDL_BlitSurface(quitover, NULL, screen, &quitover_dest);

		SDL_Flip(screen);
	}
	all_quit();
	return 0;
}
