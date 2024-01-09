#pragma once

struct vec3d
{
	float x, y, z, w;

	vec3d operator + (const vec3d& vOther)
	{
		vec3d newVec;
		newVec.x = this->x + vOther.x;
		newVec.y = this->y + vOther.y;
		newVec.z = this->z + vOther.z;
		return newVec;
	}
	
	vec3d operator - (const vec3d& vOther)
	{
		vec3d newVec;
		newVec.x = this->x - vOther.x;
		newVec.y = this->y - vOther.y;
		newVec.z = this->z - vOther.z;
		return newVec;
	}

	vec3d operator * (const vec3d& vOther)
	{
		vec3d newVec;
		newVec.x = this->x * vOther.x;
		newVec.y = this->y * vOther.y;
		newVec.z = this->z * vOther.z;
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

	static float VectorDotProduct(vec3d& vec1, vec3d& vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	}

	static float VectorLenght(vec3d& v1)
	{
		return sqrtf(VectorDotProduct(v1, v1));
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

};

struct triangle
{
	vec3d points[3];
	vec3d middlePoint;

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

class camera 
{
public:
	vec3d position = { 0.0f,0.0f,0.0f,0.0f };
	
};

class lightSource
{
public:
	vec3d dir = { 0.0f,0.0f,-1.0f,0.0f };
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

void createRotationZMatrix(mat4x4& matrix, float time,float dir)
{
	float multiplier = time * dir;
	matrix.m[0][0] = cosf(multiplier);
	matrix.m[0][1] = sinf(multiplier);
	matrix.m[1][0] = -sinf(multiplier);
	matrix.m[1][1] = cosf(multiplier);
	matrix.m[2][2] = 1.0;
	matrix.m[3][3] = 1.0;
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



