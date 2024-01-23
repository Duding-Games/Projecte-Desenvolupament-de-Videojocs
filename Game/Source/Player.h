#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "PerfTimer.h"
#include "Timer.h"

#include "Physics.h"

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

	bool isOutOfBounds(int x, int y);

public:
	Timer timerDash;
	float speed;
	float speedDash;
	float jumpForce;
	const char* texturePath;
	const char* texturePath2;
	SDL_Texture* texture = NULL;
	SDL_Texture* textureHeart = NULL;
	PhysBody* pbody;
	PhysBody* sensor;
	int pickCoinFxId;
	int jumpFxId;
	int dashFxId;
	int swordFxId;
	int runFxId;
	int winFxId;
	int offsetTexY = 6;
	int lives = 3;
	bool isJumping = false;
	bool isFacingLeft = false;
	bool isDashing = false;
	bool isAttacking = false;
	bool isCrouching = false;
	bool isDying = false;
	iPoint heartPos;

	Animation* currentAnim;

private:
	Animation idleAnim;
	Animation runAnim;
	Animation jumpAnim;
	Animation dashAnim;
	Animation crouchAnim;
	Animation attackAnim;
	Animation dieAnim;

	b2Transform initialPos;
};

#endif // __PLAYER_H__