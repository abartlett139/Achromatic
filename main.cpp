#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Player.h"
#include "Enemy.h"
#include "Turret.h"


SDL_Window *window;
SDL_Renderer *ren;
SDL_GameController *gc;
bool quit = false;


float musicVol = 20;
float fxVol = 100;
float deadZone = 5000;

class GameState {
public:
	SDL_Event *e;
	float deltaTime, currentTime;

	GameState() {
		e = new SDL_Event();
		deltaTime = 0;
		currentTime = SDL_GetTicks();
	}

	~GameState() {
		delete e;
	}

	virtual void enter() = 0;
	virtual void prime() = 0;
	inline virtual void update() = 0;
};

GameState *gameState, *menu, *options, *instructions, *backStory, *levelOne, *levelTwo, *win, *lose;


class Menu :public GameState {
public:
	SDL_Rect bgPos[2], titlePos, btnPos[5], cursorPos;
	SDL_Texture *bg, *title, *btn[5], *btnOver[5], *cursor;
	Mix_Music* bgm;
	Mix_Chunk *overSnd, *clickSnd;
	Sint16 leftx, lefty;
	float cursorSpeed, cx, cy, scrollSpeed, bgMove[2];
	bool isBtn[5], isPlayed;


	Menu() {

		cursorSpeed = .05;
		scrollSpeed = 50;
		bgMove[0] = 0;
		bgMove[1] = -3200;

		bgm = Mix_LoadMUS("sounds/introBGM.mp3");
		overSnd = Mix_LoadWAV("sounds/over.wav");
		clickSnd = Mix_LoadWAV("sounds/click.wav");
		isPlayed = false;

		//load textures
		bg = IMG_LoadTexture(ren, "sprites/background.png");
		title = IMG_LoadTexture(ren, "sprites/achromatic.png");
		btn[0] = IMG_LoadTexture(ren, "sprites/play.png");
		btn[1] = IMG_LoadTexture(ren, "sprites/options.png");
		btn[2] = IMG_LoadTexture(ren, "sprites/instructions.png");
		btn[3] = IMG_LoadTexture(ren, "sprites/backstory.png");
		btn[4] = IMG_LoadTexture(ren, "sprites/quit.png");
		btnOver[0] = IMG_LoadTexture(ren, "sprites/playOver.png");
		btnOver[1] = IMG_LoadTexture(ren, "sprites/optionsOver.png");
		btnOver[2] = IMG_LoadTexture(ren, "sprites/instructionsOver.png");
		btnOver[3] = IMG_LoadTexture(ren, "sprites/backstoryOver.png");
		btnOver[4] = IMG_LoadTexture(ren, "sprites/quitOver.png");
		cursor = IMG_LoadTexture(ren, "sprites/cursor.png");

		//set up rectangles
		SDL_QueryTexture(bg, NULL, NULL, &bgPos[0].w, &bgPos[0].h);
		bgPos[0].x = 0;
		bgPos[0].y = 0;

		SDL_QueryTexture(bg, NULL, NULL, &bgPos[1].w, &bgPos[1].h);
		bgPos[1].x = -3200;
		bgPos[1].y = 0;

		SDL_QueryTexture(title, NULL, NULL, &titlePos.w, &titlePos.h);
		titlePos.x = 0;
		titlePos.y = 20;

		for (int i = 0; i < 5; i++) {
			SDL_QueryTexture(btn[i], NULL, NULL, &btnPos[i].w, &btnPos[i].h);
			btnPos[i].x = 400 - (btnPos[i].w / 2);
		}
		btnPos[0].y = 185;
		btnPos[1].y = 257;
		btnPos[2].y = 329;
		btnPos[3].y = 401;
		btnPos[4].y = 473;

		SDL_QueryTexture(cursor, NULL, NULL, &cursorPos.w, &cursorPos.h);
		cursorPos.x = 200;
		cursorPos.y = 200;
	}

	~Menu() {
		SDL_DestroyTexture(bg);
		SDL_DestroyTexture(title);
		for (int i = 0; i < 5; i++) {
			SDL_DestroyTexture(btn[i]);
			SDL_DestroyTexture(btnOver[i]);
		}
		SDL_DestroyTexture(cursor);

	}

	void enter() {
		Mix_Volume(-1, fxVol);
		if (!Mix_PlayingMusic()) {
			Mix_PlayMusic(bgm, -1);
			Mix_VolumeMusic(musicVol);
		}
	}

	void prime() {
		enter();
		bool changeState = false;
		while (!changeState) {
			//check for cursor-button intersection
			if (SDL_HasIntersection(&cursorPos, &btnPos[0]) && !isBtn[1]) { isBtn[0] = true; }
			else { isBtn[0] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[1]) && !isBtn[0] && !isBtn[2]) { isBtn[1] = true; }
			else { isBtn[1] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[2]) && !isBtn[1] && !isBtn[3]) { isBtn[2] = true; }
			else { isBtn[2] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[3]) && !isBtn[2] && !isBtn[4]) { isBtn[3] = true; }
			else { isBtn[3] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[4]) && !isBtn[3]) { isBtn[4] = true; }
			else { isBtn[4] = false; }

			//play over sound
			if (isBtn[0] || isBtn[1] || isBtn[2] || isBtn[3] || isBtn[4]) {
				if (!isPlayed) {
					Mix_PlayChannel(-1, overSnd, 0);
					isPlayed = true;
				}
			}
			else {
				Mix_HaltChannel(-1);
				isPlayed = false;
			}


			//controller events
			while (SDL_PollEvent(e)) {
				switch (e->type) {
				case SDL_CONTROLLERBUTTONDOWN:
					switch (e->cbutton.button) {
					case SDL_CONTROLLER_BUTTON_A:
						for (int i = 0; i < 5; i++) {
							if (isBtn[i]) {
								changeState = true;
								Mix_PlayChannel(-1, clickSnd, 0);
								switch (i) {
								case 0:
									gameState = levelOne;
									break;
								case 1:
									gameState = options;
									break;
								case 2:
									gameState = instructions;
									break;
								case 3:
									gameState = backStory;
									break;
								case 4:
									quit = true;
									break;
								default:break;
								}
							}
						}
						break;
					default:break;
					}
					break;
				case SDL_CONTROLLERAXISMOTION:
					leftx = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
					lefty = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
					break;
				case SDL_QUIT:
					quit = true;
					break;
				default:break;
				}
				break;
			}
			update();
			SDL_RenderClear(ren);
			SDL_RenderCopy(ren, bg, NULL, &bgPos[0]);
			SDL_RenderCopyEx(ren, bg, NULL, &bgPos[1], 0, NULL, SDL_FLIP_NONE);
			SDL_RenderCopy(ren, title, NULL, &titlePos);
			for (int i = 0; i < 5; i++) {
				if (!isBtn[i]) { SDL_RenderCopy(ren, btn[i], NULL, &btnPos[i]); }
				else { SDL_RenderCopy(ren, btnOver[i], NULL, &btnPos[i]); }
			}
			SDL_RenderCopy(ren, cursor, NULL, &cursorPos);
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
			SDL_RenderPresent(ren);
		}
	}

	inline void update() {
		deltaTime = (SDL_GetTicks() - currentTime) / 1000;
		currentTime = SDL_GetTicks();
		if (bgPos[0].x >= bgPos[0].w) { bgMove[0] -= bgPos[0].w * 2; }
		if (bgPos[1].x >= bgPos[1].w) { bgMove[1] -= bgPos[1].w * 2; }
		bgMove[0] += scrollSpeed*deltaTime;
		bgMove[1] += scrollSpeed*deltaTime;
		bgPos[0].x = bgMove[0];
		bgPos[1].x = bgMove[1];

		if (leftx > deadZone || leftx < -deadZone) { cx += leftx*deltaTime*cursorSpeed; }
		if (lefty > deadZone || lefty < -deadZone) { cy += lefty*deltaTime*cursorSpeed; }
		cursorPos.x = cx;
		cursorPos.y = cy;
	}
};

