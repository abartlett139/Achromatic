#include "Player.h"



Player::Player()
{
}


Player::Player(SDL_Renderer *ren, float x, float y, int dZ)
{
	//basic stuff
	active = true;
	this->x = x;
	this->y = y;
	speed = .005f;
	angle = 0;
	oldAngle = 0;
	targetAngle = 0;
	deadZone = dZ;


	spriteSpeed = .005;
	frameCounter = 0;
	frameWidth = 40;
	frameHeight = 40;


	texture = IMG_LoadTexture(ren, "sprites/player.png");
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

	//load sounds
	fire = Mix_LoadWAV("sounds/fire.wav");
	hit = Mix_LoadWAV("sounds/hit.wav");

	//bullet list
	for (int i = 0; i < 20; i++) {
		Bullet temp(ren, "sprites/vial.png", -1000, -1000, 0, 0);
		bulletList.push_back(temp);
	}
}


Player::~Player()
{
	SDL_DestroyTexture(texture);
}



void Player::update(float deltaTime) {
	//update player position
	if (xDir > deadZone || xDir < -deadZone) {
		x += speed *xDir *deltaTime;
	}
	if (yDir > deadZone || yDir < -deadZone) {
		y += speed*yDir*deltaTime;
	}

	if (x < 0) { x = 0; }
	if (x > 800 - pos.w) { x = 800 - pos.w; }
	if (y < 0) { y = 0; }
	if (y > 600 - pos.h) { y = 600 - pos.h; }

	pos.x = x;
	pos.y = y;

	if (xDir > deadZone || xDir < -deadZone || yDir > deadZone || yDir < -deadZone) {
		if (xDir != 0 || yDir != 0) {
			angle = atan2(yDir, xDir)toDeg;
			oldAngle = angle;
			xDirOld = xDir;
			yDirOld = yDir;
		}
	}

	if (active) {
		frameCounter += deltaTime * spriteSpeed;
		if (frameCounter >= spriteSpeed) {
			frameCounter = 0;
			drawRect.x += frameWidth;

			if (drawRect.x >= texW) {
				drawRect.x = 0;
			}
		}
	}


	//update  bullets
	for (int i = 0; i < bulletList.size(); i++) {
		if (bulletList[i].active) {
			bulletList[i].update(deltaTime);
		}
	}
}


void Player::draw(SDL_Renderer *ren) {
	SDL_RenderCopyEx(ren, texture, &drawRect, &pos, angle, NULL, SDL_FLIP_NONE);

	for (int i = 0; i < bulletList.size(); i++) {
		if (bulletList[i].active) {
			bulletList[i].draw(ren);
		}
	}

}


void Player::createBullet() {
	for (int i = 0; i < bulletList.size(); i++) {
		if (!bulletList[i].active) {
			bulletList[i].active = true;
			bulletList[i].x = x + pos.w / 2;
			bulletList[i].y = y + pos.h / 2;
			bulletList[i].xDir = cos((angle)toRad);
			bulletList[i].yDir = sin((angle)toRad);
			break;
		}
	}
}