#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "List.h"

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Additional methods
	Entity* CreateEntity(EntityType type, bool doStart = false);

	void DestroyEntity(Entity* entity);

	void AddEntity(Entity* entity);

	void DestroyDeadEnemies();

public:

	List<Entity*> entities;
	List<Entity*> enemies;
	List<iPoint> deadEnemies;



};

#endif // __ENTITYMANAGER_H__