class Options :public GameState {
public:
	SDL_Rect bgPos[2], titlePos, btnPos[4], optTxtPos[3], optPos[3], cursorPos;
	SDL_Texture *bg, *title, *btn[4], *btnOver[4], *opt[5], *optTxt[3], *cursor;
	Mix_Music* bgm;
	Mix_Chunk *overSnd, *clickSnd, *slideSnd;
	Sint16 leftx, lefty;
	float cursorSpeed, cx, cy, scrollSpeed, bgMove[2];
	bool isBtn[4], isOpt[3], isPlayed;

	Options() {
		cursorSpeed = .05;
		scrollSpeed = 50;
		bgMove[0] = 0;
		bgMove[1] = -3200;

		bgm = Mix_LoadMUS("sounds/introBGM.mp3");
		overSnd = Mix_LoadWAV("sounds/over.wav");
		clickSnd = Mix_LoadWAV("sounds/click.wav");
		slideSnd = Mix_LoadWAV("sounds/over.wav");
		isPlayed = false;

		//load textures
		bg = IMG_LoadTexture(ren, "sprites/background.png");
		title = IMG_LoadTexture(ren, "sprites/optionsTitle.png");
		btn[0] = IMG_LoadTexture(ren, "sprites/mainMenu.png");
		btn[1] = IMG_LoadTexture(ren, "sprites/backstory.png");
		btn[2] = IMG_LoadTexture(ren, "sprites/instructions.png");
		btn[3] = IMG_LoadTexture(ren, "sprites/play.png");
		btnOver[0] = IMG_LoadTexture(ren, "sprites/mainMenuOver.png");
		btnOver[1] = IMG_LoadTexture(ren, "sprites/backstoryOver.png");
		btnOver[2] = IMG_LoadTexture(ren, "sprites/instructionsOver.png");
		btnOver[3] = IMG_LoadTexture(ren, "sprites/playOver.png");
		cursor = IMG_LoadTexture(ren, "sprites/cursor.png");

		//set up rectangles
		SDL_QueryTexture(bg, NULL, NULL, &bgPos[0].w, &bgPos[0].h);
		bgPos[0].x = 0;
		bgPos[0].y = 0;

		SDL_QueryTexture(bg, NULL, NULL, &bgPos[1].w, &bgPos[1].h);
		bgPos[1].x = -3200;
		bgPos[1].y = 0;

		SDL_QueryTexture(title, NULL, NULL, &titlePos.w, &titlePos.h);
		titlePos.x = 0;
		titlePos.y = 20;

		for (int i = 0; i < 4; i++) {
			SDL_QueryTexture(btn[i], NULL, NULL, &btnPos[i].w, &btnPos[i].h);
			btnPos[i].x = 200 - (btnPos[i].w / 2);
		}
		btnPos[0].y = 185;
		btnPos[1].y = 257;
		btnPos[2].y = 329;
		btnPos[3].y = 401;

		SDL_QueryTexture(cursor, NULL, NULL, &cursorPos.w, &cursorPos.h);
		cursorPos.x = 200;
		cursorPos.y = 200;
	}

	~Options() {
		SDL_DestroyTexture(bg);
		SDL_DestroyTexture(title);
		for (int i = 0; i < 3; i++) {
			SDL_DestroyTexture(optTxt[i]);
		}
		for (int i = 0; i < 4; i++) {
			SDL_DestroyTexture(btn[i]);
			SDL_DestroyTexture(btnOver[i]);
		}
		for (int i = 0; i < 5; i++) {
			SDL_DestroyTexture(opt[i]);
		}
		SDL_DestroyTexture(cursor);

	}

	void enter() {
		Mix_Volume(-1, fxVol);
		if (!Mix_PlayingMusic()) {
			Mix_PlayMusic(bgm, -1);
			Mix_VolumeMusic(musicVol);
		}
	}

	void prime() {
		enter();
		bool changeState = false;
		while (!changeState) {
			//check for cursor-button intersection
			if (SDL_HasIntersection(&cursorPos, &btnPos[0]) && !isBtn[1]) { isBtn[0] = true; }
			else { isBtn[0] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[1]) && !isBtn[0] && !isBtn[2]) { isBtn[1] = true; }
			else { isBtn[1] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[2]) && !isBtn[1] && !isBtn[3]) { isBtn[2] = true; }
			else { isBtn[2] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[3]) && !isBtn[2]) { isBtn[3] = true; }
			else { isBtn[3] = false; }
			if (SDL_HasIntersection(&cursorPos, &optPos[0]) && !isOpt[1]) { isOpt[0] = true; }
			else { isOpt[0] = false; }
			if (SDL_HasIntersection(&cursorPos, &optPos[1]) && !isOpt[0] && !isOpt[2]) { isOpt[1] = true; }
			else { isOpt[1] = false; }
			if (SDL_HasIntersection(&cursorPos, &optPos[2]) && !isOpt[1]) { isOpt[2] = true; }
			else { isOpt[2] = false; }

			//play over sound
			if (isBtn[0] || isBtn[1] || isBtn[2] || isBtn[3]) {
				if (!isPlayed) {
					Mix_PlayChannel(-1, overSnd, 0);
					isPlayed = true;
				}
			}
			else {
				Mix_HaltChannel(-1);
				isPlayed = false;
			}


			//controller events
			while (SDL_PollEvent(e)) {
				switch (e->type) {
				case SDL_CONTROLLERBUTTONDOWN:
					switch (e->cbutton.button) {
					case SDL_CONTROLLER_BUTTON_A:
						for (int i = 0; i < 4; i++) {
							if (isBtn[i]) {
								changeState = true;
								Mix_PlayChannel(-1, clickSnd, 0);
								switch (i) {
								case 0:
									gameState = menu;
									break;
								case 1:
									gameState = backStory;
									break;
								case 2:
									gameState = instructions;
									break;
								case 3:
									gameState = levelOne;
									break;
								default:break;
								}
							}
						}
						for (int i = 0; i < 3; i++) {
							Mix_PlayChannel(-1, slideSnd, 0);
							switch (i) {
							case 0:
								musicVol += 25;
								if (musicVol > 100) { musicVol = 0; }
								Mix_VolumeMusic(musicVol);
								break;
							case 1:
								fxVol += 25;
								if (fxVol > 100) { fxVol = 0; }
								Mix_Volume(-1, fxVol);
								break;
							case 2:
								deadZone += 1000;
								if (deadZone > 1000) { deadZone = 0; }
								break;
							default:break;
							}
						}
						break;
					case SDL_CONTROLLER_BUTTON_B:
						changeState = true;
						gameState = menu;
						break;
					default:break;
					}
					break;
				case SDL_CONTROLLERAXISMOTION:
					leftx = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
					lefty = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
					break;
				case SDL_QUIT:
					quit = true;
					break;
				default:break;
				}
			}
			break;
		}
		update();
		SDL_RenderClear(ren);
		SDL_RenderCopy(ren, bg, NULL, &bgPos[0]);
		SDL_RenderCopyEx(ren, bg, NULL, &bgPos[1], 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopy(ren, title, NULL, &titlePos);
		for (int i = 0; i < 4; i++) {
			if (!isBtn[i]) { SDL_RenderCopy(ren, btn[i], NULL, &btnPos[i]); }
			else { SDL_RenderCopy(ren, btnOver[i], NULL, &btnPos[i]); }
		}
		//also add the 3 opt sliders and opt text
		SDL_RenderCopy(ren, cursor, NULL, &cursorPos);
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
		SDL_RenderPresent(ren);
	}

	inline void update() {
		deltaTime = (SDL_GetTicks() - currentTime) / 1000;
		currentTime = SDL_GetTicks();
		if (bgPos[0].x >= bgPos[0].w) { bgMove[0] -= bgPos[0].w * 2; }
		if (bgPos[1].x >= bgPos[1].w) { bgMove[1] -= bgPos[1].w * 2; }
		bgMove[0] += scrollSpeed*deltaTime;
		bgMove[1] += scrollSpeed*deltaTime;
		bgPos[0].x = bgMove[0];
		bgPos[1].x = bgMove[1];

		if (leftx > deadZone || leftx < -deadZone) { cx += leftx*deltaTime*cursorSpeed; }
		if (lefty > deadZone || lefty < -deadZone) { cy += lefty*deltaTime*cursorSpeed; }
		cursorPos.x = cx;
		cursorPos.y = cy;
	}
};

