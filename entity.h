#ifndef ENTITY
#define ENTITY

#include "globals.h"
#include "timeController.h"
#include "animationSet.h"

//Abstract class
class Entity {
public:
	static const int DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NONE; // reference constants
	int state; // quick label to see what the entity is up to
	float x, y;
	int direction;
	bool solid = true;
	bool collideWithSolids = true;
	bool active = true;
	string type = "entity";
	bool moving;
	float angle;
	float moveSpeed;
	float moveSpeedMax;
	float slideAngle;
	float slideAmount;
	float moveLerp = 4;
	float totalXMove, totalYMove;
	SDL_Rect collisionBox;
	SDL_Rect lastCollisionBox;
	int collisionBoxW, collisionBoxH;
	float collisionBoxYOffset;
	AnimationSet* animSet;
	Animation* currentAnim;
	Frame* currentFrame;
	float frameTimer;

	// virtual functions
	virtual void update();
	virtual void draw();
	virtual void move(float angle);
	virtual void updateMovement();
	virtual void updateCollisionBox();
	virtual void changeAnimation(int newState, bool resetFrameToBeginning) = 0; // abstract function
	virtual void updateCollisions();

	// help functions
	static float distanceBetweenTwoRects(SDL_Rect& r1, SDL_Rect& r2);
	static float distanceBetweenTwoEntities(Entity* e1, Entity* e2);
	static float angleBetweenTwoEntities(Entity* e1, Entity* e2);
	static bool checkCollision(SDL_Rect cbox1, SDL_Rect cbox2);
	static int angleToDirection(float angle);
	static float angleBetweenTwoPoints(float cx1, float cy1, float cx2, float cy2);
	static float angleBetweenTwoRects(SDL_Rect& r1, SDL_Rect& r2);

	// global entities list
	static list<Entity*> entities;
	static bool EntityCompare(const Entity* const& a, const Entity* const& b); // compare two entities in a list
	static void removeInactiveEntitiesFromList(list<Entity*>* entityList, bool deleteEntities);
	static void removeAllFromList(list<Entity*>* entityList, bool deleteEntities);
};

#endif
