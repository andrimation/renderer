#pragma once
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <thread>


inline float random()
{
	return rand() / (RAND_MAX + 0.1);
}

inline float random(float min, float max)
{
	return min + (max - min) * random();
}

struct vec3d
{
	float x, y, z, w;

	//vec3d() {};
	//vec3d(float xin, float yin, float zin) : x(xin), y(yin), z(zin) {};
	//vec3d(float xin, float yin, float zin, float win) : x(xin), y(yin), z(zin), w(win) {};
	//~vec3d() {};

	vec3d operator + (const vec3d& vOther) const
	{
		vec3d newVec;
		newVec.x = this->x + vOther.x;
		newVec.y = this->y + vOther.y;
		newVec.z = this->z + vOther.z;
		return newVec;
	}

	vec3d operator + (const float f)
	{
		vec3d newVec;
		newVec.x = this->x + f;
		newVec.y = this->y + f;
		newVec.z = this->z + f;
		return newVec;
	}

	vec3d operator += (const vec3d& vOther)
	{
		this->x += vOther.x;
		this->y += vOther.y;
		this->z += vOther.z;
		return *this;
	}

	vec3d operator += (const float& vOther)
	{
		this->x += vOther;
		this->y += vOther;
		this->z += vOther;
		return *this;
	}

	vec3d operator -= (const vec3d& vOther)
	{
		this->x -= vOther.x;
		this->y -= vOther.y;
		this->z -= vOther.z;
		return *this;
	}

	vec3d operator -= (const float& vOther)
	{
		this->x -= vOther;
		this->y -= vOther;
		this->z -= vOther;
		return *this;
	}
	

	vec3d operator - ()
	{
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
		return *this;
	}
	
	vec3d operator - (const vec3d& vOther)
	{
		vec3d newVec;
		newVec.x = this->x - vOther.x;
		newVec.y = this->y - vOther.y;
		newVec.z = this->z - vOther.z;
		return newVec;
	}

	vec3d operator - (const float f)
	{
		vec3d newVec;
		newVec.x = this->x - f;
		newVec.y = this->y - f;
		newVec.z = this->z - f;
		return newVec;
	}

	vec3d operator * (const vec3d& vOther) const
	{
		vec3d newVec;
		newVec.x = this->x * vOther.x;
		newVec.y = this->y * vOther.y;
		newVec.z = this->z * vOther.z;
		return newVec;
	}

	vec3d operator * (const float val) const
	{
		vec3d newVec;
		newVec.x = this->x * val;
		newVec.y = this->y * val;
		newVec.z = this->z * val;
		return newVec;
	}

	vec3d operator / (const float divide)
	{
		vec3d newVec;
		newVec.x = this->x / divide;
		newVec.y = this->y / divide;
		newVec.z = this->z / divide;
		return newVec;
	}

	vec3d operator /= (const float divide)
	{
		this->x /= divide;
		this->y /= divide;
		this->z /= divide;
		return *this;
	}

	bool operator == (const vec3d& other)
	{
		return (this->x == other.x && this->y == other.y && this->z == other.z);
	}

	static float VectorDotProduct(vec3d& vec1, vec3d& vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	}

	static float VectorLenght(vec3d& v1)
	{
		return sqrtf(VectorDotProduct(v1, v1));
	}

	float VectorLenght_squared() const
	{
		return this->x * this->x + this->y * this->y + this->z * this->z;
	}

	static vec3d VectorNormalize(vec3d& vec1)
	{
		float lenght = VectorLenght(vec1);
		vec3d newVector;
		newVector.x = vec1.x / lenght;
		newVector.y = vec1.y / lenght;
		newVector.z = vec1.z / lenght;
		return newVector;
	}

	static vec3d VectorCrossProduct(vec3d& vec1, vec3d& vec2)
	{
		vec3d newVector;
		newVector.x = vec1.y * vec2.z - vec1.z * vec2.y;
		newVector.y = vec1.z * vec2.x - vec1.x * vec2.z;
		newVector.z = vec1.x * vec2.y - vec1.y * vec2.x;

		return newVector;
	}