class Instructions :public GameState {
public:
	SDL_Rect bgPos[2], titlePos, txtPos, btnPos[3], cursorPos;
	SDL_Texture *bg, *title, *txt, *btn[3], *btnOver[3], *cursor;
	Mix_Music* bgm;
	Mix_Chunk *overSnd, *clickSnd;
	Sint16 leftx, lefty;
	float cursorSpeed, cx, cy, scrollSpeed, bgMove[2];
	bool isBtn[3], isPlayed;

	Instructions() {
		cursorSpeed = .05;
		scrollSpeed = 50;
		bgMove[0] = 0;
		bgMove[1] = -3200;

		bgm = Mix_LoadMUS("sounds/introBGM.mp3");
		overSnd = Mix_LoadWAV("sounds/over.wav");
		clickSnd = Mix_LoadWAV("sounds/click.wav");
		isPlayed = false;

		//load textures
		bg = IMG_LoadTexture(ren, "sprites/background.png");
		title = IMG_LoadTexture(ren, "sprites/instructionsTitle.png");
		txt = IMG_LoadTexture(ren, "sprites/instructText.png");
		btn[0] = IMG_LoadTexture(ren, "sprites/mainMenu.png");
		btn[1] = IMG_LoadTexture(ren, "sprites/backstory.png");
		btn[2] = IMG_LoadTexture(ren, "sprites/play.png");
		btnOver[0] = IMG_LoadTexture(ren, "sprites/mainMenuOver.png");
		btnOver[1] = IMG_LoadTexture(ren, "sprites/backstoryOver.png");
		btnOver[2] = IMG_LoadTexture(ren, "sprites/playOver.png");
		cursor = IMG_LoadTexture(ren, "sprites/cursor.png");

		//set up rectangles
		SDL_QueryTexture(bg, NULL, NULL, &bgPos[0].w, &bgPos[0].h);
		bgPos[0].x = 0;
		bgPos[0].y = 0;

		SDL_QueryTexture(bg, NULL, NULL, &bgPos[1].w, &bgPos[1].h);
		bgPos[1].x = -3200;
		bgPos[1].y = 0;

		SDL_QueryTexture(title, NULL, NULL, &titlePos.w, &titlePos.h);
		titlePos.x = 0;
		titlePos.y = 20;

		SDL_QueryTexture(txt, NULL, NULL, &txtPos.w, &txtPos.h);
		txtPos.x = 100;
		txtPos.y = 100;

		for (int i = 0; i < 3; i++) {
			SDL_QueryTexture(btn[i], NULL, NULL, &btnPos[i].w, &btnPos[i].h);
			btnPos[i].x = 200 - (btnPos[i].w / 2);
		}
		btnPos[0].y = 185;
		btnPos[1].y = 257;
		btnPos[2].y = 329;

		SDL_QueryTexture(cursor, NULL, NULL, &cursorPos.w, &cursorPos.h);
		cursorPos.x = 200;
		cursorPos.y = 200;
	}

	~Instructions() {
		SDL_DestroyTexture(bg);
		SDL_DestroyTexture(title);
		SDL_DestroyTexture(txt);
		for (int i = 0; i < 3; i++) {
			SDL_DestroyTexture(btn[i]);
			SDL_DestroyTexture(btnOver[i]);
		}
		SDL_DestroyTexture(cursor);

	}

	void enter() {
		if (!Mix_PlayingMusic()) {
			Mix_PlayMusic(bgm, -1);
			Mix_VolumeMusic(musicVol);
		}
		Mix_Volume(-1, fxVol);

	}

	void prime() {
		enter();
		bool changeState = false;
		while (!changeState) {
			//check for cursor-button intersection
			if (SDL_HasIntersection(&cursorPos, &btnPos[0]) && !isBtn[1]) { isBtn[0] = true; }
			else { isBtn[0] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[1]) && !isBtn[0] && !isBtn[2]) { isBtn[1] = true; }
			else { isBtn[1] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[2]) && !isBtn[1]) { isBtn[2] = true; }
			else { isBtn[2] = false; }


			//play over sound
			if (isBtn[0] || isBtn[1] || isBtn[2]) {
				if (!isPlayed) {
					Mix_PlayChannel(-1, overSnd, 0);
					isPlayed = true;
				}
			}
			else {
				Mix_HaltChannel(-1);
				isPlayed = false;
			}


