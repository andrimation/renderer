#pragma once
#include "olcConsoleGameEngineGL.h"
#include <fstream>
#include <strstream>
#include "3dFunctions.h"

class consoleEngine : public olcConsoleGameEngine
{
public:
// Data

	std::vector<string> meshesToLoad = { "VideoShip.obj" };
	std::vector<mesh> sceneMeshes;
	mat4x4 projectionMatrix;
	camera camView;
	lightSource light;

	float timeZ = 0;
	float timeX = 0;
	float timeY = 0;

// Functions
	consoleEngine()
	{
		m_sAppName = L"Console 3d";
	}

	bool OnUserCreate() override
	{
		LoadMeshes(meshesToLoad, sceneMeshes);	
		createProjectionMatrix(projectionMatrix,ScreenHeight(),ScreenWidth());
		return true;
	}

	bool OnUserUpdate(float FElapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(),PIXEL_SOLID,FG_BLACK);

		mat4x4 matZRot;
		mat4x4 matXRot;
		mat4x4 matYRot;

		if (GetKey(VK_UP).bHeld)
		{
			timeX += 5.0 * FElapsedTime;
		}
		if (GetKey(VK_DOWN).bHeld)
		{
			timeX -= 5.0 * FElapsedTime;
		}

		if (GetKey(VK_RIGHT).bHeld)
		{
			timeY += 5.0 * FElapsedTime;
		}
		if (GetKey(VK_LEFT).bHeld)
		{
			timeY -= 5.0 * FElapsedTime;
		}

		createRotationZMatrix(matZRot, timeZ, 1);
		matXRot = createRotationXMatrix(timeX);
		matYRot = createRotationYMatrix(timeY);

		for (mesh meshToProject : sceneMeshes)
		{

			std::vector<triangle> TrianglesToSort;
					
			for (triangle& tri : meshToProject.meshTriangles)
			{
				
				triangle TriRotZ,TriRotX,TriRotY,TriOffset,TriProj;

				MultiplyMatrixTriangle(matZRot, tri, TriRotZ);
				MultiplyMatrixTriangle(matXRot, TriRotZ, TriRotX);
				MultiplyMatrixTriangle(matYRot, TriRotX, TriRotY);

				TriOffset = TriRotY;
				TriOffset.points[0].z = TriRotY.points[0].z + 10;
				TriOffset.points[1].z = TriRotY.points[1].z + 10;
				TriOffset.points[2].z = TriRotY.points[2].z + 10;

				// Camera - triangle dot product
				vec3d triangleLine1 = TriOffset.points[1] - TriOffset.points[0];
				vec3d triangleLine2 = TriOffset.points[2] - TriOffset.points[0];
				// Pami�ta� o tym �e �eby zrobi� dot product z tego gdzie jest camera, nie wystarczy poda� punktu gdzie jest kamera,
				// ale nale�y 
				vec3d camTriangleVector = TriOffset.points[0] - camView.position;	
				vec3d camTriangleVectorNormalized = vec3d::VectorNormalize(camTriangleVector);
				vec3d triangleNormalVector = vec3d::VectorCrossProduct(triangleLine1, triangleLine2);				
				vec3d triangleNormalNormalized = vec3d::VectorNormalize(triangleNormalVector);

				float cameraTriangleDotProduct = vec3d::VectorDotProduct(camTriangleVectorNormalized, triangleNormalNormalized);
				
				if (cameraTriangleDotProduct < 0.0f)
				{
					vec3d lightVectorNormalized = vec3d::VectorNormalize(light.dir);
					float triLightDotProduct = vec3d::VectorDotProduct(triangleNormalNormalized, lightVectorNormalized);

					CHAR_INFO color = GetColour(triLightDotProduct);

					MultiplyMatrixTriangle(projectionMatrix, TriOffset, TriProj);

					TriProj.points[0].x += 1.0f;
					TriProj.points[0].y += 1.0f;
					TriProj.points[1].x += 1.0f;
					TriProj.points[1].y += 1.0f;
					TriProj.points[2].x += 1.0f;
					TriProj.points[2].y += 1.0f;

					TriProj.points[0].x *= (float)ScreenWidth() * 0.5f;
					TriProj.points[0].y *= (float)ScreenHeight() * 0.5f;
					TriProj.points[1].x *= (float)ScreenWidth() * 0.5f;
					TriProj.points[1].y *= (float)ScreenHeight() * 0.5f;
					TriProj.points[2].x *= (float)ScreenWidth() * 0.5f;
					TriProj.points[2].y *= (float)ScreenHeight() * 0.5f;

					TriProj.colorChar = color.Char.UnicodeChar;
					TriProj.colorAtrr = color.Attributes;

					TrianglesToSort.push_back(TriProj);

					FillTriangle(TriProj.points[0].x, TriProj.points[0].y,
						TriProj.points[1].x, TriProj.points[1].y, TriProj.points[2].x,
						TriProj.points[2].y,color.Char.UnicodeChar,color.Attributes);
				}
			}
			// Posortowa� list� tr�jk�t�w wed�ug odleg�o�ci. 
		}

		return true;
	}
};

int main()
{

	consoleEngine engineObj;

	if (engineObj.ConstructConsole(256, 240, 4, 4))
	{
		engineObj.Start();
	}

	return 0;
}