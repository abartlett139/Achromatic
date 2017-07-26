#include "Bullet.h"


Bullet::Bullet()
{

}


Bullet::Bullet(SDL_Renderer *ren, char *file, float x, float y, Sint16 xDir, Sint16 yDir)
{
	active = false;
	this->x = xDir;
	this->y = yDir;
	speed = 200;

	texture = IMG_LoadTexture(ren, file);

	SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
	pos.x = x - pos.w / 2;
	pos.y = y - pos.h / 2;
}


Bullet::~Bullet()
{
}

void Bullet::update(float deltaTime) {

	x += speed*deltaTime*xDir;
	y += speed*deltaTime*yDir;

	pos.x = x;
	pos.y = y;

	if (x > 800 + pos.w || x < -pos.w || y > 600 + pos.h || y < -pos.h) {
		reset();
	}
}


void Bullet::draw(SDL_Renderer *ren) {
	SDL_RenderCopy(ren, texture, NULL, &pos);
}

void Bullet::reset() {
	pos.x = -1000;
	active = false;
}