	static vec3d createRandomVector()
	{
		vec3d newVector{ random(), random(), random() };
		return newVector;
	};

	static vec3d createRandomVector(float min, float max)
	{
		vec3d newVector{ random(min,max), random(min,max), random(min,max) };
		return newVector;
	};
};

struct triangle
{
	vec3d points[3];
	vec3d middlePoint;
	vec3d triangleNormal;

	WCHAR colorChar;
	WORD colorAtrr;

	// �eby posortowa�?
	vec3d ComputeTriangleMiddle()
	{
		vec3d newVector = (points[0] + points[1] + points[2]) / 3;
		return newVector;
	}
};

struct mesh
{
	std::vector<triangle> meshTriangles;
};

struct mat4x4
{
	float m[4][4] = {0};
};

class lightSource
{
public:
	vec3d dir = { 0.0f,-0.5f,-1.0f,0.0f };
};

void LoadMeshes(std::vector<string>& meshesToLoad,std::vector<mesh>& sceneMeshes)
{
	for (string& meshFileName : meshesToLoad)
	{
		mesh tempMesh;

		ifstream meshFile(meshFileName);
		if (!meshFile.is_open())
		{
			continue;
		}

		std::vector<vec3d> vertices;

		while (!meshFile.eof())
		{
			char line[128];
			meshFile.getline(line, 128);

			strstream string;
			string << line;

			char junkChar;

			if (line[0] == 'v')
			{
				vec3d vertex;
				string >> junkChar >> vertex.x >> vertex.y >> vertex.z;
				vertices.push_back(vertex);
			}

			if (line[0] == 'f')
			{
				triangle tri;

				int vectorIds[3];

				string >> junkChar >> vectorIds[0] >> vectorIds[1] >> vectorIds[2];

				tri.points[0] = vertices[vectorIds[0] - 1];
				tri.points[1] = vertices[vectorIds[1] - 1];
				tri.points[2] = vertices[vectorIds[2] - 1];
				tempMesh.meshTriangles.push_back(tri);
			}
		}
		sceneMeshes.push_back(tempMesh);
	}
}

void MultiplyMatrixVertex(mat4x4& matrix, vec3d& inVec,vec3d& outVec)
{
	outVec.x = inVec.x * matrix.m[0][0] + inVec.y * matrix.m[1][0] + inVec.z * matrix.m[2][0] + matrix.m[3][0];
	outVec.y = inVec.x * matrix.m[0][1] + inVec.y * matrix.m[1][1] + inVec.z * matrix.m[2][1] + matrix.m[3][1];
	outVec.z = inVec.x * matrix.m[0][2] + inVec.y * matrix.m[1][2] + inVec.z * matrix.m[2][2] + matrix.m[3][2];

	float w = inVec.x * matrix.m[0][3] + inVec.y * matrix.m[1][3] + inVec.z * matrix.m[2][3] + matrix.m[3][3];

	if (w != 0.0f)
	{
		outVec.x = outVec.x / w;
		outVec.y = outVec.y / w;
		outVec.z = outVec.z / w;
	}
}

void MultiplyMatrixTriangle(mat4x4& matrix, triangle& inTriangle, triangle& outTriangle)
{
	MultiplyMatrixVertex(matrix, inTriangle.points[0], outTriangle.points[0]);
	MultiplyMatrixVertex(matrix, inTriangle.points[1], outTriangle.points[1]);
	MultiplyMatrixVertex(matrix, inTriangle.points[2], outTriangle.points[2]);
}
 
void createProjectionMatrix(mat4x4& matrix,int screenHeight,int screenWidth)
{
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	float fov = 90.0f;
	float aspectRatio = (float)screenHeight / (float)screenWidth;
	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);

	matrix.m[0][0] = aspectRatio * fovRad;
	matrix.m[1][1] = fovRad;
	matrix.m[2][2] = farPlane / (farPlane - nearPlane);
	matrix.m[3][2] = (farPlane * nearPlane) / (farPlane - nearPlane);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
}

