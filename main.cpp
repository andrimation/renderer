#pragma once
#include "olcConsoleGameEngineGL.h"
#include <fstream>
#include <strstream>
#include <algorithm>
#include "3dFunctions.h"

class consoleEngine : public olcConsoleGameEngine
{
public:
// Data

	std::vector<string> meshesToLoad = { "VideoShip.obj","plane.obj"};
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

		

		createRotationZMatrix(matZRot, timeZ, 1);
		matXRot = createRotationXMatrix(timeX);
		matYRot = createRotationYMatrix(timeY);


		for (int i = 0; i < sceneMeshes.size(); i++)
		{
			mesh& meshToProject = sceneMeshes[i];

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

			std::vector<triangle> TrianglesToSort;
					
			for (triangle& tri : meshToProject.meshTriangles)
			{
				
				triangle TriRotZ,TriRotX,TriRotY,TriOffset,TriProj;

				if (i == 0)
				{
					MultiplyMatrixTriangle(matZRot, tri, TriRotZ);
					MultiplyMatrixTriangle(matXRot, TriRotZ, TriRotX);
					MultiplyMatrixTriangle(matYRot, TriRotX, TriRotY);
				}
				else
				{
					TriRotY = tri;
				}

				TriOffset = TriRotY;
				TriOffset.points[0].z = TriRotY.points[0].z + 10;
				TriOffset.points[1].z = TriRotY.points[1].z + 10;
				TriOffset.points[2].z = TriRotY.points[2].z + 10;

				// Camera - triangle dot product
				vec3d triangleLine1 = TriOffset.points[1] - TriOffset.points[0];
				vec3d triangleLine2 = TriOffset.points[2] - TriOffset.points[0];
				// Pamiêtaæ o tym ¿e ¿eby zrobiæ dot product z tego gdzie jest camera, nie wystarczy podaæ punktu gdzie jest kamera,
				// ale nale¿y 
				vec3d camTriangleVector = TriOffset.points[0] - camView.position;	
				vec3d camTriangleVectorNormalized = vec3d::VectorNormalize(camTriangleVector);
				vec3d triangleNormalVector = vec3d::VectorCrossProduct(triangleLine1, triangleLine2);				
				vec3d triangleNormalNormalized = vec3d::VectorNormalize(triangleNormalVector);

				float cameraTriangleDotProduct = vec3d::VectorDotProduct(camTriangleVectorNormalized, triangleNormalNormalized);
				
				if (cameraTriangleDotProduct < 0.0f )
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

					TriProj.middlePoint = TriProj.ComputeTriangleMiddle();

					TrianglesToSort.push_back(TriProj);				
				}
			}
			// Posortowaæ listê trójk¹tów wed³ug odleg³oœci.
			std::sort(TrianglesToSort.begin(), TrianglesToSort.end(), [&](triangle a, triangle b) {
				vec3d vec1 = a.middlePoint - camView.position;
				vec3d vec2 = b.middlePoint - camView.position;
				return a.middlePoint.z < b.middlePoint.z;
				});

			for (triangle& triToDraw : TrianglesToSort)
			{
				FillTriangle(triToDraw.points[0].x, triToDraw.points[0].y,
					triToDraw.points[1].x, triToDraw.points[1].y, triToDraw.points[2].x,
					triToDraw.points[2].y, triToDraw.colorChar, triToDraw.colorAtrr);
				DrawTriangle(triToDraw.points[0].x, triToDraw.points[0].y,
					triToDraw.points[1].x, triToDraw.points[1].y, triToDraw.points[2].x,
					triToDraw.points[2].y, PIXEL_SOLID,FG_BLACK);
			}
		}

		return true;
	}
};

int main()
{

	consoleEngine engineObj;

	if (engineObj.ConstructConsole(512, 512, 4, 4))
	{
		engineObj.Start();
	}

	return 0;
}