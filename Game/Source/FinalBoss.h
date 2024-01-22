#ifndef __FINALBOSS_H__
#define __FINALBOSS_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "DynArray.h"


#include "Physics.h"

struct SDL_Texture;

class FinalBoss : public Entity
{
public:

	FinalBoss();

	virtual ~FinalBoss();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	bool isOutOfBounds(int x, int y);

	bool Bossfinding( float dt);

public:
	float speed;
	b2Vec2 vel;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	/*PhysBody* pbody;*/

	int killFxId;

	bool isFacingLeft;
	bool isDead = false;

	Animation* currentAnim;
	DynArray<iPoint> lastPath;
	

private:
	Animation spawnAnim;
	Animation idleAnim;
	Animation dieAnim;
	Animation attackAnim;

	
};

#endif // __ENEMY_H__