mat4x4 createRotationZMatrix(float time,float dir)
{
	mat4x4 matrix;
	float multiplier = time * dir;
	matrix.m[0][0] = cosf(multiplier);
	matrix.m[0][1] = sinf(multiplier);
	matrix.m[1][0] = -sinf(multiplier);
	matrix.m[1][1] = cosf(multiplier);
	matrix.m[2][2] = 1.0;
	matrix.m[3][3] = 1.0;
	return matrix;
}

mat4x4 createRotationXMatrix(float angle)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosf(angle);
	matrix.m[1][2] = sinf(angle);
	matrix.m[2][1] = -sinf(angle);
	matrix.m[2][2] = cosf(angle);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 createRotationYMatrix(float angle)
{
	mat4x4 matrix;
	matrix.m[0][0] = cosf(angle);
	matrix.m[2][0] = -sinf(angle);
	matrix.m[1][1] = 1.0f;
	matrix.m[0][2] = sinf(angle);
	matrix.m[2][2] = cosf(angle);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

// Utilities
inline static std::ostream& operator << (std::ostream& out, const vec3d v)
{
	return out << v.x << " " << v.y << " " << v.z << "\n";
}

// Funkcja powi�zana z console engine
CHAR_INFO GetColour(float lum)
{
	short bg_col, fg_col;
	wchar_t sym;
	int pixel_bw = (int)(13.0f * lum);
	switch (pixel_bw)
	{
	case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

	case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
	case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
	case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
	case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

	case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
	case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
	case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
	case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

	case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
	case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
	case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
	case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
	default:
		bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
	}

	CHAR_INFO c;
	c.Attributes = bg_col | fg_col;
	c.Char.UnicodeChar = sym;
	return c;
}


// RayTracer 

class ray
{
public:
	ray() {}
	ray(vec3d& origin, vec3d& direction) : orig(origin), dir(direction) {}
	~ray() {};

	vec3d orig;
	vec3d dir;
	vec3d initialOrig;
	vec3d initialDir;
	triangle* hitTriangle;
	float hitDistance;

	vec3d at(float t) const 
	{ 
		return  (dir * t) + orig; 
	}
};

class camera
{
public:
	float focalLength = 1.0;
	vec3d position = { 0.0f,0.0f,0.0f,0.0f };
};

class RayTracer
{
public:
	RayTracer(camera camIn) : cam(camIn), imageWidth(1), imageHeight(1) {};
	~RayTracer() {};

	float imageWidth;
	int imageHeight;
	camera cam;

	float viewportHeight = 0.00165f;
	float viewportWidth  = 0.00165f; 

	vec3d viewportU = { viewportWidth,0.0,0.0 };
	vec3d viewportV = { 0.0,viewportHeight,0.0 };

	vec3d pixelDeltaU = viewportU / imageWidth;
	vec3d pixelDeltaV = viewportV / imageHeight;

	vec3d focalLenght{ 0.0,0.0,cam.focalLength };

	vec3d viewportTopLeft = cam.position + focalLenght - (viewportU / 2) - (viewportV / 2);
	vec3d pixel00 = viewportTopLeft + pixelDeltaU / 2 + pixelDeltaV / 2;

	int antyAliasingSamples = 5;

	std::vector<std::vector<vec3d>> ThreadsResults;

	void RenderToFile(std::string fileName, std::vector<triangle> trianglesToCheck)
	{

		for (int j = -imageHeight*0.5; j < imageHeight*0.5; j++)
		{
			for (int i = -imageWidth*0.5; i < imageWidth*0.5; i++)
			{	
				vec3d pixelCol{ 0.0,0.0,0.0 };
				vec3d nextPixel = pixel00 + (pixelDeltaU * i) + (pixelDeltaV * j);
				vec3d rayDir = (nextPixel - cam.position);
				ray r(cam.position, rayDir);

				int check = 0;

				std::vector<vec3d> colorsToBlend; 

				for (int sample = 0; sample < antyAliasingSamples; sample++)
				{
					vec3d random = addRandomness();
					rayDir = (nextPixel - cam.position) + random;
					r.dir = rayDir;

					// Zrobi� vector kolor�w. i ka�dy trafia po 1 razie. Ale bez sensu, bo np tam gdzie jest pe�ny obszar "trafie�" to i tak mi b�dzie
					// dodawa� kolor t�a, 
					// Ok zrobi� vector. dla ka�dego raya dodajemy kolor kt�ry trafi, ale dla raya, nie dla tr�jk�ta. 					
					for (triangle& triToTest : trianglesToCheck)
					{
						if (rayHitTriangle(triToTest, r))
						{
							vec3d hitColor = { 1.0,1.0,1.0 };
							colorsToBlend.push_back(hitColor);
							break;
						}
					}
				}

				// Dlaczego wychodzi szary w �rodku a czarny na bokach ? 

				//Sprawdzam w wektorze kolor�w ile by�o trafie� i r�nic� uzupe�niam kolorem t�a 
				// ( im mniej trafie� tym pi*oko minejsz� cz�� zajmuje tr�jk�t w pikselu )
				int trianglesHits = colorsToBlend.size();
				std::cout << trianglesHits << "\n";
				// Uzupe�niam r�nic� kolorem t�a
				r.dir = (nextPixel - cam.position);
				vec3d backgroundColor = computeRayColor(r);
				
				// B��d jest na pewno tu ?
				for (int missedRays = 0; missedRays < antyAliasingSamples-trianglesHits; missedRays++)
				{
					colorsToBlend.push_back(backgroundColor);
				}

				vec3d combinedColors{ 0.0,0.0,0.0 };
				for (vec3d& colorToAdd : colorsToBlend)
				{
					combinedColors += colorToAdd;
				}
				
				pixelCol = combinedColors / colorsToBlend.size();
			}
		}
		
	}

	void RenderChunkInThread(std::vector<triangle> trianglesToCheck,int startPixelU,int endPixelU,int threadId)
	{
		std::vector<vec3d> resultColors;

		for (int j = startPixelU; j < endPixelU; j++)
		{
			for (int i = -imageWidth * 0.5; i < imageWidth * 0.5; i++)
			{
				vec3d pixelCol{ 0.0,0.0,0.0 };
				vec3d nextPixel = pixel00 + (pixelDeltaU * i) + (pixelDeltaV * j);
				vec3d rayDir = (nextPixel - cam.position);
				ray r(cam.position, rayDir);

				std::vector<vec3d> colorsToBlend;

				for (int sample = 0; sample < antyAliasingSamples; sample++)
				{
					vec3d random = addRandomness();
					rayDir = (nextPixel - cam.position) + random;
					rayDir = vec3d::VectorNormalize(rayDir);
					r.dir = rayDir;
					r.initialDir = rayDir;
					r.initialOrig = r.orig;

					int iteration = 0;
					vec3d pixelToFill{ 1.0,1.0,1.0 };
					RayTrace(trianglesToCheck, r, iteration, pixelToFill);
					colorsToBlend.push_back(pixelToFill);
				}
				//Sprawdzam w wektorze kolor�w ile by�o trafie� i r�nic� uzupe�niam kolorem t�a 
				// ( im mniej trafie� tym pi*oko minejsz� cz�� zajmuje tr�jk�t w pikselu )
				int trianglesHits = colorsToBlend.size();
				//std::cout << trianglesHits << "\n";
				// Uzupe�niam r�nic� kolorem t�a
				r.dir = (nextPixel - cam.position);
				r.orig = r.initialOrig;
				vec3d backgroundColor = computeRayColor(r);

				for (int missedRays = 0; missedRays < antyAliasingSamples - trianglesHits; missedRays++)
				{
					colorsToBlend.push_back(backgroundColor);
				}

				vec3d combinedColors{ 0.0,0.0,0.0 };
				for (vec3d& colorToAdd : colorsToBlend)
				{
					combinedColors += colorToAdd;
				}

				pixelCol = combinedColors / colorsToBlend.size();
				resultColors.push_back(pixelCol);
			}
		}
		ThreadsResults[threadId] = resultColors;
	}

	void writeColor(std::ostream& out, vec3d pixelColor)
	{
		out << static_cast<int>(255.999 * pixelColor.x) << " "
			<< static_cast<int>(255.999 * pixelColor.y) << " "
			<< static_cast<int>(255.999 * pixelColor.z) << "\n";
	}

	void SaveRenderResultToFile(std::string fileName)
	{
		std::ofstream renderResultFile(fileName, std::ios::binary);
		renderResultFile << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

		for (std::vector<vec3d>& threadResult : ThreadsResults)
		{
			for (vec3d& pixelCol : threadResult)
			{
				writeColor(renderResultFile, pixelCol);
			}
		}
		renderResultFile.close();
	}

	float clamp(float val, float min, float max)
	{
		if (val < min)
		{
			return min;
		}
		else if (val > max)
		{
			return max;
		}
		else
		{
			return val;
		}
	}

	vec3d computeRayColor( ray& r )
	{		
		// Gradient t�a
		vec3d direction = r.initialDir;
		vec3d normalized_ray = vec3d::VectorNormalize(direction);
		auto a = 0.5 * ((normalized_ray.y) + 1.0);

		vec3d colorA{ 1.0,1.0,1.0 };
		vec3d colorB{ 0.5,0.7,1.0 };

		return colorA * (1.0 - a) + colorB * a;
		//return vec3d{ 1.0,1.0,1.0 };	
	}

	vec3d addRandomness()
	{
		float x =  0.5 + random();
		float y =  0.5 + random();

		return (pixelDeltaU * x) + (pixelDeltaV * y);
	}

	bool rayHitTriangle(triangle& triToTest, ray& r)
	{
		// 1) Najpierw liczymy czy promie� przecina p�aszczyzn� wyznaczon� przez normal i puntk na p�aszczy�nie ( wierzcho�ek tr�jk�ta )
		// punkt przeci�cia promienia i p�aszczyzny oblicz� po prostu przez wsadzienie wyniku w ray.at()
		// 2) Obliczamy czy proime� trafi� w tr�jk�t -> iterujemy przez "wektory-boki" tr�jk�ta i liczymy CrossProduct z punktu przeci�cia i aktualnego boku
		// 3) P�niej liczymy dotproduct z uzyskanego w pkt 2) nowego wektora i normala tr�jk�ta i je�li wynik jest >= 0 to ray wpada w tr�jk�t
		
		vec3d triLine1 = triToTest.points[1] - triToTest.points[0];
		vec3d triLine2 = triToTest.points[2] - triToTest.points[0];

		vec3d normal = vec3d::VectorCrossProduct(triLine1, triLine2);
		normal = vec3d::VectorNormalize(normal);
		triToTest.triangleNormal = normal;

		float NdotRayDirection = vec3d::VectorDotProduct(triToTest.triangleNormal, r.dir);

		if (NdotRayDirection >= 0.0)
		{
			return false;
		}

		vec3d minNorm = triToTest.triangleNormal * -1;

		float d = vec3d::VectorDotProduct(triToTest.triangleNormal, triToTest.points[0]);
		float t = -((vec3d::VectorDotProduct(triToTest.triangleNormal, r.orig) - d) / NdotRayDirection);

		if (t <= 0)
		{
			return false;
		}

		vec3d P = r.orig + (r.dir * t);

		vec3d edge1 = triToTest.points[1] - triToTest.points[0];
		vec3d interscectionVec1 = P - triToTest.points[0];
		// Najpierw cross product. Jako ze P jest na p�aszczy�nie tr�jk�ta, to cross product mo�e mie� tylko dwa zwroty "do" albo "od" p�aszczyzny,
		// w zale�no�ci od po�o�enia P wzgl�dem edge1. To pozwala na sprawdzenie p�niej dot productu.
		vec3d test1 = vec3d::VectorCrossProduct(edge1, interscectionVec1);
		float testDot1 = vec3d::VectorDotProduct(test1, triToTest.triangleNormal);
		// szybciej sprawdza� tu ni� na ko�cu.
		if (testDot1 < 0.0)
		{
			return false;
		}

		vec3d edge2 = triToTest.points[2] - triToTest.points[1];
		vec3d interscectionVec2 = P - triToTest.points[1];
		vec3d test2 = vec3d::VectorCrossProduct(edge2, interscectionVec2);
		float testDot2 = vec3d::VectorDotProduct(test2, triToTest.triangleNormal);
		if (testDot2 < 0.0)
		{
			return false;
		}

		vec3d edge3 = triToTest.points[0] - triToTest.points[2];
		vec3d interscectionVec3 = P - triToTest.points[2];
		vec3d test3 = vec3d::VectorCrossProduct(edge3, interscectionVec3);
		float testDot3 = vec3d::VectorDotProduct(test3, triToTest.triangleNormal);
		if (testDot3 < 0.0)
		{
			return false;
		}

		r.orig = P;
		return true; 
	}

	void RayTrace(std::vector<triangle>& trianglesToCheck, ray& r, int& iteration, vec3d& outColor)
	{
		if (iteration < 6)
		{	
			
			std::vector<ray> hitPoints;
			for (triangle& triToCheck : trianglesToCheck)
			{
				// W tym miejscu, musz� zebra� wszystkie trafienia promienia do vectora, a nast�pnie wybra� trafienie o najmniejszej warto�ci Z
				// i dopiero to trafienie odbi� !! bo to s� wszystkie tr�jk�ty w jednym pikselu !!
				if (rayHitTriangle(triToCheck, r))
				{
					ray rayWithHit = r;
					rayWithHit.dir = r.dir;
					rayWithHit.initialDir = r.initialDir;
					rayWithHit.initialOrig = r.initialOrig;
					rayWithHit.hitTriangle = &triToCheck;
					vec3d triangleCamVec = rayWithHit.orig - cam.position;
					rayWithHit.hitDistance = vec3d::VectorLenght(triangleCamVec);
					hitPoints.push_back(rayWithHit);
				}			
			}

			if (hitPoints.size() > 0)
			{
				std::sort(hitPoints.begin(), hitPoints.end(), [&](ray a, ray b) {
					return a.hitDistance < b.hitDistance;
					});

				ray& rayHitClosestTriangle = hitPoints[0];
				triangle& triToCheck = *rayHitClosestTriangle.hitTriangle;
				int counter = 0;
				bool randomVectorMatches = false;

				while (!randomVectorMatches)
				{
					vec3d triLine1 = triToCheck.points[1] - triToCheck.points[0];
					vec3d triLine2 = triToCheck.points[2] - triToCheck.points[0];
					vec3d normal = vec3d::VectorCrossProduct(triLine1, triLine2);
					if (vec3d::VectorLenght(normal) > 0)
					{
						normal = vec3d::VectorNormalize(normal);
					}
						
					vec3d randomVector = vec3d::createRandomVector(-1.0, 1.0);
					float randomNormalDot = vec3d::VectorDotProduct(normal, randomVector);
						
					if (randomNormalDot > 0.0)
					{
						r.dir = randomVector;
						randomVectorMatches = true;					
					}
						
					counter++;
						
					if (counter == 50)
					{
						goto end;
					}					
				}
				iteration++;
				outColor -= 0.35;
				RayTrace(trianglesToCheck, r, iteration, outColor);
			}
			else
			end:
			{
				if (iteration == 0)
				{
					//outColor += computeRayColor(r);
				}
				else
				{
					//outColor -= computeRayColor(r)/4;
					outColor = outColor / iteration;
				}



			}
		}
		
	}
};

