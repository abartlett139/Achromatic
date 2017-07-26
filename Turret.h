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

class Turret
{
public:
	bool active;
	//std::vector<turretBullet*> bulletList;
	SDL_Texture *baseTexture, *barrelTexture;
	SDL_Rect basePos, barrelPos, baseDrawRect, barrelDrawRect;
	int baseframeWidth, baseframeHeight, basetexW, basetexH, barrelframeWidth, barrelframeHeight, barreltexW, barreltexH, h;
	float speed, spriteSpeed, frameCounter, angle, x, y, distance, fireTime, fireDelay;
	SDL_Point turret, player, center;
	Mix_Chunk *fire;

	Turret(SDL_Renderer *ren, float x, float y);
	~Turret();

	void update(float deltaTime, SDL_Rect player);
	void draw(SDL_Renderer *ren);
	void createBullet(SDL_Rect target);

};

