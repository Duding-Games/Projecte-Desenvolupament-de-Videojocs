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
#include "Map.h"
#include "DynArray.h"
#include "Pathfinding.h"

EnemyBat::EnemyBat() : Entity(EntityType::ENEMYBAT)
{
	name.Create("EnemyBat");

	//idle
	idleAnim.LoadAnimations("idleAnimBat");

	//dying
	dieAnim.LoadAnimations("dyingAnimBat");

	//dead
	deadAnim.LoadAnimations("deadAnimBat");

	//attacking
	attackAnim.LoadAnimations("attackAnimBat");

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

	killFxId = app->audio->LoadAudios("Kill");

	pbody = app->physics->CreateCircle(position.x, position.y, 6, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetFixedRotation(false);
	pbody->body->SetGravityScale(0);


	
	initialPos.x = METERS_TO_PIXELS((pbody->body->GetTransform().p.x));
	initialPos.y = METERS_TO_PIXELS((pbody->body->GetTransform().p.y));
	isFacingLeft = true;

	return true;
}

bool EnemyBat::Update(float dt)
{

	

	currentAnim = &idleAnim;

	vel = b2Vec2(0, 0);

	if (app->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT) {
		vel = b2Vec2(speed * dt, 0);
	}

	if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		isFacingLeft = !isFacingLeft;
	}
	


	if (app->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN) {
		pbody->body->SetGravityScale(1);
		pbody->body->SetLinearVelocity(b2Vec2(0, -GRAVITY_Y));
		currentAnim = &dieAnim;
	}

	if(isAttacking && !isDead){
		currentAnim = &attackAnim;
	}

	
	Bathfinding(dt);

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;
	
	pbody->body->SetLinearVelocity(vel);
	
	if (isDying) {
		isAttacking = false;
		pbody->body->SetGravityScale(1);
		pbody->body->SetLinearVelocity(b2Vec2(0, -GRAVITY_Y - 17.0f));
		currentAnim = &dieAnim;

	}

	if (isDead) {
		currentAnim = &deadAnim;
		if (deadAnim.HasFinished() == true) {
			pbody->body->GetWorld()->DestroyBody(pbody->body);
			app->entityManager->DestroyEntity(pbody->listener);
			active = false;
			deadAnim.Reset();
		}
	}

	currentAnim->Update();

	return true;
}

bool EnemyBat::PostUpdate() {

	SDL_Rect rect = currentAnim->GetCurrentFrame();

	if (isFacingLeft) {
		app->render->DrawTexture(texture, position.x + 7, position.y + 7, SDL_FLIP_HORIZONTAL, &rect);
	}
	else {
		app->render->DrawTexture(texture, position.x + 5, position.y + 7, SDL_FLIP_NONE, &rect);
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
		isAttacking = false;
		isDying = true;
		app->audio->PlayFx(killFxId);
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
		isDead = true;
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

bool EnemyBat::Bathfinding(float dt)
{
	if(app->map->pathfinding_flying->GetDistance(app->scene->GetPLayer()->position, position) <= 150){

		iPoint playerPos = app->map->WorldToMap(app->scene->GetPLayer()->position.x, app->scene->GetPLayer()->position.y);
		playerPos.x += 1;
		iPoint enemyPos = app->map->WorldToMap(position.x,position.y);

		app->map->pathfinding_flying->CreatePath(playerPos,enemyPos);
		lastPath = *app->map->pathfinding_flying->GetLastPath();
		// L13: Get the latest calculated path and draw
		for (uint i = 0; i < lastPath.Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(lastPath.At(i)->x, lastPath.At(i)->y);
			if (app->physics->debug == true) {
				app->render->DrawTexture(app->map->pathfinding_flying->mouseTileTex, pos.x, pos.y, SDL_FLIP_NONE);
			}
		}

		if (lastPath.Count() > 2) {
			if (lastPath.At(lastPath.Count() - 2)->x < enemyPos.x) {
				vel.x -= speed * dt;
				isFacingLeft = true;
			}

			if (lastPath.At(lastPath.Count() - 2)->x > enemyPos.x) {
				vel.x += speed * dt; 
					isFacingLeft = false;
			}

			if (lastPath.At(lastPath.Count() - 2)->y < enemyPos.y) {
				vel.y -= speed * dt;
			}

			if (lastPath.At(lastPath.Count() - 2)->y > enemyPos.y) {
				vel.y += speed * dt;
			}
			isAttacking = false;
			attackAnim.Reset();
			
				pbody->body->SetLinearVelocity(vel);
			
		}

		if (app->map->pathfinding_flying->GetDistance(app->scene->GetPLayer()->position, position) <= 66){
			
			if (isFacingLeft) {
				vel.x -= speed  * dt;
			}
			else {
				vel.x += speed  * dt;

			}
			isAttacking = true;
			
				pbody->body->SetLinearVelocity(vel);
			
		}
	
	}

	else {

		currentAnim = &idleAnim;

		if (isFacingLeft) {
			if(position.x > initialPos.x - 40) {
				vel.x -= speed * dt;
			}
			else {
				isFacingLeft = false;
			}
		}


		else{
			if (position.x < initialPos.x + 40) {
				vel.x += speed * dt;
			}
			else {
				isFacingLeft = true;
			}
		}
		
			pbody->body->SetLinearVelocity(vel);
		
	}
	return true;

}
