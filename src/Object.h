#pragma once
#include "raylib.h"

class Object
{
public:
	Object(Vector2 playerGridPos, Vector2 playerWorldPos);
	~Object();

	Vector2 gridPosition;
	Vector2 worldPosition;
	float timer = 180;

	void DestroyDestructable(char** collisionsMap, char** mapMap);
	void PrintObject();

private:

};