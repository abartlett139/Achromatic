#include "Turret.h"



Turret::Turret(SDL_Renderer *ren, float x, float y)
{

	active = true;
	angle = 0;

	spriteSpeed = .005;
	frameCounter = 0;
	baseframeWidth = 50;
	baseframeHeight = 50;
	barrelframeWidth = 50;
	barrelframeHeight = 20;

	baseTexture = IMG_LoadTexture(ren, "sprites/turretSprite.png");
	if (!baseTexture)printf(SDL_GetError());
	SDL_QueryTexture(baseTexture, NULL, NULL, &basetexW, &basetexH);
	barrelTexture = IMG_LoadTexture(ren, "sprites/turretBarrellSprite.png");
	if (!barrelTexture)printf(SDL_GetError());
	SDL_QueryTexture(baseTexture, NULL, NULL, &barreltexW, &barreltexH);

	basePos.x = x;
	basePos.y = y;
	basePos.w = baseframeWidth;
	basePos.h = baseframeHeight;

	barrelPos.x = x + 28 / 2;
	barrelPos.y = y + 28 / 2;
	barrelPos.w = barrelframeWidth;
	barrelPos.h = barrelframeHeight;

	baseDrawRect.x = 0;
	baseDrawRect.y = 0;
	baseDrawRect.w = baseframeWidth;
	baseDrawRect.h = baseframeHeight;

	barrelDrawRect.x = 0;
	barrelDrawRect.y = 0;
	barrelDrawRect.w = barrelframeWidth;
	barrelDrawRect.h = barrelframeHeight;

	center.x = 10;
	center.y = 10;

	/*turretBullet *temp;

	for (int i = 0; i < 10; i++) {
	temp = new turretBullet(ren, 500, 500);
	bulletList.push_back(temp);
	}*/

}


Turret::~Turret()
{
	SDL_DestroyTexture(baseTexture);
	SDL_DestroyTexture(barrelTexture);
}

void Turret::draw(SDL_Renderer *ren) {
	/*for (int i = 0; i < bulletList.size(); i++) {
	if (bulletList[i]->active)
	bulletList[i]->draw(ren);
	}*/

	SDL_RenderCopyEx(ren, baseTexture, &baseDrawRect, &basePos, NULL, NULL, SDL_FLIP_NONE);
	SDL_RenderCopyEx(ren, barrelTexture, &barrelDrawRect, &barrelPos, angle, &center, SDL_FLIP_NONE);
}

void Turret::update(float deltaTime, SDL_Rect playerPos) {

	x = (playerPos.x + playerPos.w / 2) - (basePos.x + basePos.w / 2);
	y = (playerPos.y + playerPos.h / 2) - (basePos.y + basePos.h / 2);
	angle = atan2(y, x) * 180 / 3.14159;


	if (SDL_GetTicks() > fireTime) {
		//createBullet(playerPos);
		fireTime = SDL_GetTicks() + (rand() % 3 + 1) * 1000;
	}

	if (active) {
		frameCounter += deltaTime*spriteSpeed;
		if (frameCounter >= spriteSpeed) {
			frameCounter = 0;
			baseDrawRect.x += baseframeWidth;
			barrelDrawRect.x += barrelframeWidth;

			if (baseDrawRect.x >= basetexW)
				baseDrawRect.x = 0;
			if (barrelDrawRect.x >= barreltexW)
				barrelDrawRect.x = 0;
		}
	}

	/*for (int i = 0; i < bulletList.size(); i++) {
	if (bulletList[i]->active)
	bulletList[i]->update(deltaTime);
	}*/
}

void Turret::createBullet(SDL_Rect target) {
	/*for (int i = 0; i < bulletList.size(); i++) {
	if (!bulletList[i]->active)
	bulletList[i]->start(target, basePos);
	bulletList[i]->active = true;
	Mix_PlayChannel(-1, fire, 0);
	bulletList[i]->pos.x = (basePos.x + basePos.w / 2) - (bulletList[i]->pos.w / 2);
	bulletList[i]->pos.y = (basePos.y + basePos.h / 2) - (bulletList[i]->pos.h / 2);
	bulletList[i]->x = bulletList[i]->pos.x;
	bulletList[i]->y = bulletList[i]->pos.y;
	break;
	}*/
}