			//controller events
			while (SDL_PollEvent(e)) {
				switch (e->type) {
				case SDL_CONTROLLERBUTTONDOWN:
					switch (e->cbutton.button) {
					case SDL_CONTROLLER_BUTTON_A:
						for (int i = 0; i < 3; i++) {
							if (isBtn[i]) {
								changeState = true;
								Mix_PlayChannel(-1, clickSnd, 0);
								switch (i) {
								case 0:
									gameState = menu;
									break;
								case 1:
									gameState = backStory;
									break;
								case 2:
									gameState = levelOne;
									break;
								default:break;
								}
							}
						}
						break;
					case SDL_CONTROLLER_BUTTON_B:
						changeState = true;
						gameState = menu;
						break;
					default:break;
					}
					break;
				case SDL_CONTROLLERAXISMOTION:
					leftx = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
					lefty = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
					break;
				case SDL_QUIT:
					quit = true;
					break;
				default:break;
				}
				break;
			}
			update();
			SDL_RenderClear(ren);
			SDL_RenderCopy(ren, bg, NULL, &bgPos[0]);
			SDL_RenderCopyEx(ren, bg, NULL, &bgPos[1], 0, NULL, SDL_FLIP_NONE);
			SDL_RenderCopy(ren, title, NULL, &titlePos);
			SDL_RenderCopy(ren, txt, NULL, &txtPos);
			for (int i = 0; i < 3; i++) {
				if (!isBtn[i]) { SDL_RenderCopy(ren, btn[i], NULL, &btnPos[i]); }
				else { SDL_RenderCopy(ren, btnOver[i], NULL, &btnPos[i]); }
			}
			SDL_RenderCopy(ren, cursor, NULL, &cursorPos);
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
			SDL_RenderPresent(ren);
		}
	}

	inline void update() {
		deltaTime = (SDL_GetTicks() - currentTime) / 1000;
		currentTime = SDL_GetTicks();
		if (bgPos[0].x >= bgPos[0].w) { bgMove[0] -= bgPos[0].w * 2; }
		if (bgPos[1].x >= bgPos[1].w) { bgMove[1] -= bgPos[1].w * 2; }
		bgMove[0] += scrollSpeed*deltaTime;
		bgMove[1] += scrollSpeed*deltaTime;
		bgPos[0].x = bgMove[0];
		bgPos[1].x = bgMove[1];

		if (leftx > deadZone || leftx < -deadZone) { cx += leftx*deltaTime*cursorSpeed; }
		if (lefty > deadZone || lefty < -deadZone) { cy += lefty*deltaTime*cursorSpeed; }
		cursorPos.x = cx;
		cursorPos.y = cy;
	}
};

class Backstory :public GameState {
public:
	SDL_Rect bgPos[2], titlePos, txtPos, btnPos[3], cursorPos;
	SDL_Texture *bg, *title, *txt, *btn[3], *btnOver[3], *cursor;
	Mix_Music* bgm;
	Mix_Chunk *overSnd, *clickSnd;
	Sint16 leftx, lefty;
	float cursorSpeed, cx, cy, scrollSpeed, bgMove[2];
	bool isBtn[3], isPlayed;

	Backstory() {
		cursorSpeed = .05;
		scrollSpeed = 50;
		bgMove[0] = 0;
		bgMove[1] = -3200;

		bgm = Mix_LoadMUS("sounds/introBGM.mp3");
		overSnd = Mix_LoadWAV("sounds/over.wav");
		clickSnd = Mix_LoadWAV("sounds/click.wav");
		isPlayed = false;

		//load textures
		bg = IMG_LoadTexture(ren, "sprites/background.png");
		title = IMG_LoadTexture(ren, "sprites/backstoryTitle.png");
		txt = IMG_LoadTexture(ren, "sprites/backstoryTxt.png");
		btn[0] = IMG_LoadTexture(ren, "sprites/mainMenu.png");
		btn[1] = IMG_LoadTexture(ren, "sprites/instructions.png");
		btn[2] = IMG_LoadTexture(ren, "sprites/play.png");
		btnOver[0] = IMG_LoadTexture(ren, "sprites/mainMenuOver.png");
		btnOver[1] = IMG_LoadTexture(ren, "sprites/instructionsOver.png");
		btnOver[2] = IMG_LoadTexture(ren, "sprites/playOver.png");
		cursor = IMG_LoadTexture(ren, "sprites/cursor.png");

		//set up rectangles
		SDL_QueryTexture(bg, NULL, NULL, &bgPos[0].w, &bgPos[0].h);
		bgPos[0].x = 0;
		bgPos[0].y = 0;

		SDL_QueryTexture(bg, NULL, NULL, &bgPos[1].w, &bgPos[1].h);
		bgPos[1].x = -3200;
		bgPos[1].y = 0;

		SDL_QueryTexture(title, NULL, NULL, &titlePos.w, &titlePos.h);
		titlePos.x = 0;
		titlePos.y = 20;

		SDL_QueryTexture(txt, NULL, NULL, &txtPos.w, &txtPos.h);
		txtPos.x = 100;
		txtPos.y = 100;

		for (int i = 0; i < 3; i++) {
			SDL_QueryTexture(btn[i], NULL, NULL, &btnPos[i].w, &btnPos[i].h);
			btnPos[i].x = 200 - (btnPos[i].w / 2);
		}
		btnPos[0].y = 185;
		btnPos[1].y = 257;
		btnPos[2].y = 329;

		SDL_QueryTexture(cursor, NULL, NULL, &cursorPos.w, &cursorPos.h);
		cursorPos.x = 200;
		cursorPos.y = 200;
	}

	~Backstory() {
		SDL_DestroyTexture(bg);
		SDL_DestroyTexture(title);
		SDL_DestroyTexture(txt);
		for (int i = 0; i < 3; i++) {
			SDL_DestroyTexture(btn[i]);
			SDL_DestroyTexture(btnOver[i]);
		}
		SDL_DestroyTexture(cursor);

	}

	void enter() {
		if (!Mix_PlayingMusic()) {
			Mix_PlayMusic(bgm, -1);
			Mix_VolumeMusic(musicVol);
		}
		Mix_Volume(-1, fxVol);

	}

	void prime() {
		enter();
		bool changeState = false;
		while (!changeState) {
			//check for cursor-button intersection
			if (SDL_HasIntersection(&cursorPos, &btnPos[0]) && !isBtn[1]) { isBtn[0] = true; }
			else { isBtn[0] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[1]) && !isBtn[0] && !isBtn[2]) { isBtn[1] = true; }
			else { isBtn[1] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[2]) && !isBtn[1]) { isBtn[2] = true; }
			else { isBtn[2] = false; }


			//play over sound
			if (isBtn[0] || isBtn[1] || isBtn[2]) {
				if (!isPlayed) {
					Mix_PlayChannel(-1, overSnd, 0);
					isPlayed = true;
				}
			}
			else {
				Mix_HaltChannel(-1);
				isPlayed = false;
			}


			//controller events
			while (SDL_PollEvent(e)) {
				switch (e->type) {
				case SDL_CONTROLLERBUTTONDOWN:
					switch (e->cbutton.button) {
					case SDL_CONTROLLER_BUTTON_A:
						for (int i = 0; i < 3; i++) {
							if (isBtn[i]) {
								changeState = true;
								Mix_PlayChannel(-1, clickSnd, 0);
								switch (i) {
								case 0:
									gameState = menu;
									break;
								case 1:
									gameState = instructions;
									break;
								case 2:
									gameState = levelOne;
									break;
								default:break;
								}
							}
						}
						break;
					case SDL_CONTROLLER_BUTTON_B:
						changeState = true;
						gameState = menu;
						break;
					default:break;
					}
					break;
				case SDL_CONTROLLERAXISMOTION:
					leftx = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
					lefty = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
					break;
				case SDL_QUIT:
					quit = true;
					break;
				default:break;
				}
				break;
			}
			update();
			SDL_RenderClear(ren);
			SDL_RenderCopy(ren, bg, NULL, &bgPos[0]);
			SDL_RenderCopyEx(ren, bg, NULL, &bgPos[1], 0, NULL, SDL_FLIP_NONE);
			SDL_RenderCopy(ren, title, NULL, &titlePos);
			SDL_RenderCopy(ren, txt, NULL, &txtPos);
			for (int i = 0; i < 3; i++) {
				if (!isBtn[i]) { SDL_RenderCopy(ren, btn[i], NULL, &btnPos[i]); }
				else { SDL_RenderCopy(ren, btnOver[i], NULL, &btnPos[i]); }
			}
			SDL_RenderCopy(ren, cursor, NULL, &cursorPos);
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
			SDL_RenderPresent(ren);
		}
	}

	inline void update() {
		deltaTime = (SDL_GetTicks() - currentTime) / 1000;
		currentTime = SDL_GetTicks();
		if (bgPos[0].x >= bgPos[0].w) { bgMove[0] -= bgPos[0].w * 2; }
		if (bgPos[1].x >= bgPos[1].w) { bgMove[1] -= bgPos[1].w * 2; }
		bgMove[0] += scrollSpeed*deltaTime;
		bgMove[1] += scrollSpeed*deltaTime;
		bgPos[0].x = bgMove[0];
		bgPos[1].x = bgMove[1];

		if (leftx > deadZone || leftx < -deadZone) { cx += leftx*deltaTime*cursorSpeed; }
		if (lefty > deadZone || lefty < -deadZone) { cy += lefty*deltaTime*cursorSpeed; }
		cursorPos.x = cx;
		cursorPos.y = cy;
	}
};

