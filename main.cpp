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

	std::vector<string> meshesToLoad = { "plane.obj","VideoShip2.obj" };
	std::vector<mesh> sceneMeshes;
	mat4x4 projectionMatrix;
	camera camView;
	lightSource light;

	bool ifRenderImage = false;

	float timeZ = 0;
	float timeX = 0;
	float timeY = 0;

	RayTracer renderer{ camView };
	
// Functions
	consoleEngine()
	{
		m_sAppName = L"Console 3d";
	}

	bool OnUserCreate() override
	{
		LoadMeshes(meshesToLoad, sceneMeshes);	
		createProjectionMatrix(projectionMatrix,ScreenHeight(),ScreenWidth());
		renderer.imageWidth = (int)ScreenWidth();
		renderer.imageHeight = (int)ScreenHeight();
		return true;
	}

	bool OnUserUpdate(float FElapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(),PIXEL_SOLID,FG_BLACK);

		mat4x4 matZRot;
		mat4x4 matXRot;
		mat4x4 matYRot;

		matZRot = createRotationZMatrix(timeZ,1);
		matXRot = createRotationXMatrix(timeX);
		matYRot = createRotationYMatrix(timeY);

		std::vector<triangle> TrianglesToRayTrace;

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
			if (GetKey(VK_SPACE).bHeld)
			{
				ifRenderImage = true;
			}

			std::vector<triangle> TrianglesToSort;
					
			for (triangle& tri : meshToProject.meshTriangles)
			{
				triangle TriRotZ,TriRotX,TriRotY,TriOffset,TriProj;

				if (i == 1)
				{
					//MultiplyMatrixTriangle(matZRot, tri, TriRotZ);
					//MultiplyMatrixTriangle(matXRot, tri, TriRotX);
					MultiplyMatrixTriangle(matYRot, tri, TriRotY);
				}
				else
				{
					//MultiplyMatrixTriangle(matZRot, tri, TriRotZ);
					//MultiplyMatrixTriangle(matXRot, TriRotZ, TriRotX);
					//MultiplyMatrixTriangle(matYRot, TriRotX, TriRotY);

					TriRotY = tri;
				}

				TriOffset = TriRotY;
				TriOffset.points[0].z = TriRotY.points[0].z + 6;
				TriOffset.points[1].z = TriRotY.points[1].z + 6;
				TriOffset.points[2].z = TriRotY.points[2].z + 6;

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
						
				vec3d lightVectorNormalized = vec3d::VectorNormalize(light.dir);
				float triLightDotProduct = vec3d::VectorDotProduct(triangleNormalNormalized, lightVectorNormalized);

				CHAR_INFO color = GetColour(triLightDotProduct);

				MultiplyMatrixTriangle(projectionMatrix, TriOffset, TriProj);
				//TriProj = TriOffset;

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

				

				// Ok czyli wiem �e problem jest gdzie� tu. Je�li daj� do renderowania TriOffset - a wi�c bez matrycy projekcji,
				// to wtedy renderuje si� poprawnie
				
				triangle Xtri = TriOffset;

				Xtri.points[0].x += 1.0f;
				Xtri.points[0].y += 1.0f;
				Xtri.points[1].x += 1.0f;
				Xtri.points[1].y += 1.0f;
				Xtri.points[2].x += 1.0f;
				Xtri.points[2].y += 1.0f;

				Xtri.points[0].x *= (float)ScreenWidth() * 0.5f;
				Xtri.points[0].y *= (float)ScreenHeight() * 0.5f;
				Xtri.points[1].x *= (float)ScreenWidth() * 0.5f;
				Xtri.points[1].y *= (float)ScreenHeight() * 0.5f;
				Xtri.points[2].x *= (float)ScreenWidth() * 0.5f;
				Xtri.points[2].y *= (float)ScreenHeight() * 0.5f;

				TriProj.colorChar = color.Char.UnicodeChar;
				TriProj.colorAtrr = color.Attributes;
				TriProj.triangleNormal = triangleNormalNormalized;

				TriProj.middlePoint = TriProj.ComputeTriangleMiddle();

				if (i == 1) { TrianglesToRayTrace.push_back(TriOffset); }
				

				if (cameraTriangleDotProduct < 0.0f)
				{
					if (i == 1)
					{
						TrianglesToSort.push_back(TriProj);
						
					}
					else
					{
						FillTriangle(TriProj.points[0].x, TriProj.points[0].y,
							TriProj.points[1].x, TriProj.points[1].y, TriProj.points[2].x,
							TriProj.points[2].y, PIXEL_SOLID, FG_GREY);
						DrawTriangle(TriProj.points[0].x, TriProj.points[0].y,
							TriProj.points[1].x, TriProj.points[1].y, TriProj.points[2].x,
							TriProj.points[2].y, PIXEL_SOLID, FG_BLACK);
						
					}
					
				}

				
			}
			// Posortowa� list� tr�jk�t�w wed�ug odleg�o�ci.
			std::sort(TrianglesToSort.begin(), TrianglesToSort.end(), [&](triangle a, triangle b) {
				vec3d vec1 = a.middlePoint - camView.position;
				vec3d vec2 = b.middlePoint - camView.position;
				return a.middlePoint.z < b.middlePoint.z;
				});

			std::sort(TrianglesToRayTrace.begin(), TrianglesToRayTrace.end(), [&](triangle a, triangle b) {
				vec3d vec1 = a.middlePoint - camView.position;
				vec3d vec2 = b.middlePoint - camView.position;
				return a.middlePoint.z < b.middlePoint.z;
				});

			for (triangle& triToDraw : TrianglesToSort)
			{
				FillTriangle(triToDraw.points[0].x, triToDraw.points[0].y,
					triToDraw.points[1].x, triToDraw.points[1].y, triToDraw.points[2].x,
					triToDraw.points[2].y, triToDraw.colorChar, triToDraw.colorAtrr);
				/*DrawTriangle(triToDraw.points[0].x, triToDraw.points[0].y,
					triToDraw.points[1].x, triToDraw.points[1].y, triToDraw.points[2].x,
					triToDraw.points[2].y, PIXEL_SOLID,FG_WHITE);*/
			}

			if (ifRenderImage)
			{			
				renderer.RenderToFile("plik.ppm", TrianglesToRayTrace);
				ifRenderImage = false;
			}
		}

		return true;
	}
};

int main()
{

	consoleEngine engineObj;

	if (engineObj.ConstructConsole(256, 256, 4, 4))
	{
		engineObj.Start();

	}

	return 0;
}