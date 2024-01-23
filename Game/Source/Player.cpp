#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "ModuleFonts.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Scene2.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Animation.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "Window.h"
#include "Map.h"
#include "ModuleFadeToBlack.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	
}

Player::~Player() {

}

bool Player::Awake() {

	

	return true;
}

bool Player::Start() {



	name.Create("Player");

	//idle
	idleAnim.LoadAnimations("idleAnim");


	//run
	runAnim.LoadAnimations("runAnim");

	//jump
	jumpAnim.LoadAnimations("jumpAnim");


	//crouch
	crouchAnim.LoadAnimations("crouchAnim");


	//dash
	dashAnim.LoadAnimations("dashAnim");


	//attack
	attackAnim.LoadAnimations("attackAnim");

	//die
	dieAnim.LoadAnimations("dieAnim");

	
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	texturePath2 = parameters.attribute("texturepath2").as_string();
	scorePath = parameters.attribute("scorePath").as_string();
	coinPath = parameters.attribute("coinPath").as_string();
	speed = parameters.attribute("speed").as_float();
	speedDash = parameters.attribute("speedDash").as_float();
	jumpForce = parameters.attribute("jumpForce").as_float();
	timerDash = Timer();

	char lookupTable[] = { "0123456789" };
	scoreFont = app->fonts->Load(scorePath, lookupTable, 1);

	//initilize textures
	texture = app->tex->Load(texturePath);
	textureHeart = app->tex->Load(texturePath2);
	textureScore = app->tex->Load(scorePath);
	textureCoin = app->tex->Load(coinPath);

	pbody = app->physics->CreateCircle(position.x, position.y + 12, 14, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	pbody->body->SetFixedRotation(false);

	pickCoinFxId = app->audio->LoadAudios("pickCoin");
	jumpFxId = app->audio->LoadAudios("Jump");
	dashFxId = app->audio->LoadAudios("Dash");
	swordFxId = app->audio->LoadAudios("Sword");
	runFxId = app->audio->LoadAudios("Run");
	winFxId = app->audio->LoadAudios("Win");

	initialPos = pbody->body->GetTransform();
	currentAnim = &idleAnim;

	return true;
}

bool Player::Update(float dt)
{
	currentAnim = &idleAnim;
	isCrouching = false;
	
	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y);
	vel.y = pbody->body->GetLinearVelocity().y;



	//Godmode
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		app->godmode = !app->godmode;
	}
	//Movement
	if (app->godmode == true) {
		
		b2Vec2 vel = b2Vec2(0, 0);

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			vel = b2Vec2(0, -speed * dt);
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			vel = b2Vec2(0, speed * dt);
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isDashing) {
			vel = b2Vec2(-speed * dt, 0);
			isFacingLeft = true;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isDashing) {
			vel = b2Vec2(speed * dt, 0);
			isFacingLeft = false;
		}
		pbody->body->SetLinearVelocity(vel);
		pbody->body->SetGravityScale(0);
		pbody->body->GetFixtureList()[0].SetSensor(true);
		isDying = false;

	}

	//Jump


	if (app->godmode == false) {

		pbody->body->SetGravityScale(1);
		pbody->body->GetFixtureList()[0].SetSensor(false);
		
		
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && !isDying && !isAttacking) {
			currentAnim = &crouchAnim;
			isCrouching = true;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isDashing && !isDying &&!isCrouching && !isAttacking) {
			vel = b2Vec2(-speed * dt, pbody->body->GetLinearVelocity().y);
			currentAnim = &runAnim;
			isFacingLeft = true;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isDashing && !isDying && !isCrouching && !isAttacking) {
			vel = b2Vec2(speed * dt, pbody->body->GetLinearVelocity().y);
			currentAnim = &runAnim;
			isFacingLeft = false;
		}
		
		pbody->body->SetLinearVelocity(vel);

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !isDying && !isAttacking) {
			if (isJumping == false) {
				vel.y = 0;
				pbody->body->SetLinearVelocity(vel);
				pbody->body->ApplyLinearImpulse(b2Vec2(0, GRAVITY_Y * jumpForce), pbody->body->GetWorldCenter(), true);
				isJumping = true;
				app->audio->PlayFx(jumpFxId);
			}

		}
		//Dash
		if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN && !isDying && !isAttacking) {
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
				app->audio->PlayFx(dashFxId);
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
		{}
		//Attack
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && !isDying && !isDashing && !isJumping && !isAttacking) {
			isAttacking = true;
			if (isFacingLeft) {
				sensor = app->physics->CreateRectangleSensor(position.x - 10, position.y + 15,30, 40, bodyType::STATIC);
				sensor->ctype = ColliderType::ATTACK;
			}
			else {
				sensor = app->physics->CreateRectangleSensor(position.x + 40, position.y + 15, 30, 40, bodyType::STATIC);
				sensor->ctype = ColliderType::ATTACK;
			}
			app->audio->PlayFx(swordFxId);
		}

		if (isAttacking) {
			

			currentAnim = &attackAnim;
			if (attackAnim.HasFinished() == true) {
				isAttacking = false;
				attackAnim.Reset();
				currentAnim = &idleAnim;
				sensor->body->GetWorld()->DestroyBody(sensor->body);
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
			isDying = true;
		}

		if (lives == 0) {
			isDying = true;
		}
		//Die
		if (isDying) {
			currentAnim = &dieAnim;
			if (dieAnim.HasFinished()) {
				pbody->body->SetTransform(b2Vec2(initialPos.p.x, initialPos.p.y), initialPos.q.GetAngle());
				isDying = false;
				dieAnim.Reset();
				isFacingLeft = false;
				lives = 3;
			}
		}
		//Dash
		if (timerDash.ReadMSec() > 500 && isDashing) {

			isDashing = false;
			offsetTexY = 6;
			pbody->body->GetFixtureList()[0].GetShape()->m_radius = PIXEL_TO_METERS(14);
		}
	}

	if (dashAnim.HasFinished()) { isDashing = false; };
	if (isDashing == false) { dashAnim.Reset(); }
	if (isJumping == true) { currentAnim = &jumpAnim; };
	if (isJumping == false) { jumpAnim.Reset(); };
	if (isDashing && isJumping) { currentAnim = &dashAnim; };
	//Vsync

	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
		app->render->vsync = !app->render->vsync;
	}


	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	
	currentAnim->Update();
	

	// Cam Movement
	uint windowH;
	uint windowW;

	app->win->GetWindowSize(windowW, windowH);
	
	if (!app->godmode) {
		if (position.x < 255) {
			app->render->camera.x = -20;
		}
		else {
			app->render->camera.x = -position.x * app->win->GetScale() + (windowW / 2) - 25;

		}
		if (position.x > 3156) {
			app->render->camera.x = -5823;
		}
		if (position.y < 120) {
			app->render->camera.y = 0;
		}
		else {

			app->render->camera.y = -position.y * app->win->GetScale() + (windowH / 2) - 19;
		}
		if (position.y > 438) {
			app->render->camera.y = -650;
		}
	}

	

	return true;
}