class Win :public GameState {
public:
	SDL_Rect bgPos[2], titlePos, txtPos, btnPos[2], cursorPos;
	SDL_Texture *bg, *title, *txt, *btn[2], *btnOver[2], *cursor;
	Mix_Music* bgm;
	Mix_Chunk *overSnd, *clickSnd;
	Sint16 leftx, lefty;
	float cursorSpeed, cx, cy, scrollSpeed, bgMove[2];
	bool isBtn[2], isPlayed;

	Win() {
		cursorSpeed = .05;
		scrollSpeed = 50;
		bgMove[0] = 0;
		bgMove[1] = -3200;

		bgm = Mix_LoadMUS("sounds/winBGM.mp3");
		overSnd = Mix_LoadWAV("sounds/over.wav");
		clickSnd = Mix_LoadWAV("sounds/click.wav");
		isPlayed = false;

		//load textures
		bg = IMG_LoadTexture(ren, "sprites/background.png");
		title = IMG_LoadTexture(ren, "sprites/winTitle.png");
		txt = IMG_LoadTexture(ren, "sprites/winTxt.png");
		btn[0] = IMG_LoadTexture(ren, "sprites/mainMenu.png");
		btn[1] = IMG_LoadTexture(ren, "sprites/playAgain.png");
		btnOver[0] = IMG_LoadTexture(ren, "sprites/mainMenuOver.png");
		btnOver[1] = IMG_LoadTexture(ren, "sprites/playAgainOver.png");
		cursor = IMG_LoadTexture(ren, "sprites/cursor.png");

		//set up rectangles
		SDL_QueryTexture(bg, NULL, NULL, &bgPos[0].w, &bgPos[0].h);
		bgPos[0].x = 0;
		bgPos[0].y = 0;

		SDL_QueryTexture(bg, NULL, NULL, &bgPos[1].w, &bgPos[1].h);
		bgPos[1].x = -3200;
		bgPos[1].y = 0;

		SDL_QueryTexture(title, NULL, NULL, &titlePos.w, &titlePos.h);
		titlePos.x = 0;
		titlePos.y = 20;

		SDL_QueryTexture(txt, NULL, NULL, &txtPos.w, &txtPos.h);
		txtPos.x = 100;
		txtPos.y = 100;

		for (int i = 0; i < 2; i++) {
			SDL_QueryTexture(btn[i], NULL, NULL, &btnPos[i].w, &btnPos[i].h);
			btnPos[i].x = 200 - (btnPos[i].w / 2);
		}
		btnPos[0].y = 185;
		btnPos[1].y = 257;
		btnPos[2].y = 329;

		SDL_QueryTexture(cursor, NULL, NULL, &cursorPos.w, &cursorPos.h);
		cursorPos.x = 200;
		cursorPos.y = 200;
	}

	~Win() {
		SDL_DestroyTexture(bg);
		SDL_DestroyTexture(title);
		SDL_DestroyTexture(txt);
		for (int i = 0; i < 2; i++) {
			SDL_DestroyTexture(btn[i]);
			SDL_DestroyTexture(btnOver[i]);
		}
		SDL_DestroyTexture(cursor);

	}

	void enter() {
		if (!Mix_PlayingMusic()) {
			Mix_PlayMusic(bgm, -1);
			Mix_VolumeMusic(musicVol);
		}
		Mix_Volume(-1, fxVol);

	}

	void prime() {
		enter();
		bool changeState = false;
		while (!changeState) {
			//check for cursor-button intersection
			if (SDL_HasIntersection(&cursorPos, &btnPos[0]) && !isBtn[1]) { isBtn[0] = true; }
			else { isBtn[0] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[1]) && !isBtn[0]) { isBtn[1] = true; }
			else { isBtn[1] = false; }


			//play over sound
			if (isBtn[0] || isBtn[1]) {
				if (!isPlayed) {
					Mix_PlayChannel(-1, overSnd, 0);
					isPlayed = true;
				}
			}
			else {
				Mix_HaltChannel(-1);
				isPlayed = false;
			}


			//controller events
			while (SDL_PollEvent(e)) {
				switch (e->type) {
				case SDL_CONTROLLERBUTTONDOWN:
					switch (e->cbutton.button) {
					case SDL_CONTROLLER_BUTTON_A:
						for (int i = 0; i < 2; i++) {
							if (isBtn[i]) {
								changeState = true;
								Mix_PlayChannel(-1, clickSnd, 0);
								switch (i) {
								case 0:
									gameState = menu;
									break;
								case 1:
									gameState = levelOne;
									break;
								default:break;
								}
							}
						}
						break;
					case SDL_CONTROLLER_BUTTON_B:
						changeState = true;
						gameState = menu;
						break;
					default:break;
					}
					break;
				case SDL_CONTROLLERAXISMOTION:
					leftx = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
					lefty = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
					break;
				case SDL_QUIT:
					quit = true;
					break;
				default:break;
				}
				break;
			}
			update();
			SDL_RenderClear(ren);
			SDL_RenderCopy(ren, bg, NULL, &bgPos[0]);
			SDL_RenderCopyEx(ren, bg, NULL, &bgPos[1], 0, NULL, SDL_FLIP_NONE);
			SDL_RenderCopy(ren, title, NULL, &titlePos);
			SDL_RenderCopy(ren, txt, NULL, &txtPos);
			for (int i = 0; i < 2; i++) {
				if (!isBtn[i]) { SDL_RenderCopy(ren, btn[i], NULL, &btnPos[i]); }
				else { SDL_RenderCopy(ren, btnOver[i], NULL, &btnPos[i]); }
			}
			SDL_RenderCopy(ren, cursor, NULL, &cursorPos);
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
			SDL_RenderPresent(ren);
		}
	}

	inline void update() {
		deltaTime = (SDL_GetTicks() - currentTime) / 1000;
		currentTime = SDL_GetTicks();
		if (bgPos[0].x >= bgPos[0].w) { bgMove[0] -= bgPos[0].w * 2; }
		if (bgPos[1].x >= bgPos[1].w) { bgMove[1] -= bgPos[1].w * 2; }
		bgMove[0] += scrollSpeed*deltaTime;
		bgMove[1] += scrollSpeed*deltaTime;
		bgPos[0].x = bgMove[0];
		bgPos[1].x = bgMove[1];

		if (leftx > deadZone || leftx < -deadZone) { cx += leftx*deltaTime*cursorSpeed; }
		if (lefty > deadZone || lefty < -deadZone) { cy += lefty*deltaTime*cursorSpeed; }
		cursorPos.x = cx;
		cursorPos.y = cy;
	}
};

