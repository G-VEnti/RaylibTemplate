/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "rlgl.h"       // Required to define vertex data (immediate-mode style)

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>

std::string header;
std::string version;
std::string title;
std::string tmp;

int textureAmount;
int lvlWidth;
int lvlHeight;

char** floorMap;
char** mapMap;
char** collisionsMap;
char** objectsMap;

std::map<char, std::string> texturesNamesMap;
std::map<char, Texture2D> textures2DMap;

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color); // Draw cube textured
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color); // Draw cube with a region of a texture

int start_raylib(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - cubicmap rendering");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
	camera.position = { 0.0f, 7.0f, 8.0f };     // Camera position
	camera.target = { 0.0f, 0.0f, 0.0f };          // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };              // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                    // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;               // Camera projection type

	for (auto const& texturesMapIt : texturesNamesMap)
	{
		tmp = "resources/" + texturesMapIt.second;

		Image tmpImage = LoadImage(tmp.c_str());				// Load cubicmap image (RAM)
		Texture2D tmpText = LoadTextureFromImage(tmpImage);		// Convert image to texture to display (VRAM)
		UnloadImage(tmpImage);

		textures2DMap.insert({ texturesMapIt.first, tmpText });
	}

    //UnloadImage(image);     // Unload cubesmap image from RAM, already uploaded to VRAM

    bool pause = true;     // Pause camera orbital rotation (and zoom)

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_P)) pause = !pause;

        if (!pause) UpdateCamera(&camera, CAMERA_ORBITAL);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		float offsetX = -lvlWidth / 2;
		if (lvlWidth % 2 == 0)
		{
			offsetX += 0.5;
		}

		float offsetZ = -lvlHeight / 2;
		if (lvlHeight % 2 == 0)
		{
			offsetZ += 0.5;
		}

		// Draw cube with an applied texture
		for (int i = 0; i < lvlHeight; i++)
		{
			for (int j = 0; j < lvlWidth; j++)
			{
				if (floorMap[i][j] != '0')
				{
					DrawCubeTexture(textures2DMap[floorMap[i][j]], { float(j) + offsetX, 0.0f, float(i) + offsetZ }, 1.0f, 1.0f, 1.0f, WHITE);
				}
			}
		}

		for (int i = 0; i < lvlHeight; i++)
		{
			for (int j = 0; j < lvlWidth; j++)
			{
				if (mapMap[i][j] != '0')
				{
					DrawCubeTexture(textures2DMap[mapMap[i][j]], { float(j) + offsetX, 1.0f, float(i) + offsetZ }, 1.0f, 1.0f, 1.0f, WHITE);
				}
			}
		}

		for (int i = 0; i < lvlHeight; i++)
		{
			for (int j = 0; j < lvlWidth; j++)
			{
				if (objectsMap[i][j] != '0')
				{
					DrawSphere({ float(j) + offsetX, 1.0f, float(i) + offsetZ }, 0.25f, RED);
				}
			}
		}

		//DrawCubeTexture(textures2DMap['a'], { 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 1.0f, WHITE);

		DrawGrid(10, 1.0f);        // Draw a grid

		EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    //UnloadTexture(cubicmap);    // Unload cubicmap texture
    //UnloadTexture(texture);     // Unload map texture
    //UnloadModel(model);         // Unload map model

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


