#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Animation.h"

Item::Item() : Entity(EntityType::ITEM)
{
	
}

Item::~Item() {}

bool Item::Awake() {

	

	return true;
}

bool Item::Start() {
	name.Create("item");

	coinAnim.LoadAnimations("coinAnim");
	//initilize textures
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangleSensor(position.x + 8, position.y + 8, 4, 4, bodyType::KINEMATIC);
	pbody->ctype = ColliderType::ITEM;
	pbody->listener = this;
	currentAnim = &coinAnim;
	

	return true;
}

bool Item::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics. 
	currentAnim = &coinAnim;
	if (isDestroyed) {
		app->entityManager->DestroyEntity(pbody->listener);
		pbody->body->GetWorld()->DestroyBody(pbody->body);
	}
	coinAnim.Update();
	return true;
}

bool Item::PostUpdate()
{
	SDL_Rect rect = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, SDL_FLIP_NONE, &rect);

	return true;
}

bool Item::CleanUp()
{
	return true;
}


