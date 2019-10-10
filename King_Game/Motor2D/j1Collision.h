#ifndef __j1Collision_H__
#define __j1Collision_H__

#define MAX_COLLIDERS 50

#include "j1Module.h"

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_WALL_LEFT,
	COLLIDER_WALL_RIGHT,
	COLLIDER_PLAYER,

	COLLIDER_PLAYER_PARTICLES,
	COLLIDER_PLAYER_2_PARTICLES,
	COLLIDER_PLAYER_2,
	
	COLLIDER_PLAYER_1_ATTACK,
	COLLIDER_PLAYER_2_ATTACK,

	COLLIDER_FX_PARTICLES,

	COLLIDER_MAX,
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}
	void SetSize(int w, int h)
	{
		rect.w = w;
		rect.h = h;
	}
	bool CheckCollision(const SDL_Rect& r) const;
};

class j1Collision : public j1Module
{
public:

	j1Collision();
	~j1Collision();

	bool PreUpdate();
	bool Update();
	bool CleanUp();

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	void DebugDraw();

private:
	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	//show colliders
	bool debug = true;
};
#endif // __j1Collision_H__