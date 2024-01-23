#include "Heart.h"
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

Heart::Heart() : Entity(EntityType::HEART)
{
	name.Create("heart");

	heartAnim.LoadAnimations("heartAnim");
}

Heart::~Heart() {}

bool Heart::Awake() {

	

	return true;
}

bool Heart::Start() {


	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangleSensor(position.x + 8, position.y + 8, 4, 4, bodyType::KINEMATIC);
	pbody->ctype = ColliderType::HEART;
	pbody->listener = this;
	
	currentAnim = &heartAnim;

	return true;
}

bool Heart::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics. 
	currentAnim = &heartAnim;
	if (isDestroyed) {
		app->entityManager->DestroyEntity(pbody->listener);
		pbody->body->GetWorld()->DestroyBody(pbody->body);
	}
	heartAnim.Update();
	return true;
}

bool Heart::PostUpdate()
{
	SDL_Rect rect = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, SDL_FLIP_NONE, &rect);

	return true;
}

bool Heart::CleanUp()
{
	return true;
}