int main()
{
	std::ifstream inputFile("levels/level_1.bos");

	if (!inputFile.is_open()) return 1;


	// VERSION
	// Check header
	getline(inputFile, header, ';');
	if (header != "BOS")
	{
		inputFile.close();
		return 2;
	}

	// Store and print title
	getline(inputFile, version, ';');
	std::cout << "Version: " << version << std::endl;

	// Skip ; and get the next word
	getline(inputFile, tmp);


	// TITLE
	// Check header
	getline(inputFile, header, ';');
	if (header != "TITLE")
	{
		std::cout << "Title format error";
		inputFile.close();
		return 3;
	}

	// Store and print title
	getline(inputFile, title, ';');
	std::cout << "Title: " << title << std::endl;

	// Skip ; and get the next word
	getline(inputFile, tmp);


	// TEXTURES
	// Check header
	getline(inputFile, header, ';');
	if (header != "TEXTURES")
	{
		std::cout << "Texture format error";
		inputFile.close();
		return 4;
	}

	// Store and print textures amount
	getline(inputFile, tmp, ';');
	textureAmount = std::stoi(tmp);
	std::cout << "Texture amount: " << textureAmount << std::endl;

	getline(inputFile, tmp);

	// From file into map
	for (int i = 0; i < textureAmount; i++)
	{
		// Read texture char
		getline(inputFile, tmp, ';');
		char tmpChar = tmp[0];

		// Read texture name an inserts char and name into the map
		getline(inputFile, tmp, ';');
		texturesNamesMap.insert({ tmpChar, tmp });

		// Go to next line
		getline(inputFile, tmp);
	}

	for (auto texturesMapIt : texturesNamesMap)
	{
		std::cout << texturesMapIt.first << " => " << texturesMapIt.second << std::endl;
	}


	// SIZE
	// Check header
	getline(inputFile, header, ';');
	if (header != "SIZE")
	{
		std::cout << "Size format error";
		inputFile.close();
		return 5;
	}

	// Store lvl width
	getline(inputFile, tmp, ';');
	lvlWidth = std::stoi(tmp);

	// Store lvl height
	getline(inputFile, tmp, ';');
	lvlHeight = std::stoi(tmp);

	std::cout << "Width = " << lvlWidth << std::endl;
	std::cout << "Height = " << lvlHeight << std::endl;

	getline(inputFile, tmp);


	// FLOOR
	// Check header
	getline(inputFile, header, ';');
	if (header != "FLOOR")
	{
		std::cout << "Floor format error";
		inputFile.close();
		return 6;
	}
	getline(inputFile, tmp);


	// Create pointer matrix
	floorMap = new char* [lvlHeight];

	for (int i = 0; i < lvlHeight; i++)
	{
		floorMap[i] = new char[lvlWidth];
	}

	// Insert chars in the matrix
	for (int i = 0; i < lvlHeight; i++)
	{
		for (int j = 0; j < lvlWidth; j++)
		{
			getline(inputFile, tmp, ';');
			floorMap[i][j] = tmp[0];
		}
		getline(inputFile, tmp);
	}

	// Print matrix chars
	std::cout << "FLOOR" << std::endl;
	for (int i = 0; i < lvlHeight; i++)
	{
		for (int j = 0; j < lvlWidth; j++)
		{
			std::cout << floorMap[i][j];
		}

		std::cout << std::endl;
	}


	// MAP
	// Check header
	getline(inputFile, header, ';');
	if (header != "MAP")
	{
		std::cout << "Map format error";
		inputFile.close();
		return 7;
	}
	getline(inputFile, tmp);

	// Create pointer matrix
	mapMap = new char* [lvlHeight];

	for (int i = 0; i < lvlHeight; i++)
	{
		mapMap[i] = new char[lvlWidth];
	}

	// Insert chars in the matrix
	for (int i = 0; i < lvlHeight; i++)
	{
		for (int j = 0; j < lvlWidth; j++)
		{
			getline(inputFile, tmp, ';');
			mapMap[i][j] = tmp[0];
		}
		getline(inputFile, tmp);
	}

	// Print matrix chars
	std::cout << "MAP" << std::endl;
	for (int i = 0; i < lvlHeight; i++)
	{
		for (int j = 0; j < lvlWidth; j++)
		{
			std::cout << mapMap[i][j];
		}

		std::cout << std::endl;
	}


	// COLLISIONS
	// Check header
	getline(inputFile, header, ';');
	if (header != "COLLISIONS")
	{
		std::cout << "Collisions format error";
		inputFile.close();
		return 8;
	}
	getline(inputFile, tmp);


	// Create pointer matrix
	collisionsMap = new char* [lvlHeight];

	for (int i = 0; i < lvlHeight; i++)
	{
		collisionsMap[i] = new char[lvlWidth];
	}

	// Insert chars in the matrix
	for (int i = 0; i < lvlHeight; i++)
	{
		for (int j = 0; j < lvlWidth; j++)
		{
			getline(inputFile, tmp, ';');
			collisionsMap[i][j] = tmp[0];
		}
		getline(inputFile, tmp);
	}

	// Print matrix chars
	std::cout << "COLLISIONS" << std::endl;
	for (int i = 0; i < lvlHeight; i++)
	{
		for (int j = 0; j < lvlWidth; j++)
		{
			std::cout << collisionsMap[i][j];
		}

		std::cout << std::endl;
	}


	// OBJECTS
	// Check header
	getline(inputFile, header, ';');
	if (header != "OBJECTS")
	{
		std::cout << "Objects format error";
		inputFile.close();
		return 9;
	}
	getline(inputFile, tmp);


	// Create pointer matrix


	objectsMap = new char* [lvlHeight];

	for (int i = 0; i < lvlHeight; i++)
	{
		objectsMap[i] = new char[lvlWidth];
	}

	// Insert chars in the matrix
	for (int i = 0; i < lvlHeight; i++)
	{
		for (int j = 0; j < lvlWidth; j++)
		{
			getline(inputFile, tmp, ';');
			objectsMap[i][j] = tmp[0];
		}
		getline(inputFile, tmp);
	}

	// Print matrix chars
	std::cout << "OBJECTS" << std::endl;
	for (int i = 0; i < lvlHeight; i++)
	{
		for (int j = 0; j < lvlWidth; j++)
		{
			std::cout << objectsMap[i][j];
		}

		std::cout << std::endl;
	}

	// Floor reading with vectors
	//std::vector<std::vector<char>> column;

	//for (int i = 0; i < lvlHeight; i++)
	//{
	//	std::vector<char> row;

	//	for (int j = 0; j < lvlWidth; j++)
	//	{
	//		getline(inputFile, tmp, ';');
	//		row.push_back(tmp[0]);
	//	}

	//	column.push_back(row);
	//	getline(inputFile, tmp);
	//}


	//for (int i = 0; i < lvlHeight; i++)
	//{
	//	for (int j = 0; j < lvlWidth; j++)
	//	{
	//		std::cout << column[i][j];
	//	}

	//	std::cout << std::endl;
	//}

	inputFile.close();

	start_raylib();

	return 0;
}

