#ifndef __ENEMYSLIME_H__
#define __ENEMYSLIME_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "DynArray.h"


#include "Physics.h"

struct SDL_Texture;

class EnemySlime : public Entity
{
public:

	EnemySlime();

	virtual ~EnemySlime();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	bool isOutOfBounds(int x, int y);

	bool Slimefinding( float dt);

public:
	float speed;
	b2Vec2 vel;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;

	bool isFacingLeft;
	bool isDead = false;
	bool isAttacking = false;

	Animation* currentAnim;
	DynArray<iPoint> lastPath;

private:
	Animation idleAnim;
	Animation dieAnim;
	Animation attackAnim;

	b2Transform initialPos;
};

#endif // __ENEMY_H__
