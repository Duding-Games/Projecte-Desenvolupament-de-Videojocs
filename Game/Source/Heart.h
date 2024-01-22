#ifndef __HEART_H__
#define __HEART_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Heart : public Entity
{
public:

	Heart();
	virtual ~Heart();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

public:

	bool isPicked = false;
	Animation* currentAnim;

private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;

	Animation heartAnim;
};

#endif // __HEART_H__