#include <SDL.h>
#include <SDL_image.h>


class Bullet
{
public:
	bool active;
	SDL_Texture *texture;
	SDL_Rect pos;
	float xDir, yDir, speed, angle, x, y;


	Bullet();
	Bullet(SDL_Renderer *ren, char *file, float x, float y, Sint16 xDir, Sint16 yDir);
	~Bullet();

	void update(float deltaTime);
	void draw(SDL_Renderer *ren);
	void reset();
};

