#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene2.h"
#include "Scene.h"
#include "ModuleFadeToBlack.h"
#include "Map.h"
#include "Player.h"
#include "EnemyBat.h"
#include "EnemySlime.h"
#include "FinalBoss.h"
#include "EntityManager.h"

#include "Defs.h"
#include "Log.h"

Scene2::Scene2(App* app, bool start_enabled) : Module(app, start_enabled)
{
	name.Create("scene");
}

// Destructor
Scene2::~Scene2()
{}

// Called before render is available
bool Scene2::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("lvl2").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	if (config.child("lvl2").child("player")) {
		player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
		player->parameters = config.child("lvl2").child("player");
	}

	for (pugi::xml_node itemNode = config.child("lvl2").child("enemyBat"); itemNode; itemNode = itemNode.next_sibling("enemyBat"))
	{
		EnemyBat* enemyBat = (EnemyBat*)app->entityManager->CreateEntity(EntityType::ENEMYBAT);
		enemyBat->parameters = itemNode;
	}

	for (pugi::xml_node itemNode = config.child("lvl2").child("enemySlime"); itemNode; itemNode = itemNode.next_sibling("enemySlime"))
	{
		EnemySlime* enemySlime = (EnemySlime*)app->entityManager->CreateEntity(EntityType::ENEMYSLIME);
		enemySlime->parameters = itemNode;
	}

	for (pugi::xml_node itemNode = config.child("lvl2").child("finalBoss"); itemNode; itemNode = itemNode.next_sibling("finalBoss"))
	{
		FinalBoss* finalBoss = (FinalBoss*)app->entityManager->CreateEntity(EntityType::FINALBOSS);
		finalBoss->parameters = itemNode;
	}

	if (config.child("map")) {
		//Get the map name from the config file and assigns the value in the module
		app->map->name = config.child("map").attribute("name").as_string();
		app->map->path = config.child("map").attribute("path").as_string();
	}

	if (config.child("map").child("mouseTileTex")) {
		texturePath = config.child("map").child("mouseTileTex").attribute("texturepath").as_string();
	}

	texture = config.child("e_tutorial").attribute("texturepath").as_string();

	return ret;
}
// Called before the first frame
bool Scene2::Start()
{
	//Music is commented so that you can add your own music
	app->audio->PlayMusic("Assets/Audio/Music/background_music.ogg");

	tex = app->tex->Load(texture);

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	winSensor = app->physics->CreateRectangleSensor(3405, 345, 69, 92, bodyType::STATIC);
	winSensor->ctype = ColliderType::WIN;
	

	return true;
}

// Called each loop iteration
bool Scene2::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene2::Update(float dt)
{
	float camSpeed = 1; 

	if(app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && app->godmode)
		app->render->camera.y += (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && app->godmode)
		app->render->camera.y -= (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && app->godmode)
		app->render->camera.x += (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && app->godmode)
		app->render->camera.x -= (int)ceil(camSpeed * dt);
	
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		app->fadeToBlack->FadeToBlack(this, app->scene);
	}
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		app->fadeToBlack->FadeToBlack(this, this);
	}
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		app->fadeToBlack->FadeToBlack(this, this);
	}
	// Render tutorial image
	app->render->DrawTexture(tex, 1104, 207, SDL_FLIP_NONE);

	//Request App to Load / Save when pressing the keys F5(save) / F6(load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) app->SaveRequest();
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) app->LoadRequest();

	return true;
}

// Called each loop iteration
bool Scene2::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene2::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

Player* Scene2::GetPLayer()
{
	return player;
}

//Implement a method to load the state
// for now load camera's x and y
bool Scene2::LoadState(pugi::xml_node node) {

	int x = node.child("player").attribute("x").as_int();
	int y = node.child("player").attribute("y").as_int();

	player->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y)), 0);


	//ELIMINAR TODOS LOS ENEMIGOS
	ListItem<Entity*>* item;

	/*for (item = app->entityManager->enemies.start; item != NULL; item = item->next)
	{
		item->data->active = false;
		

		
		
	}
	app->entityManager->enemies.Clear()*/;

	while (app->entityManager->enemies.Count() > 0) {

		Entity* enemyToDelete = app->entityManager->enemies.At(app->entityManager->enemies.Count() - 1)->data;

		//enemyToDelete->pbody->body->SetActive(false);
		app->entityManager->DestroyEntity(enemyToDelete);
		app->entityManager->enemies.Del(app->entityManager->enemies.At(app->entityManager->enemies.Count() - 1));
	}
	app->entityManager->enemies.Clear();
	
	

	//Spawnear todos los enemigos
	pugi::xml_document configFile;
	pugi::xml_parse_result file = configFile.load_file("config.xml");
	
	for (pugi::xml_node itemNode = configFile.child("config").child("scene").child("enemyBat"); itemNode; itemNode = itemNode.next_sibling("enemyBat"))
	{
		EnemyBat* enemyBat = (EnemyBat*)app->entityManager->CreateEntity(EntityType::ENEMYBAT);
		enemyBat->parameters = itemNode;
		enemyBat->Awake();
		enemyBat->Start();
		
	}
	for (pugi::xml_node itemNode = configFile.child("config").child("scene").child("enemySlime"); itemNode; itemNode = itemNode.next_sibling("enemySlime"))
	{
		EnemySlime* enemySlime = (EnemySlime*)app->entityManager->CreateEntity(EntityType::ENEMYSLIME);
		enemySlime->parameters = itemNode;
		enemySlime->Awake();
		enemySlime->Start();
	}

	//Lo de abajo para eliminar los que estaban muertos
	for (pugi::xml_node itemNode = node.child("enemies").child("enemy"); itemNode; itemNode = itemNode.next_sibling("enemy"))
	{
		if (!itemNode.attribute("alive").as_bool()) {
			app->entityManager->deadEnemies.Add(iPoint(itemNode.attribute("x").as_int(), itemNode.attribute("y").as_int()));
		}
	}
	app->entityManager->DestroyDeadEnemies();


	return true;
}

//Create a method to save the state of the renderer
// using append_child and append_attribute
bool Scene2::SaveState(pugi::xml_node node) {

	pugi::xml_node camNode = node.append_child("player");
	camNode.append_attribute("x").set_value(player->position.x);
	camNode.append_attribute("y").set_value(player->position.y);

	pugi::xml_node enemiesNode = node.append_child("enemies");

	for (int i = 0; i < app->entityManager->enemies.Count(); i++)
	{
		pugi::xml_node enemy = enemiesNode.append_child("enemy");
		enemy.append_attribute("x").set_value(app->entityManager->enemies.At(i)->data->initialPos.x);
		enemy.append_attribute("y").set_value(app->entityManager->enemies.At(i)->data->initialPos.y);
		enemy.append_attribute("alive").set_value(app->entityManager->enemies.At(i)->data->active);

	}


	return true;
}
