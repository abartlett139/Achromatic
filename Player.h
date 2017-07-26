#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <SDL_mixer.h>
#include <vector>
#include <math.h>
#include "Bullet.h"

#define toDeg *(180/3.14159)
#define toRad *(3.14159/180)

class Player
{
public:
	bool active;

	std::vector<Bullet>bulletList;

	SDL_Texture* texture;
	SDL_Rect pos, drawRect;
	Sint16 xDir, yDir, xDirOld, yDirOld;
	int frameWidth, frameHeight, texW, texH, deadZone, red, green, blue, health, score;
	float spriteSpeed, frameCounter, speed, x, y, px, py, angle, oldAngle, targetAngle;
	Mix_Chunk *fire, *hit;

	Player();
	Player(SDL_Renderer *ren, float x, float y, int dZ);
	~Player();

	void update(float deltaTime);
	void draw(SDL_Renderer *ren);
	void createBullet();
};

