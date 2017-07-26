#include "Enemy.h"



Enemy::Enemy(SDL_Renderer *ren, char* filename, float x, float y)
{

	active = true;
	angle = 0;

	spriteSpeed = .005;
	frameCounter = 0;

	frameWidth = 50;
	frameHeight = 50;

	health = 5;

	texture = IMG_LoadTexture(ren, filename);
	if (!texture)printf(SDL_GetError());
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

	pos.x = x;
	pos.y = y;
	pos.w = frameWidth;
	pos.h = frameHeight;

	drawRect.x = 0;
	drawRect.y = 0;
	drawRect.w = frameWidth;
	drawRect.h = frameHeight;

	center.x = 20;
	center.y = 25;

	/*turretBullet *temp;

	for (int i = 0; i < 10; i++) {
	temp = new turretBullet(ren, 500, 500);
	bulletList.push_back(temp);
	}*/

}


Enemy::~Enemy()
{
	SDL_DestroyTexture(texture);
}

void Enemy::draw(SDL_Renderer *ren) {
	/*for (int i = 0; i < bulletList.size(); i++) {
	if (bulletList[i]->active)
	bulletList[i]->draw(ren);
	}*/

	SDL_RenderCopyEx(ren, texture, &drawRect, &pos, angle, &center, SDL_FLIP_NONE);
}

void Enemy::update(float deltaTime, SDL_Rect playerPos) {

	x = (playerPos.x + playerPos.w / 2) - (pos.x + pos.w / 2);
	y = (playerPos.y + playerPos.h / 2) - (pos.y + pos.h / 2);
	angle = atan2(y, x) * 180 / 3.14159;


	if (SDL_GetTicks() > fireTime) {
		//createBullet(playerPos);
		fireTime = SDL_GetTicks() + (rand() % 3 + 1) * 1000;
	}

	if (active) {
		frameCounter = health;
		//frameCounter += deltaTime*spriteSpeed;
		/*if (frameCounter >= spriteSpeed) {

		drawRect.x += frameWidth;

		if (drawRect.x >= texW)
		drawRect.x = 0;
		}*/
	}

	/*for (int i = 0; i < bulletList.size(); i++) {
	if (bulletList[i]->active)
	bulletList[i]->update(deltaTime);
	}*/
}

void Enemy::createBullet(SDL_Rect target) {
	/*for (int i = 0; i < bulletList.size(); i++) {
	if (!bulletList[i]->active)
	bulletList[i]->start(target, pos);
	bulletList[i]->active = true;
	Mix_PlayChannel(-1, fire, 0);
	bulletList[i]->pos.x = (pos.x + pos.w / 2) - (bulletList[i]->pos.w / 2);
	bulletList[i]->pos.y = (pos.y + pos.h / 2) - (bulletList[i]->pos.h / 2);
	bulletList[i]->x = bulletList[i]->pos.x;
	bulletList[i]->y = bulletList[i]->pos.y;
	break;
	}*/
}