class Lose :public GameState {
public:
	SDL_Rect bgPos[2], titlePos, txtPos, btnPos[2], cursorPos;
	SDL_Texture *bg, *title, *txt, *btn[2], *btnOver[2], *cursor;
	Mix_Music* bgm;
	Mix_Chunk *overSnd, *clickSnd;
	Sint16 leftx, lefty;
	float cursorSpeed, cx, cy, scrollSpeed, bgMove[2];
	bool isBtn[2], isPlayed;

	Lose() {
		cursorSpeed = .05;
		scrollSpeed = 50;
		bgMove[0] = 0;
		bgMove[1] = -3200;

		bgm = Mix_LoadMUS("sounds/loseBGM.mp3");
		overSnd = Mix_LoadWAV("sounds/over.wav");
		clickSnd = Mix_LoadWAV("sounds/click.wav");
		isPlayed = false;

		//load textures
		bg = IMG_LoadTexture(ren, "sprites/background.png");
		title = IMG_LoadTexture(ren, "sprites/loseTitle.png");
		txt = IMG_LoadTexture(ren, "sprites/loseTxt.png");
		btn[0] = IMG_LoadTexture(ren, "sprites/mainMenu.png");
		btn[1] = IMG_LoadTexture(ren, "sprites/playAgain.png");
		btnOver[0] = IMG_LoadTexture(ren, "sprites/mainMenuOver.png");
		btnOver[1] = IMG_LoadTexture(ren, "sprites/playAgainOver.png");
		cursor = IMG_LoadTexture(ren, "sprites/cursor.png");

		//set up rectangles
		SDL_QueryTexture(bg, NULL, NULL, &bgPos[0].w, &bgPos[0].h);
		bgPos[0].x = 0;
		bgPos[0].y = 0;

		SDL_QueryTexture(bg, NULL, NULL, &bgPos[1].w, &bgPos[1].h);
		bgPos[1].x = -3200;
		bgPos[1].y = 0;

		SDL_QueryTexture(title, NULL, NULL, &titlePos.w, &titlePos.h);
		titlePos.x = 0;
		titlePos.y = 20;

		SDL_QueryTexture(txt, NULL, NULL, &txtPos.w, &txtPos.h);
		txtPos.x = 100;
		txtPos.y = 100;

		for (int i = 0; i < 2; i++) {
			SDL_QueryTexture(btn[i], NULL, NULL, &btnPos[i].w, &btnPos[i].h);
			btnPos[i].x = 200 - (btnPos[i].w / 2);
		}
		btnPos[0].y = 185;
		btnPos[1].y = 257;
		btnPos[2].y = 329;

		SDL_QueryTexture(cursor, NULL, NULL, &cursorPos.w, &cursorPos.h);
		cursorPos.x = 200;
		cursorPos.y = 200;
	}

	~Lose() {
		SDL_DestroyTexture(bg);
		SDL_DestroyTexture(title);
		SDL_DestroyTexture(txt);
		for (int i = 0; i < 2; i++) {
			SDL_DestroyTexture(btn[i]);
			SDL_DestroyTexture(btnOver[i]);
		}
		SDL_DestroyTexture(cursor);

	}

	void enter() {
		if (!Mix_PlayingMusic()) {
			Mix_PlayMusic(bgm, -1);
			Mix_VolumeMusic(musicVol);
		}
		Mix_Volume(-1, fxVol);

	}

	void prime() {
		enter();
		bool changeState = false;
		while (!changeState) {
			//check for cursor-button intersection
			if (SDL_HasIntersection(&cursorPos, &btnPos[0]) && !isBtn[1]) { isBtn[0] = true; }
			else { isBtn[0] = false; }
			if (SDL_HasIntersection(&cursorPos, &btnPos[1]) && !isBtn[0]) { isBtn[1] = true; }
			else { isBtn[1] = false; }


			//play over sound
			if (isBtn[0] || isBtn[1]) {
				if (!isPlayed) {
					Mix_PlayChannel(-1, overSnd, 0);
					isPlayed = true;
				}
			}
			else {
				Mix_HaltChannel(-1);
				isPlayed = false;
			}


			//controller events
			while (SDL_PollEvent(e)) {
				switch (e->type) {
				case SDL_CONTROLLERBUTTONDOWN:
					switch (e->cbutton.button) {
					case SDL_CONTROLLER_BUTTON_A:
						for (int i = 0; i < 2; i++) {
							if (isBtn[i]) {
								changeState = true;
								Mix_PlayChannel(-1, clickSnd, 0);
								switch (i) {
								case 0:
									gameState = menu;
									break;
								case 1:
									gameState = levelOne;
									break;
								default:break;
								}
							}
						}
						break;
					case SDL_CONTROLLER_BUTTON_B:
						changeState = true;
						gameState = menu;
						break;
					default:break;
					}
					break;
				case SDL_CONTROLLERAXISMOTION:
					leftx = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
					lefty = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
					break;
				case SDL_QUIT:
					quit = true;
					break;
				default:break;
				}
				break;
			}
			update();
			SDL_RenderClear(ren);
			SDL_RenderCopy(ren, bg, NULL, &bgPos[0]);
			SDL_RenderCopyEx(ren, bg, NULL, &bgPos[1], 0, NULL, SDL_FLIP_NONE);
			SDL_RenderCopy(ren, title, NULL, &titlePos);
			SDL_RenderCopy(ren, txt, NULL, &txtPos);
			for (int i = 0; i < 2; i++) {
				if (!isBtn[i]) { SDL_RenderCopy(ren, btn[i], NULL, &btnPos[i]); }
				else { SDL_RenderCopy(ren, btnOver[i], NULL, &btnPos[i]); }
			}
			SDL_RenderCopy(ren, cursor, NULL, &cursorPos);
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
			SDL_RenderPresent(ren);
		}
	}

	inline void update() {
		deltaTime = (SDL_GetTicks() - currentTime) / 1000;
		currentTime = SDL_GetTicks();
		if (bgPos[0].x >= bgPos[0].w) { bgMove[0] -= bgPos[0].w * 2; }
		if (bgPos[1].x >= bgPos[1].w) { bgMove[1] -= bgPos[1].w * 2; }
		bgMove[0] += scrollSpeed*deltaTime;
		bgMove[1] += scrollSpeed*deltaTime;
		bgPos[0].x = bgMove[0];
		bgPos[1].x = bgMove[1];

		if (leftx > deadZone || leftx < -deadZone) { cx += leftx*deltaTime*cursorSpeed; }
		if (lefty > deadZone || lefty < -deadZone) { cy += lefty*deltaTime*cursorSpeed; }
		cursorPos.x = cx;
		cursorPos.y = cy;
	}
};

