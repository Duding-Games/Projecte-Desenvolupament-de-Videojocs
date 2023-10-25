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

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	//idle

	idleAnim.PushBack({ 0 ,0 , 50, 37 });
	idleAnim.PushBack({ 50 ,0 , 50, 37 });
	idleAnim.PushBack({ 100 ,0 , 50, 37 });
	idleAnim.PushBack({ 150 ,0 , 50, 37 });
	idleAnim.speed = 0.1f;
	idleAnim.loop;

	//run

	runAnim.PushBack({ 50,40,50,37 });
	runAnim.PushBack({ 100,40,50,37 });
	runAnim.PushBack({ 150,40,50,37 });
	runAnim.PushBack({ 200,40,50,37 });
	runAnim.PushBack({ 250,40,50,37 });
	runAnim.speed = 0.1;
	runAnim.loop;

	//jump
	jumpAnim.PushBack({ 50, 75, 50,37 });
	jumpAnim.PushBack({ 100, 75, 50,37 });
	jumpAnim.PushBack({ 150, 75, 50,37 });
	jumpAnim.PushBack({ 200, 75, 50,37 });
	jumpAnim.PushBack({ 250, 75, 50,37 });
	jumpAnim.PushBack({ 300, 75, 50,37 });
	jumpAnim.PushBack({ 0, 120, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.PushBack({ 50, 110, 50,37 });
	jumpAnim.speed = 0.13f;

	//crouch

	crouchAnim.PushBack({ 50, 110, 50,37 });
	


}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateRectangle(position.x, position.y + 16, 16, 28, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	pbody->body->SetFixedRotation(true);

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
	currentAnim = &idleAnim;

	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y);
	vel.y = pbody->body->GetLinearVelocity().y;

	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		//
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		//
	}

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		vel = b2Vec2(-speed*dt, pbody->body->GetLinearVelocity().y);
		currentAnim = &runAnim;
		isFacingLeft = true;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		vel = b2Vec2(speed*dt, pbody->body->GetLinearVelocity().y);
		currentAnim = &runAnim;
		isFacingLeft = false;
	}


	pbody->body->SetLinearVelocity(vel);

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		if (isJumping == false) {
			vel.y = 0;
			pbody->body->SetLinearVelocity(vel);
			pbody->body->ApplyLinearImpulse(b2Vec2(0, GRAVITY_Y * 0.15f), pbody->body->GetWorldCenter(), true);
			isJumping = true;
			
		}
		
	}

	//pbody->body->SetLinearVelocity(vel);

	//if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
	//	if (isDashing = false) {
	//		vel.x = 0;
	//		pbody->body->SetLinearVelocity(vel);
	//		pbody->body->ApplyLinearImpulse(b2Vec2(0.5f,0), pbody->body->GetWorldCenter(), true);
	//		/*isDashing = true;*/
	//	}
	//}
	
	if (isJumping == true) { currentAnim = &jumpAnim; };
	if (isJumping == false) { jumpAnim.Reset(); };
	//if (jumpAnim.HasFinished() && isJumping == true) {
	//	jumpAnim.PushBack({ 50, 110, 50,37 });
	//	jumpAnim.loop = true;
	//}
	
	//Set the velocity of the pbody of the player
	

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	
	//app->render->DrawTexture(texture, position.x - 8, position.y-4,&currentAnim->GetCurrentFrame());

	currentAnim->Update();
	return true;
}

bool Player::PostUpdate() {

	SDL_Rect rect = currentAnim->GetCurrentFrame();

	if (isFacingLeft) {
		app->render->DrawTexture(texture, position.x, position.y, SDL_FLIP_HORIZONTAL, &rect);
	}
	else {
		app->render->DrawTexture(texture, position.x, position.y, SDL_FLIP_NONE, &rect);
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
		break;

	case ColliderType::WALL:
		LOG("Collision WALL");
		break;

	case ColliderType::PLATFORM:
		isJumping = false;
		LOG("Collision PLATFORM");
		break;

	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}