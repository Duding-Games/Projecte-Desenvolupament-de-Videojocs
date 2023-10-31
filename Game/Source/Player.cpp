#include "Player.h"
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

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	//idle

	idleAnim.PushBack({ 0 ,0 , 50, 37 });
	idleAnim.PushBack({ 50 ,0 , 50, 37 });
	idleAnim.PushBack({ 100 ,0 , 50, 37 });
	idleAnim.PushBack({ 150 ,0 , 50, 37 });
	idleAnim.speed = 0.1f;
	

	//run

	runAnim.PushBack({ 50,37,50,37 });
	runAnim.PushBack({ 100,37,50,37 });
	runAnim.PushBack({ 150,37,50,37 });
	runAnim.PushBack({ 200,37,50,37 });
	runAnim.PushBack({ 250,37,50,37 });
	runAnim.speed = 0.1;


	//jump
	jumpAnim.PushBack({ 50, 75, 50,37 });
	jumpAnim.PushBack({ 100, 75, 50,37 });
	jumpAnim.PushBack({ 150, 75, 50,37 });
	jumpAnim.PushBack({ 200, 75, 50,37 });
	jumpAnim.PushBack({ 250, 75, 50,37 });
	jumpAnim.PushBack({ 300, 75, 50,37 });
	jumpAnim.PushBack({ 0, 120, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.loop = false;
	jumpAnim.speed = 0.17f;

	//crouch
	crouchAnim.PushBack({ 200, 0, 50,37 });
	crouchAnim.PushBack({ 250, 0, 50,37 });
	crouchAnim.PushBack({ 300, 0, 50,37 });
	crouchAnim.PushBack({ 0, 37, 50,37 });
	crouchAnim.speed = 0.07f;
	
	//dash
	dashAnim.PushBack({ 150, 111, 50,37 });
	dashAnim.PushBack({ 200, 111, 50,37 });
	dashAnim.PushBack({ 250, 111, 50,37 });
	dashAnim.PushBack({ 300, 111, 50,37 });
	dashAnim.PushBack({ 0, 148, 50,37 });
	dashAnim.speed = 0.1f;

	//attack
	attackAnim.PushBack({ 0, 222, 50,37 });
	attackAnim.PushBack({ 50, 222, 50,37 });
	attackAnim.PushBack({ 100, 222, 50,37 });
	attackAnim.PushBack({ 150, 222, 50,37 });
	attackAnim.PushBack({ 200, 222, 50,37 });
	attackAnim.PushBack({ 250, 222, 50,37 });
	attackAnim.PushBack({ 300, 222, 50,37 });
	attackAnim.PushBack({ 0, 259, 50,37 });
	attackAnim.PushBack({ 50, 259, 50,37 });
	attackAnim.PushBack({ 100, 259, 50,37 });
	attackAnim.PushBack({ 150, 259, 50,37 });
	attackAnim.PushBack({ 200, 259, 50,37 });
	attackAnim.PushBack({ 250, 259, 50,37 });
	attackAnim.PushBack({ 300, 259, 50,37 });
	attackAnim.PushBack({ 0, 296, 50,37 });
	attackAnim.PushBack({ 50, 296, 50,37 });
	attackAnim.PushBack({ 100, 296, 50,37 });
	attackAnim.speed = 0.15f;

	//die
	dieAnim.PushBack({ 100, 333, 50,37 });
	dieAnim.PushBack({ 150, 333, 50,37 });
	dieAnim.PushBack({ 200, 333, 50,37 });
	dieAnim.PushBack({ 250, 333, 50,37 });
	dieAnim.PushBack({ 150, 333, 50,37 });
	dieAnim.PushBack({ 200, 333, 50,37 });
	dieAnim.PushBack({ 250, 333, 50,37 });
	dieAnim.PushBack({ 200, 333, 50,37 });
	dieAnim.speed = 0.07f;
	dieAnim.loop = false;
}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	timerDash = Timer();
	
	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateCircle(position.x, position.y + 12, 14, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	pbody->body->SetFixedRotation(false);

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
	currentAnim = &idleAnim;

	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y);
	vel.y = pbody->body->GetLinearVelocity().y;

	//Godmode
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		godmode = !godmode;
	}
	//Movement
	if (godmode == true) {
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			vel = b2Vec2(0, -speed * dt);
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			vel = b2Vec2(0, speed *dt);
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isDashing) {
			vel = b2Vec2(-speed * dt,0);
			isFacingLeft = true;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isDashing) {
			vel = b2Vec2(speed * dt, 0);
			isFacingLeft = false;
		}
		
	}

	//Jump

	
	if (godmode == false) {

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isDashing) {
			vel = b2Vec2(-speed * dt, pbody->body->GetLinearVelocity().y);
			currentAnim = &runAnim;
			isFacingLeft = true;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isDashing) {
			vel = b2Vec2(speed * dt, pbody->body->GetLinearVelocity().y);
			currentAnim = &runAnim;
			isFacingLeft = false;
		}

		pbody->body->SetLinearVelocity(vel);

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			if (isJumping == false) {
				vel.y = 0;
				pbody->body->SetLinearVelocity(vel);
				pbody->body->ApplyLinearImpulse(b2Vec2(0, GRAVITY_Y * 0.11f), pbody->body->GetWorldCenter(), true);
				isJumping = true;

			}

		}
		//Dash
		if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
			if (!isDashing) {
				timerDash.Start();
				if (isFacingLeft) {
					vel = b2Vec2(-speedDash * dt, 0);
					pbody->body->SetLinearVelocity(vel);
					currentAnim = &dashAnim;
				}
				else {
					vel = b2Vec2(speedDash * dt, 0);
					pbody->body->SetLinearVelocity(vel);
					currentAnim = &dashAnim;
				}
				pbody->body->GetFixtureList()[0].GetShape()->m_radius = PIXEL_TO_METERS(7);
				isDashing = true;
				offsetTexY = 12;
			}

		}
		if (timerDash.ReadMSec() < 500 && isDashing) {
			if (isFacingLeft) {
				vel = b2Vec2(-speedDash * dt, 0);
				pbody->body->SetLinearVelocity(vel);
				currentAnim = &dashAnim;
			}
			else {
				vel = b2Vec2(speedDash * dt, 0);
				pbody->body->SetLinearVelocity(vel);
				currentAnim = &dashAnim;
			}

			isDashing = true;
		}
		//Attack
		if (app->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) {
			currentAnim = &attackAnim;
			isAttacking = true;
		}
		//Die
		if (isDying) {
			currentAnim = &dieAnim;
			position.x = 200;
			position.y = 460;
		}


		//Dash
		if (timerDash.ReadMSec() > 500 && isDashing) {

			isDashing = false;
			offsetTexY = 6;
			pbody->body->GetFixtureList()[0].GetShape()->m_radius = PIXEL_TO_METERS(14);
			//Dash
			if (timerDash.ReadMSec() > 500 && isDashing) {

				isDashing = false;
				offsetTexY = 6;
				pbody->body->GetFixtureList()[0].GetShape()->m_radius = PIXEL_TO_METERS(14);
				//Dash
				if (timerDash.ReadMSec() > 500 && isDashing) {

					isDashing = false;
					offsetTexY = 6;
					pbody->body->GetFixtureList()[0].GetShape()->m_radius = PIXEL_TO_METERS(14);

				}

			}

		}
	}
	if (dashAnim.HasFinished()) { isDashing = false; };
	if (isDashing == false) { dashAnim.Reset(); }
	if (isJumping == true) { currentAnim = &jumpAnim; };
	if (isJumping == false) { jumpAnim.Reset(); };


	

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	currentAnim->Update();
	_isCrouching = isCrouching;

	//TODO Cam Movement
	uint windowH;
	uint windowW;
	app->win->GetWindowSize(windowW, windowH);

	app->render->camera.x = MIN(-position.x * app->win->GetScale() + (windowW / 2), -20);
	app->render->camera.y = -position.y * app->win->GetScale() + (windowH / 2);
	LOG("---------Player : %d", -position.x * app->win->GetScale() + (windowW / 2));
	LOG("---------Camera : %d", app->render->camera.x);

	//uint windowH;
	//uint windowW;
	//app->win->GetWindowSize(windowW, windowH);

	//int mapLimitX = -20; // Set your map limit for x-coordinate here
	//int mapLimitY = 0; // Set your map limit for y-coordinate here

	//int cameraX = -position.x * app->win->GetScale() + (windowW / 2);
	//int cameraY = -position.y * app->win->GetScale() + (windowH / 2);

	//if (cameraX < mapLimitX * app->win->GetScale()) {
	//	app->render->camera.x = mapLimitX * app->win->GetScale();
	//}
	//else {
	//	app->render->camera.x = cameraX;
	//}

	//if (cameraY < mapLimitY * app->win->GetScale()) {
	//	app->render->camera.y = mapLimitY * app->win->GetScale();
	//}
	//else {
	//	app->render->camera.y = cameraY;
	//}

	//LOG("---------Player : %d", -position.x * app->win->GetScale() + (windowW / 2));
	//LOG("---------Camera : %d", app->render->camera.x);

	

	return true;
}

bool Player::PostUpdate() {

	SDL_Rect rect = currentAnim->GetCurrentFrame();

	if (isFacingLeft) {
		app->render->DrawTexture(texture, position.x - 8, position.y- offsetTexY, SDL_FLIP_HORIZONTAL, &rect);
	}
	else {
		app->render->DrawTexture(texture, position.x-8, position.y- offsetTexY, SDL_FLIP_NONE, &rect);
	}

	return true;
}
bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		/*app->entityManager->DestroyEntity(physB->listener);*/
		break;

	case ColliderType::WALL:
		LOG("Collision WALL");
		break;

	case ColliderType::PLATFORM:
		isDying = false;
		isJumping = false;
		LOG("Collision PLATFORM");
		break;

	case ColliderType::SPIKES:
		isDying = true;
		LOG("Collision SPIKES");
		break;

	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}

bool Player::isOutOfBounds(int x, int y) {
	return true;
}