bool Player::PostUpdate() {

	SDL_Rect rect = currentAnim->GetCurrentFrame();

	if (isFacingLeft) {
		app->render->DrawTexture(texture, position.x - 8, position.y - offsetTexY, SDL_FLIP_HORIZONTAL, &rect);
	}
	else {
		app->render->DrawTexture(texture, position.x - 8, position.y - offsetTexY, SDL_FLIP_NONE, &rect);
	}

	sprintf_s(scoreText, 10, "%d", score);
	app->fonts->BlitText(10, 25, scoreFont, scoreText);

	SDL_Rect coinRect = { 0,0,16,16 };
	app->render->DrawTexture(textureCoin, 15, 30, SDL_FLIP_NONE, &coinRect, 0);

	SDL_Rect heartRect = { 0,0,18,18 };

	if (lives == 3) {
		app->render->DrawTexture(textureHeart,15,10, SDL_FLIP_NONE, &heartRect, 0);
		app->render->DrawTexture(textureHeart, 35, 10, SDL_FLIP_NONE, &heartRect, 0);
		app->render->DrawTexture(textureHeart, 55, 10, SDL_FLIP_NONE, &heartRect, 0);
	}
	if (lives == 2) {
		app->render->DrawTexture(textureHeart, 15, 10, SDL_FLIP_NONE, &heartRect, 0);
		app->render->DrawTexture(textureHeart, 35, 10, SDL_FLIP_NONE, &heartRect, 0);
	}
	if (lives == 1) {
		app->render->DrawTexture(textureHeart, 15, 10, SDL_FLIP_NONE, &heartRect, 0);
	}


	

	return true;
}
bool Player::CleanUp()
{
	//RELEASE(pbody.algo)
	pbody->body->GetWorld()->DestroyBody(pbody->body);
	app->entityManager->DestroyEntity(pbody->listener);

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		score += 1;
		
		app->audio->PlayFx(pickCoinFxId);
		physB->listener->isDestroyed = true;
		break;

	case ColliderType::HEART:
		LOG("Collision	HEART");
		if (lives < 3) {
			app->audio->PlayFx(pickCoinFxId);
			physB->listener->isDestroyed = true;
			lives++;
		}
		
		break;

	case ColliderType::WALL:
		LOG("Collision WALL");
		break;

	case ColliderType::PLATFORM:
		isJumping = false;
		LOG("Collision PLATFORM");
		break;

	case ColliderType::SPIKES:
		if (lives > 0) {
			lives--;
		}
		isJumping = false;
		LOG("Collision SPIKES");
		break;

	case ColliderType::ENEMY:
		if (physB->listener->isAttacking) {
			/*isDying = true;*/
			//ALBERT, AQUÍ POSA UN SOROLL DE UGH
			if (lives > 0) {
				lives--;
			}
		}

		isJumping = false;
		LOG("Collision ENEMY");
		break;
	case ColliderType::WIN:
		LOG("Collision WIN");
		app->audio->PlayFx(winFxId);
		app->fadeToBlack->FadeToBlack(app->scene, app->scene2);
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}

bool Player::isOutOfBounds(int x, int y) {
	return true;
}