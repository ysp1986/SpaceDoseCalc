#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <assimp/Importer.hpp>
using std::stringstream;
using std::vector;
using std::string;

#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	string type;
	aiString path;
};

class Mesh
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;  //vertex array id
	unsigned int VBO;  //vertex data buffer id
	unsigned int EBO;  //element indices buffer id
	bool bufferCreated;
public:
	Mesh(vector<Vertex> &vers, vector<unsigned int> &inds, vector<Texture> &texs):bufferCreated(false){
		vertices = vers;
		indices = inds;
		textures = texs;
		VAO = 0;
		//CreateBuffers();
	}
	Mesh() :bufferCreated(false), VAO(0) {
	}
	~Mesh() {
	}

	void MakeCube(float len);
	/*
	if outSurface is true, the direction of all the facets points out of the sphere.
	*/
	void MakeSphere(float radius, bool outSurface
		, int numLattd = 10, int numLottd = 10);

	void MakeScreenPlanar1(unsigned int textureID, float minx = -1.0f, float maxx=1.0f, float miny = -1.0f, float maxy= 1.0f);

	void MakeScreenPlanar(unsigned int textureID, const char *textName = "diffuse", float depth = 0.0, float minx = -1.0f, float maxx = 1.0f, float miny = -1.0f, float maxy = 1.0f);


	bool CreateBuffers();

	bool BindBuffer(){
		glBindVertexArray(VAO);
		return true;
	}

	bool SetTexture(GLuint texture,string name= "diffuse"){
		textures.push_back({ texture, name });
		return true;
	}


	void Draw(Shader &shader);
};

