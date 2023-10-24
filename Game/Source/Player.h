#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:
	float speed = 0.2f;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	int pickCoinFxId;
	bool isJumping = false;
	bool isFacingLeft = false;
	float initialY; // Store the initial position when the jump starts

	Animation* currentAnim;

private:
	Animation idleAnim;
	Animation runAnim;
	Animation jumpAnim;
};

#endif // __PLAYER_H__