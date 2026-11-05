#include "Object.h"
#include "raylib.h"

Object::Object(Vector2 playerGridPos, Vector2 playerWorldPos)
{
	gridPosition = playerGridPos;
	worldPosition = playerWorldPos;
}

Object::~Object()
{
}

void Object::DestroyDestructable(char** collisionsMap, char** mapMap)
{
	if (collisionsMap[int(gridPosition.y) - 1][int(gridPosition.x)] == 'D')
	{
		collisionsMap[int(gridPosition.y) - 1][int(gridPosition.x)] = '0';
		mapMap[int(gridPosition.y) - 1][int(gridPosition.x)] = '0';
	}
	if (collisionsMap[int(gridPosition.y) + 1][int(gridPosition.x)] == 'D')
	{
		collisionsMap[int(gridPosition.y) + 1][int(gridPosition.x)] = '0';
		mapMap[int(gridPosition.y) + 1][int(gridPosition.x)] = '0';
	}
	if (collisionsMap[int(gridPosition.y)][int(gridPosition.x) - 1] == 'D')
	{
		collisionsMap[int(gridPosition.y)][int(gridPosition.x) - 1] = '0';
		mapMap[int(gridPosition.y)][int(gridPosition.x) - 1] = '0';
	}
	if (collisionsMap[int(gridPosition.y)][int(gridPosition.x) + 1] == 'D')
	{
		collisionsMap[int(gridPosition.y)][int(gridPosition.x) + 1] = '0';
		mapMap[int(gridPosition.y)][int(gridPosition.x) + 1] = '0';
	}
}

void Object::PrintObject()
{
	DrawSphere({ worldPosition.x, 1.0f, worldPosition.y }, 0.25f, BLACK);
}