//class LevelOne :public GameState {
//public:
//	SDL_Rect bgPos, hudPos[5], barPos[4], gbarPos[4], gemPos[3];
//	SDL_Texture *bg, *barOutline, *gemHolder, *bar[7], *gem[3];
//
//	LevelOne() {
//
//		//load textures
//		bg = IMG_LoadTexture(ren, "sprites/leveloneBackground.png");
//		barOutline = IMG_LoadTexture(ren, "sprites/barOutline.png");
//		gemHolder = IMG_LoadTexture(ren, "sprites/gemHolder.png");
//		bar[0] = IMG_LoadTexture(ren, "sprites/barRedFill.png");
//		bar[1] = IMG_LoadTexture(ren, "sprites/barRedFill.png");
//		bar[2] = IMG_LoadTexture(ren, "sprites/barGreenFill.png");
//		bar[3] = IMG_LoadTexture(ren, "sprites/barBlueFill.png");
//		bar[4] = IMG_LoadTexture(ren, "sprites/barGrayFill.png");
//		bar[5] = IMG_LoadTexture(ren, "sprites/barHealthFill.png");
//		bar[6] = IMG_LoadTexture(ren, "sprites/barGrayHealthFill.png");
//		gem[0] = IMG_LoadTexture(ren, "sprites/gemRed.png");
//		gem[1] = IMG_LoadTexture(ren, "sprites/gemGreen.png");
//		gem[2] = IMG_LoadTexture(ren, "sprites/gemBlue.png");
//
//		//set up rectangles
//		SDL_QueryTexture(bg, NULL, NULL, &bgPos.w, &bgPos.h);
//		bgPos.x = 0;
//		bgPos.y = 0;
//
//		for (int i = 0; i < 4; i++) {
//			SDL_QueryTexture(barOutline, NULL, NULL, &hudPos[i].x, &hudPos[i].y);
//			hudPos[i].x = 15 * i;
//			hudPos[i].y = 5;
//			SDL_QueryTexture(bar[i], NULL, NULL, &barPos[i].x, &barPos[i].y);
//			barPos[i].x = 15 * i;
//			barPos[i].y = 5;
//
//		}
//
//	}
//};

class LevelOne :public GameState {
public:
	SDL_Rect bgPos, barPos[4], gbarPos[4], gemHolderPos, gemPos[3];
	SDL_Texture *bg, *barOutline, *bar[4], *gbar, *gemHolder, *gem[3];
	Mix_Music* bgm;
	Player *player;
	Enemy *enemy[3];

	LevelOne() {
		bgm = Mix_LoadMUS("sounds/levelOneBGM.mp3");

		bg = IMG_LoadTexture(ren, "sprites/leveloneBackground.png");
		barOutline = IMG_LoadTexture(ren, "sprites/barOutline.png");
		bar[0] = IMG_LoadTexture(ren, "sprites/barRedFill.png");
		bar[1] = IMG_LoadTexture(ren, "sprites/barGreenFill.png");
		bar[2] = IMG_LoadTexture(ren, "sprites/barBlueFill.png");
		bar[3] = IMG_LoadTexture(ren, "sprites/barHealthFill.png");
		gbar = IMG_LoadTexture(ren, "sprites/barGrayFill.png");
		gemHolder = IMG_LoadTexture(ren, "sprites/gemHolder.png");
		gem[0] = IMG_LoadTexture(ren, "sprites/gemRed.png");
		gem[1] = IMG_LoadTexture(ren, "sprites/gemGreen.png");
		gem[2] = IMG_LoadTexture(ren, "sprites/gemBlue.png");

		SDL_QueryTexture(bg, NULL, NULL, &bgPos.w, &bgPos.h);
		bgPos.x = 0;
		bgPos.y = 0;
		for (int i = 0; i < 4; i++) {
			SDL_QueryTexture(barOutline, NULL, NULL, &barPos[i].w, &barPos[i].h);
			barPos[i].x = 10;
			barPos[i].y = 10 + (barPos[i].h*i);
			gbarPos[i].w = 0;
			gbarPos[i].h = barPos[i].h;
			gbarPos[i].x = barPos[i].x;
			gbarPos[i].y = barPos[i].y;
		}
		SDL_QueryTexture(gemHolder, NULL, NULL, &gemHolderPos.w, &gemHolderPos.h);
		gemHolderPos.x = 790 - gemHolderPos.w;
		gemHolderPos.y = 10;
		for (int i = 0; i < 3; i++) {
			SDL_QueryTexture(gem[i], NULL, NULL, &gemPos[i].w, &gemPos[i].h);
			gemPos[i].y = gemHolderPos.y + 8;
		}
		gemPos[0].x = gemHolderPos.x + 9;
		gemPos[1].x = gemHolderPos.x + 50;
		gemPos[2].x = gemHolderPos.x + 93;

		player = new Player(ren, 200, 300, deadZone);
		enemy[0] = new Enemy(ren, "sprites/red.png", 500, 100);
		enemy[1] = new Enemy(ren, "sprites/green.png", 700, 300);
		enemy[2] = new Enemy(ren, "sprites/blue.png", 500, 500);


	}

	~LevelOne() {
		SDL_DestroyTexture(bg);
		SDL_DestroyTexture(barOutline);
		for (int i = 0; i < 4; i++) {
			SDL_DestroyTexture(bar[i]);
		}
		SDL_DestroyTexture(gbar);
		SDL_DestroyTexture(gemHolder);
		for (int i = 0; i < 3; i++) {
			SDL_DestroyTexture(gem[i]);
		}

	}

	void enter() {
		if (!Mix_PlayingMusic()) {
			Mix_PlayMusic(bgm, -1);
			Mix_VolumeMusic(musicVol);
		}
		Mix_Volume(-1, fxVol);

	}

	void prime() {
		enter();
		bool changeState = false;
		while (!changeState) {
			while (SDL_PollEvent(e)) {
				switch (e->type) {
				case SDL_CONTROLLERBUTTONDOWN:
					switch (e->cbutton.button) {
					case SDL_CONTROLLER_BUTTON_A:
						player->createBullet();
						break;
					case SDL_CONTROLLER_BUTTON_B:
						changeState = true;
						gameState = menu;
						break;
					case SDL_CONTROLLER_BUTTON_Y:
						changeState = true;
						gameState = levelTwo;
						break;
					default:break;
					}
					break;
				case SDL_CONTROLLERAXISMOTION:
					player->xDir = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
					player->yDir = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
					break;
				default:break;
				}
			}
			update();
			SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
			SDL_RenderClear(ren);
			SDL_RenderCopy(ren, bg, NULL, &bgPos);
			for (int i = 0; i < 4; i++) {
				SDL_RenderCopy(ren, bar[i], NULL, &barPos[i]);
				SDL_RenderCopy(ren, gbar, NULL, &gbarPos[i]);
				SDL_RenderCopy(ren, barOutline, NULL, &barPos[i]);
			}
			SDL_RenderCopy(ren, gemHolder, NULL, &gemHolderPos);
			for (int i = 0; i < 3; i++) {
				SDL_RenderCopy(ren, gem[i], NULL, &gemPos[i]);
			}

			player->draw(ren);
			for (int i = 0; i < 3; i++) {
				if (enemy[i]->health > 0)
					enemy[i]->draw(ren);
			}
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
			SDL_RenderPresent(ren);
		}
	}

