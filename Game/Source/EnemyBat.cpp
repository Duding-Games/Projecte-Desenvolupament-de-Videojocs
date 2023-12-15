#include "EnemyBat.h"
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
#include "Timer.h"
#include "PerfTimer.h"
#include "Window.h"

EnemyBat::EnemyBat() : Entity(EntityType::ENEMYBAT)
{
	name.Create("EnemyBat");

	//idle
	idleAnim.LoadAnimations("idleAnimBat");

	//dying
	dieAnim.LoadAnimations("dyingAnimBat");

	//dead
	deadAnim.LoadAnimations("deadAnimBat");

}

EnemyBat::~EnemyBat() {

}

bool EnemyBat::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	speed = parameters.attribute("speed").as_float();

	return true;
}

bool EnemyBat::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateCircle(position.x, position.y, 6, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetFixedRotation(false);
	pbody->body->SetGravityScale(0);

	initialPos = pbody->body->GetTransform();
	isFacingLeft = true;

	return true;
}

bool EnemyBat::Update(float dt)
{
	currentAnim = &idleAnim;

	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y);
	vel.y = pbody->body->GetLinearVelocity().y;

	if (app->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT) {
		vel = b2Vec2(speed * dt, 0);
	}

	if(isDead) {
		currentAnim = &deadAnim;
		/*pbody->body->GetWorld()->DestroyBody(pbody->body);*/
		pbody->body->SetActive(false);
		if (deadAnim.HasFinished() == true) {
			app->entityManager->DestroyEntity(pbody->listener);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN) {
		pbody->body->SetGravityScale(1);
		pbody->body->SetLinearVelocity(b2Vec2(0, -GRAVITY_Y));
		currentAnim = &dieAnim;
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	pbody->body->SetLinearVelocity(vel);

	currentAnim->Update();

	return true;
}

bool EnemyBat::PostUpdate() {

	SDL_Rect rect = currentAnim->GetCurrentFrame();

	if (isFacingLeft) {
		app->render->DrawTexture(texture, position.x - 4, position.y + 7, SDL_FLIP_HORIZONTAL, &rect);
	}
	else {
		app->render->DrawTexture(texture, position.x + 3, position.y + 2, SDL_FLIP_NONE, &rect);
	}

	return true;
}
bool EnemyBat::CleanUp()
{

	return true;
}

void EnemyBat::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		break;
	case ColliderType::ATTACK:
		LOG("Collision ATTACK");
		pbody->body->SetGravityScale(1);
		pbody->body->SetLinearVelocity(b2Vec2(0, -GRAVITY_Y));
		currentAnim = &dieAnim;
		break;
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		break;

	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;

	case ColliderType::WALL:
		LOG("Collision WALL");
		break;

	case ColliderType::PLATFORM:
		isDead = true;
		LOG("Collision PLATFORM");
		break;

	case ColliderType::SPIKES:

		LOG("Collision SPIKES");
		break;

	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}

bool EnemyBat::isOutOfBounds(int x, int y) {
	return true;
}