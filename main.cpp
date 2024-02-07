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

	std::vector<string> meshesToLoad = { "VideoShip2.obj" };
	std::vector<mesh> sceneMeshes;
	mat4x4 projectionMatrix;
	camera camView;
	lightSource light;

	bool ifRenderImage = false;

	float timeZ = 0;
	float timeX = 0;
	float timeY = 0;

	RayTracer renderer{ camView };

	int numOfThreads = 32;  // 4 albo 16 :P
	std::vector<thread> RenderThreads;
	
	
// Functions
	consoleEngine()
	{
		m_sAppName = L"Console 3d";
	}

	bool OnUserCreate() override
	{
		LoadMeshes(meshesToLoad, sceneMeshes);	
		createProjectionMatrix(projectionMatrix,ScreenHeight(),ScreenWidth());
		renderer.imageWidth = (int)ScreenWidth()*8;
		renderer.imageHeight = (int)ScreenHeight()*8;
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

				if (i == 0)
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
				// Pamiêtaæ o tym ¿e ¿eby zrobiæ dot product z tego gdzie jest camera, nie wystarczy podaæ punktu gdzie jest kamera,
				// ale nale¿y 
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
			
				// Ok czyli wiem ¿e problem jest gdzieœ tu. Jeœli dajê do renderowania TriOffset - a wiêc bez matrycy projekcji,
				// to wtedy renderuje siê poprawnie

				TriProj.colorChar = color.Char.UnicodeChar;
				TriProj.colorAtrr = color.Attributes;
				TriProj.triangleNormal = triangleNormalNormalized;

				TriProj.middlePoint = TriProj.ComputeTriangleMiddle();

				if (i == 1) {  }
				

				if (cameraTriangleDotProduct < 0.0f)
				{
					if (i == 0)
					{
						TrianglesToSort.push_back(TriProj);
						TrianglesToRayTrace.push_back(TriOffset);
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
			// Posortowaæ listê trójk¹tów wed³ug odleg³oœci.
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
		}
		if (ifRenderImage )
		{
			//renderer.RenderThreadToArray(TrianglesToRayTrace);
			//renderer.RenderToFile("plik.ppm", TrianglesToRayTrace);
			RenderInThreads(TrianglesToRayTrace,renderer.ThreadsResults);
			renderer.SaveRenderResultToFile("plik.ppm");
			ifRenderImage = false;
		}

		return true;
	}

	void RenderInThreads(std::vector<triangle> trianglesToCheck,std::vector<std::vector<vec3d>>& ThreadsResults)
	{
		// Tu jest b³ad w obliczeniu bo bêdê renderowa³ po skosie !
		ThreadsResults.resize(numOfThreads);
		int threadID = 0;
		int threadAddNum = ((int)ScreenHeight()*8) / numOfThreads;
		std::vector<int> iterationPixelsNums;

		int addNum = (- (int)ScreenHeight() * 8) / 2;
		iterationPixelsNums.push_back(addNum);
		for (int i = 0; i < numOfThreads; i++)
		{
			addNum += threadAddNum;
			iterationPixelsNums.push_back(addNum);
		}

		for (int i = 0; i < numOfThreads; i++)
		{
			int aU = iterationPixelsNums[i];
			int bU = iterationPixelsNums[i + 1];

			std::cout <<"ThreadU range: " << aU << "  " << bU << " \n";
			std::thread thread_obj(&RayTracer::RenderChunkInThread, &renderer, trianglesToCheck, aU, bU,threadID);
			RenderThreads.push_back(move(thread_obj));
			threadID++;
			
			std::cout << threadID ;
			std::cout << "    \n\n";
		}

		for (thread& Thr : RenderThreads)
		{
			Thr.join();
		}
	}
};

int main()
{

	consoleEngine engineObj;

	if (engineObj.ConstructConsole(128, 128, 4, 4))
	{
		engineObj.Start();

	}

	return 0;
}