	inline void update() {
		deltaTime = (SDL_GetTicks() - currentTime) / 1000;
		currentTime = SDL_GetTicks();

		player->update(deltaTime);
		for (int i = 0; i < 3; i++) {
			enemy[i]->update(deltaTime, player->pos);
		}

		for (int i = 0; i < player->bulletList.size(); i++) {
			if (player->bulletList[i].active) {
				for (int j = 0; j < 3; j++) {
					if (SDL_HasIntersection(&player->bulletList[i].pos, &enemy[j]->pos)) {
						//make explosion
						enemy[j]->health--;
						//if (enemy[j]->health <= 0)
						//makeExplosion(enemyList[j]->pos.x, enemyList[j]->pos.y);
						enemy[j]->active = false;
						player->bulletList[i].reset();
						player->score += 1;
					}
				}
			}
		}
	}
};

class LevelTwo :public GameState {
public:
	SDL_Rect bgPos, barPos[2], gbarPos[2], gemHolderPos, gemPos[4];
	SDL_Texture *bg, *barOutline, *bar[2], *gbar, *gemHolder, *gem[4];
	Mix_Music* bgm;
	Player *player;
	Turret *turret[3];

	LevelTwo() {
		bgm = Mix_LoadMUS("sounds/levelTwoBGM.mp3");

		bg = IMG_LoadTexture(ren, "sprites/levelTwoBackground.png");
		barOutline = IMG_LoadTexture(ren, "sprites/barOutline.png");
		bar[0] = IMG_LoadTexture(ren, "sprites/barWhiteFill.png");
		bar[1] = IMG_LoadTexture(ren, "sprites/barHealthFill.png");
		gbar = IMG_LoadTexture(ren, "sprites/barGrayFill.png");
		gemHolder = IMG_LoadTexture(ren, "sprites/gemHolder.png");
		gem[0] = IMG_LoadTexture(ren, "sprites/gemRed.png");
		gem[1] = IMG_LoadTexture(ren, "sprites/gemGreen.png");
		gem[2] = IMG_LoadTexture(ren, "sprites/gemBlue.png");
		gem[3] = IMG_LoadTexture(ren, "sprites/gemGray.png");

		SDL_QueryTexture(bg, NULL, NULL, &bgPos.w, &bgPos.h);
		bgPos.x = 0;
		bgPos.y = 0;
		for (int i = 0; i < 2; i++) {
			SDL_QueryTexture(barOutline, NULL, NULL, &barPos[i].w, &barPos[i].h);
			barPos[i].x = 10;
			barPos[i].y = 10 + (barPos[i].h*i);
			gbarPos[i].w = 0;
			gbarPos[i].h = barPos[i].h;
			gbarPos[i].x = barPos[i].x;
			gbarPos[i].y = barPos[i].y;
		}
		SDL_QueryTexture(gemHolder, NULL, NULL, &gemHolderPos.w, &gemHolderPos.h);
		gemHolderPos.x = 790 - gemHolderPos.w;
		gemHolderPos.y = 10;
		for (int i = 0; i < 4; i++) {
			SDL_QueryTexture(gem[i], NULL, NULL, &gemPos[i].w, &gemPos[i].h);
			gemPos[i].y = gemHolderPos.y + 8;
		}
		gemPos[0].x = gemHolderPos.x + 9;
		gemPos[1].x = gemHolderPos.x + 50;
		gemPos[2].x = gemHolderPos.x + 93;
		gemPos[3].x = gemHolderPos.x + 134;

		player = new Player(ren, 200, 300, deadZone);
		turret[0] = new Turret(ren, 500, 100);
		turret[1] = new Turret(ren, 700, 300);
		turret[2] = new Turret(ren, 500, 500);
	}

	~LevelTwo() {
		SDL_DestroyTexture(bg);
		SDL_DestroyTexture(barOutline);
		for (int i = 0; i < 2; i++) {
			SDL_DestroyTexture(bar[i]);
		}
		SDL_DestroyTexture(gbar);
		SDL_DestroyTexture(gemHolder);
		for (int i = 0; i < 4; i++) {
			SDL_DestroyTexture(gem[i]);
		}

	}

	void enter() {
		if (!Mix_PlayingMusic()) {
			Mix_PlayMusic(bgm, -1);
			Mix_VolumeMusic(musicVol);
		}
		Mix_Volume(-1, fxVol);

	}

	void prime() {
		enter();
		bool changeState = false;
		while (!changeState) {
			while (SDL_PollEvent(e)) {
				switch (e->type) {
				case SDL_CONTROLLERBUTTONDOWN:
					switch (e->cbutton.button) {
					case SDL_CONTROLLER_BUTTON_A:
						player->createBullet();
						break;
					case SDL_CONTROLLER_BUTTON_B:
						changeState = true;
						gameState = menu;
						break;
					case SDL_CONTROLLER_BUTTON_Y:
						changeState = true;
						gameState = levelTwo;
						break;
					default:break;
					}
					break;
				case SDL_CONTROLLERAXISMOTION:
					player->xDir = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
					player->yDir = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
					break;
				default:break;
				}
			}
			update();
			SDL_RenderClear(ren);
			SDL_RenderCopy(ren, bg, NULL, &bgPos);
			for (int i = 0; i < 2; i++) {
				SDL_RenderCopy(ren, bar[i], NULL, &barPos[i]);
				SDL_RenderCopy(ren, gbar, NULL, &barPos[i]);
				SDL_RenderCopy(ren, barOutline, NULL, &barPos[i]);
			}
			SDL_RenderCopy(ren, gemHolder, NULL, &gemHolderPos);
			for (int i = 0; i < 4; i++) {
				SDL_RenderCopy(ren, gem[i], NULL, &gemPos[i]);
			}
			player->draw(ren);
			for (int i = 0; i < 3; i++) {
				turret[i]->draw(ren);
			}
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
			SDL_RenderPresent(ren);
		}
	}

	inline void update() {
		deltaTime = (SDL_GetTicks() - currentTime) / 1000;
		currentTime = SDL_GetTicks();

		player->update(deltaTime);
		for (int i = 0; i < 3; i++) {
			turret[i]->update(deltaTime, player->pos);
		}

		for (int i = 0; i < player->bulletList.size(); i++) {
			if (player->bulletList[i].active) {
				for (int j = 0; j < 3; j++) {
					if (SDL_HasIntersection(&player->bulletList[i].pos, &turret[j]->basePos)) {
						//make explosion
						//makeExplosion(enemyList[j]->pos.x, enemyList[j]->pos.y);
						player->bulletList[i].reset();
						player->score += 1;
					}
				}
			}
		}

	}
};



int main(int argc, char** args) {
	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("Achromatic", 50, 50, 800, 600, SDL_WINDOW_RESIZABLE);
	ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	gc = SDL_GameControllerOpen(0);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	menu = new Menu();
	options = new Options();
	instructions = new Instructions();
	backStory = new Backstory();
	levelOne = new LevelOne();
	levelTwo = new LevelTwo();
	win = new Win();
	lose = new Lose();

	gameState = menu;

	while (!quit) {
		gameState->enter();
		gameState->prime();
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_ESCAPE]) { quit = true; }
	}

	SDL_Quit();
	return 0;
}