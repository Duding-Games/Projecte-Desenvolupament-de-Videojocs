#ifndef __ENEMYBAT_H__
#define __ENEMYBAT_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "DynArray.h"


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

	bool Bathfinding( float dt);

public:
	float speed;
	b2Vec2 vel;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;

	int killFxId;

	bool isFacingLeft;
	bool isDead = false;
	bool isDying;

	Animation* currentAnim;
	DynArray<iPoint> lastPath;

private:
	Animation idleAnim;
	Animation dieAnim;
	Animation deadAnim;
	Animation attackAnim;

	b2Transform initialPos;
};

#endif // __ENEMY_H__
