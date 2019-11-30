#include "j1EntityManager.h"
#include "j1App.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "j1UI.h"
#include "j1Particles.h"
#include "j1WalkingEnemy.h"
#include "j1FlyingEnemy.h"
#include "j1Trap.h"
#include "brofiler/Brofiler/Brofiler.h"


j1EntityManager::j1EntityManager(){
	name.create("entities");
}


j1EntityManager::~j1EntityManager(){
}

j1Entity* j1EntityManager::CreateEntity(EntityType type, int position_x, int position_y)
{
	//static_assert(EntityType::UNKNOWN == 4, "code needs update");
	j1Entity* entity = nullptr;
	switch (type)
	{
	case EntityType::PLAYER:
		entity = new j1Player();
		break;
	case EntityType::WALKING_ENEMY:
		entity = new j1WalkingEnemy();
		entity->position.x = entity->initial_x_position = position_x;
		entity->position.y = entity->initial_y_position = position_y;
		break;
	case EntityType::FLYING_ENEMY:
		entity = new j1FlyingEnemy();
		entity->position.x = entity->initial_x_position = position_x;
		entity->position.y = entity->initial_y_position = position_y;
		break;
	case EntityType::TRAP:
		entity = new j1Trap();
		entity->position.x = entity->initial_x_position = position_x;
		entity->position.y = entity->initial_y_position = position_y;
		break;
	case EntityType::PARTICLES:
		break;
	case EntityType::UNKNOWN:
		break;
	default:
		break;
	}

	if (entity != nullptr) entities.add(entity);

	return entity;
}

void j1EntityManager::DestroyEntity(j1Entity* entity)
{
	p2List_item<j1Entity*>* item;

	if (entity != nullptr) {
		item = entities.At(entities.find(entity));
		if (entity->collider != nullptr)
		{
			entity->collider->to_delete = true;
			entity->collider = nullptr;
		}
		if (entity->raycast != nullptr) {
			entity->raycast->to_delete = true;
			entity->raycast = nullptr;
		}
		entities.del(item);;
	}
}

void j1EntityManager::DestroyAllEntities() {
	p2List_item<j1Entity*>* item;

	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data != player) {
			DestroyEntity(item->data);
		}
	}
	LOG("");
}

bool j1EntityManager::Awake(pugi::xml_node& config){
	bool ret = true;

	config_data = config;

	gravity = config.child("gravity").attribute("value").as_int();
	max_falling_speed = config.child("max_falling_speed").attribute("value").as_int();

	//player creation
	player = new j1Player();
	player->Awake(config.child("player"));
	entities.add(player);

	//reference walking enemy
	reference_walking_enemy = new j1WalkingEnemy();
	reference_walking_enemy->Awake(config.child("walking_enemy"));

	//reference flying enemy
	reference_flying_enemy = new j1FlyingEnemy();
	reference_flying_enemy->Awake(config.child("flying_enemy"));

	return ret;
}

bool j1EntityManager::Start()
{
	bool ret = true;

	player->Start();
	reference_walking_enemy->texture = App->tex->Load("sprites/characters/sheet_hero_idle.png");
	reference_flying_enemy->texture = App->tex->Load("sprites/characters/Sprite_bat.png");

	for (p2List_item<j1Entity*>* entity = entities.start; entity != nullptr; entity = entity->next)
	{
		if (entity->data->type == EntityType::WALKING_ENEMY){
			entity->data->texture = reference_walking_enemy->texture;}
		if (entity->data->type == EntityType::FLYING_ENEMY){
			entity->data->texture = reference_flying_enemy->texture; }
	}

	return ret;
}

bool j1EntityManager::CleanUp()
{
	bool ret = true;

	App->tex->UnLoad(trap_texture);
	trap_texture = nullptr;

	App->tex->UnLoad(reference_walking_enemy->texture);
	reference_walking_enemy->texture = nullptr;

	App->tex->UnLoad(reference_flying_enemy->texture);
	reference_flying_enemy->texture = nullptr;

	//destroy all entities
	for (p2List_item<j1Entity*>* entity = entities.start; entity != nullptr; entity = entity->next)
	{
		entity->data->DestroyEntity(entity->data);
	}

	return ret;
}

bool j1EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("EntitiesPreUpdate", Profiler::Color::Bisque)
	bool ret = true;
	player->PreUpdate();
	return ret;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntitiesUpdate", Profiler::Color::Bisque)
	bool ret = true;

		for (p2List_item<j1Entity*>* entity = entities.start; entity != nullptr; entity = entity->next)
		{
			entity->data->Update(dt);
		}
	accumulated_time += dt;
	//LOG("Accumulated time: %f", accumulated_time);

	/*if (entity != nullptr)
		{
			if (entity->data == player) {
				entity->data->Update(dt);
			}
			else if (accumulated_time > time_between_updates) {
				entity->data->Update(dt);
				accumulated_time = 0;
			}
	}*/
	return ret;
}

bool j1EntityManager::PostUpdate()
{
	BROFILER_CATEGORY("EntitiesPostUpdate", Profiler::Color::Bisque)
	bool ret = true;
	for (p2List_item<j1Entity*>* entity = entities.start; entity != nullptr; entity = entity->next)
	{
		entity->data->PostUpdate();
	}
	return ret;
}

void j1EntityManager::RellocateEntities() {
	for (p2List_item<j1Entity*>* entity = entities.start; entity != nullptr; entity = entity->next)
	{
		if (entity->data != player)
		{
			entity->data->position.x = entity->data->initial_x_position;
			entity->data->position.y = entity->data->initial_y_position;
			entity->data->state = IDLE;
			entity->data->going_after_player = false;
		}
	}
}

bool j1EntityManager::Load(pugi::xml_node& data)
{
	CleanUp();
	bool ret = true;
	p2List_item<j1Entity*>* item;
	
	for (pugi::xml_node WALKING_ENEMY = data.child("walking_enemy"); WALKING_ENEMY; WALKING_ENEMY = WALKING_ENEMY.next_sibling("walking_enemy"))
	{
		CreateEntity(EntityType::WALKING_ENEMY, WALKING_ENEMY.attribute("position_x").as_int(), WALKING_ENEMY.attribute("position_y").as_int());
	}
	
	for (pugi::xml_node FLYING_ENEMY = data.child("flying_enemy"); FLYING_ENEMY; FLYING_ENEMY = FLYING_ENEMY.next_sibling("flying_enemy"))
	{
		CreateEntity(EntityType::FLYING_ENEMY, FLYING_ENEMY.attribute("position_x").as_int(), FLYING_ENEMY.attribute("position_y").as_int());
	}
	
	
	return ret;
}

bool j1EntityManager::Save(pugi::xml_node& data)
{
	bool ret = true;
	p2List_item<j1Entity*>* item;
	
	for (item = entities.start; item != nullptr; item = item->next)
	{
		pugi::xml_node child = data.append_child(item->data->name.GetString());
		child.append_attribute("position_x") = item->data->position.x;
		child.append_attribute("position_y") = item->data->position.y;
	}
	
	return ret;
}
