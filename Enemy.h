#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <SDL_mixer.h>
#include <vector>
#include <math.h>


#define toDeg *(180/3.14159)
#define toRad *(3.14159/180)

class Enemy
{
public:
	bool active;
	//std::vector<turretBullet*> bulletList;
	SDL_Texture *texture;
	SDL_Rect pos, drawRect;
	int frameWidth, frameHeight, texW, texH, health;
	float spriteSpeed, frameCounter, speed, angle, x, y, distance, fireTime, fireDelay;
	SDL_Point enemy, player, center;
	Mix_Chunk *fire, *hit;

	Enemy(SDL_Renderer *ren, char *fileName, float x, float y);
	~Enemy();

	void update(float deltaTime, SDL_Rect player);
	void draw(SDL_Renderer *ren);
	void createBullet(SDL_Rect target);

};