//------------------------------------------------------------------------------------
// Custom Functions Definition
//------------------------------------------------------------------------------------
// Draw cube textured
// NOTE: Cube position is the center position
void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color)
{
	float x = position.x;
	float y = position.y;
	float z = position.z;

	// Set desired texture to be enabled while drawing following vertex data
	rlSetTexture(texture.id);

	// Vertex data transformation can be defined with the commented lines,
	// but in this example we calculate the transformed vertex data directly when calling rlVertex3f()
	//rlPushMatrix();
		// NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
		//rlTranslatef(2.0f, 0.0f, 0.0f);
		//rlRotatef(45, 0, 1, 0);
		//rlScalef(2.0f, 2.0f, 2.0f);

	rlBegin(RL_QUADS);
	rlColor4ub(color.r, color.g, color.b, color.a);
	// Front Face
	rlNormal3f(0.0f, 0.0f, 1.0f);       // Normal Pointing Towards Viewer
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right Of The Texture and Quad
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left Of The Texture and Quad
	// Back Face
	rlNormal3f(0.0f, 0.0f, -1.0f);     // Normal Pointing Away From Viewer
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right Of The Texture and Quad
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Left Of The Texture and Quad
	// Top Face
	rlNormal3f(0.0f, 1.0f, 0.0f);       // Normal Pointing Up
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
	// Bottom Face
	rlNormal3f(0.0f, -1.0f, 0.0f);     // Normal Pointing Down
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Right Of The Texture and Quad
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top Left Of The Texture and Quad
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
	// Right face
	rlNormal3f(1.0f, 0.0f, 0.0f);       // Normal Pointing Right
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right Of The Texture and Quad
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left Of The Texture and Quad
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
	// Left Face
	rlNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left Of The Texture and Quad
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Right Of The Texture and Quad
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
	rlEnd();
	//rlPopMatrix();

	rlSetTexture(0);
}

// Draw cube with texture piece applied to all faces
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color)
{
	float x = position.x;
	float y = position.y;
	float z = position.z;
	float texWidth = (float)texture.width;
	float texHeight = (float)texture.height;

	// Set desired texture to be enabled while drawing following vertex data
	rlSetTexture(texture.id);

	// We calculate the normalized texture coordinates for the desired texture-source-rectangle
	// It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent
	rlBegin(RL_QUADS);
	rlColor4ub(color.r, color.g, color.b, color.a);

	// Front face
	rlNormal3f(0.0f, 0.0f, 1.0f);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);

	// Back face
	rlNormal3f(0.0f, 0.0f, -1.0f);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

	// Top face
	rlNormal3f(0.0f, 1.0f, 0.0f);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

	// Bottom face
	rlNormal3f(0.0f, -1.0f, 0.0f);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

	// Right face
	rlNormal3f(1.0f, 0.0f, 0.0f);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

	// Left face
	rlNormal3f(-1.0f, 0.0f, 0.0f);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);

	rlEnd();

	rlSetTexture(0);
}
