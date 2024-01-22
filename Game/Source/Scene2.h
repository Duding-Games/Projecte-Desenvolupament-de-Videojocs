#ifndef __SCENE2_H__
#define __SCENE2_H__

#include "Module.h"
#include "Player.h"
#include "EnemyBat.h"
#include "EnemySlime.h"
#include "FinalBoss.h"
#include "Item.h"

struct SDL_Texture;

class Scene2 : public Module
{
public:

	Scene2(App* app, bool start_enabled);

	// Destructor
	virtual ~Scene2();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//
	bool LoadState(pugi::xml_node node);

	//
	bool SaveState(pugi::xml_node node);

	Player* GetPLayer();

	SString texturePath;
	const char* texture;
	SDL_Texture* tex;

private:
	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;
	Player* player;
	EnemyBat* enemyBat;
	EnemySlime* enemySlime;
	FinalBoss* finalBoss;
	PhysBody* winSensor;
	
};

#endif // __SCENE2_H__