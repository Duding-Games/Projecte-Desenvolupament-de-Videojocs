#ifndef __ENEMYBAT_H__
#define __ENEMYBAT_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "PerfTimer.h"
#include "Timer.h"

#include "Physics.h"

struct SDL_Texture;

class EnemyBat : public Entity
{
public:

	EnemyBat();

	virtual ~EnemyBat();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	bool isOutOfBounds(int x, int y);

public:
	float speed;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;

	bool isFacingLeft;

	Animation* currentAnim;

private:
	Animation idleAnim;

	b2Transform initialPos;
};

#endif // __ENEMY_H__