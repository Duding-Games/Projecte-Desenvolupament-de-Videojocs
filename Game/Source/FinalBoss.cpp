#include "FinalBoss.h"
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

FinalBoss::FinalBoss() : Entity(EntityType::FINALBOSS)
{
	name.Create("FinalBoss");

	//spawn
	spawnAnim.LoadAnimations("spawnAnimBoss");

	//idle
	idleAnim.LoadAnimations("idleAnimBoss");

	//dying
	dieAnim.LoadAnimations("dyingAnimBoss");

	//attacking
	attackAnim.LoadAnimations("attackAnimBoss");

	//walking
	walkAnim.LoadAnimations("walkAnimBoss");

	//take damage
	damagedAnim.LoadAnimations("damagedAnimBoss");

}

FinalBoss::~FinalBoss() {

}

bool FinalBoss::Awake() {

	
	return true;
}

bool FinalBoss::Start() {

	//initilize textures
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	speed = parameters.attribute("speed").as_float();

	texture = app->tex->Load(texturePath);

	killFxId = app->audio->LoadAudios("Kill");

	pbody = app->physics->CreateCircle(position.x, position.y, 40, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::BOSS;
	pbody->body->SetFixedRotation(false);
	initialPos.x = (pbody->body->GetTransform().p.x);
	initialPos.y = (pbody->body->GetTransform().p.y);
	isFacingLeft = true;
	currentAnim = &idleAnim;

	return true;
}

bool FinalBoss::Update(float dt)
{

	if (!active) {
		pbody->body->GetWorld()->DestroyBody(pbody->body);
	}

	currentAnim = &idleAnim;
	vel = b2Vec2(0, -GRAVITY_Y - 17.0f);

	if (!isDead) {
		if (isAttacking) currentAnim = &attackAnim;

		Bossfinding(dt);
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 120;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 95;

	pbody->body->SetLinearVelocity(vel);
	
	if (isDead) {
		currentAnim = &dieAnim;
		if (dieAnim.HasFinished()) {
			app->entityManager->DestroyEntity(pbody->listener);
			pbody->body->GetWorld()->DestroyBody(pbody->body);
			active = false;
			dieAnim.Reset();
		}
	}

	currentAnim->Update();

	return true;
}

bool FinalBoss::PostUpdate() {

	SDL_Rect rect = currentAnim->GetCurrentFrame();

	if (isFacingLeft) {
		app->render->DrawTexture(texture, position.x - 18, position.y - 23, SDL_FLIP_HORIZONTAL, &rect);
	}
	else {
		app->render->DrawTexture(texture, position.x - 18, position.y - 23, SDL_FLIP_NONE, &rect);
	}

	return true;
}
bool FinalBoss::CleanUp()
{

	return true;
}

void FinalBoss::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		break;
	case ColliderType::ATTACK:
		LOG("Collision ATTACK");
		isAttacking = false;
		isDead = true;
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

bool FinalBoss::isOutOfBounds(int x, int y) {
	return true;
}

bool FinalBoss::Bossfinding(float dt)
{
	if(app->map->pathfinding_walking->GetDistance(app->scene->GetPLayer()->position, position) <= 120){

		iPoint playerPos = app->map->WorldToMap(app->scene->GetPLayer()->position.x, app->scene->GetPLayer()->position.y);
		playerPos.x += 1;
		playerPos.y +=1;
		iPoint enemyPos = app->map->WorldToMap(position.x,position.y);
		enemyPos.y += 1;

		app->map->pathfinding_walking->CreatePath(playerPos,enemyPos);
		lastPath = *app->map->pathfinding_walking->GetLastPath();

		//Get the latest calculated path and draw
		for (uint i = 0; i < lastPath.Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(lastPath.At(i)->x, lastPath.At(i)->y);
			if (app->physics->debug == true) {
				app->render->DrawTexture(app->map->pathfinding_walking->mouseTileTex, pos.x, pos.y, SDL_FLIP_NONE);
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

			isAttacking = false;
			attackAnim.Reset();
			pbody->body->SetLinearVelocity(vel);
		}

		if (app->map->pathfinding_walking->GetDistance(app->scene->GetPLayer()->position, position) <= 66){
			
			if (isFacingLeft) {
				vel.x -= speed * dt;
			}
			else {
				vel.x += speed * dt;

			}
			isAttacking = true;
			pbody->body->SetLinearVelocity(vel);
		}
	
	}
	//doesn't work how it should
	/*else {

		isAttacking = false;

		if (!isDead) {
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
		
	}*/
	return true;

}
