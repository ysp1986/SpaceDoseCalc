#include "stdafx.h"
#include "Mesh.h"

void Mesh::MakeCube(float len){
	float verticetmp[] = {
		// positions          // normal           // texture coords
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top right
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,   // bottom left
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,    // top left 

		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top right
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,    // top left 

		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // top right
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // bottom left
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,    // top left 

		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,   // top right
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,   // bottom left
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,    // top left 

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // bottom right
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // bottom left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,    // top left 

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // bottom left
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,    // top left 

	};
	vertices.resize(24);

	for (int i = 0; i < 24; ++i){
		vertices[i] = {glm::vec3(verticetmp[i * 8], verticetmp[i * 8 + 1], verticetmp[i * 8 + 2])
			, glm::vec3(verticetmp[i * 8 + 3], verticetmp[i * 8 + 4], verticetmp[i * 8 + 5])
			, glm::vec2(verticetmp[i * 8 + 6], verticetmp[i * 8 + 7]) };
	}

	unsigned int indicestmp[] = {
		0, 1, 2,
		2, 3, 0,
		4, 6, 5,
		6, 4, 7,
		8, 9, 10,
		10, 8, 11,
		12, 14, 13,
		14, 15, 12,
		16, 18, 17,
		18, 16, 19,
		20, 21, 22,
		22, 23, 20
	};
	indices.resize(sizeof(indicestmp) / sizeof(unsigned int));
	memcpy(indices.data(), indicestmp, sizeof(indicestmp));
	//CreateBuffers();
}

/*
if outSurface is true, the direction of all the facets points out of the sphere.
*/
void Mesh::MakeSphere(float radius, bool outSurface
	, int numLattd , int numLottd){
	this->vertices.resize( (numLattd+1)*(numLottd + 1));
	this->indices.resize(numLottd*(numLattd -1) * 2 * 3); //totally numLattd*numLottd*2-2*numLattd
	//first vertice

	float dLotRadian = glm::radians(360.0) / numLottd; //经线
	float dLatRadian = glm::radians(180.0f) / numLattd;  //纬线

	float dLatTextCoord = 1.0f / numLattd;
	float dLotTextCoord = 1.0f / numLottd;

	float radianLattd = 0.0f;
	float texCoordLattd = 0.0f;
	unsigned offsetIndices = 0;
	for (int i = 0; i <= numLattd; ++i)
	{
		float cosRadianLattd = glm::cos(radianLattd);
		float sinRadianLattd = glm::sin(radianLattd);
		float zLottd = radius*cosRadianLattd;
		float planeRadius = radius*sinRadianLattd;
		float radianLottd = 0.0f;
		float texCoordLottd = 0.0f;
		for (int j = 0; j <= numLottd; ++j)
		{
			
			if (i<numLottd && i>0)
				new(vertices.data() + i*(numLottd + 1) + j)Vertex({
						{ planeRadius*glm::cos(radianLottd), planeRadius*glm::sin(radianLottd), zLottd },
						{ sinRadianLattd*glm::cos(radianLottd), sinRadianLattd*glm::sin(radianLottd), cosRadianLattd },
						{ texCoordLottd, texCoordLattd } });
			else
				new(vertices.data() + i*(numLottd + 1) + j)Vertex({
						{ planeRadius*glm::cos(radianLottd), planeRadius*glm::sin(radianLottd), zLottd },
						{ sinRadianLattd*glm::cos(radianLottd), sinRadianLattd*glm::sin(radianLottd), cosRadianLattd },
						{ texCoordLottd + dLotTextCoord / 2, texCoordLattd } });
			radianLottd += dLotRadian;
			texCoordLottd += dLotTextCoord;
		}
		texCoordLattd += dLatTextCoord;
		radianLattd += dLatRadian;
		//triangles indices
		if (i < numLattd)
		{
			if (outSurface){
				if (i == 0){
					for (int j = 0; j < numLottd; ++j)
					{
						indices[j * 3] = j;
						indices[j * 3 + 1] = j + (numLottd + 1);
						indices[j * 3 + 2] = j + 1 + (numLottd + 1);
					}
					offsetIndices += numLottd * 3;
				}
				else if (i < numLattd -1 && i>0){
					for (int j = 0; j < numLottd; ++j)
					{
						indices[offsetIndices + j * 6] = j + i*(numLottd + 1);
						indices[offsetIndices + j * 6 + 1] = j + i*(numLottd + 1) + (numLottd + 1);
						indices[offsetIndices + j * 6 + 2] = (j + 1) + i*(numLottd + 1) + (numLottd + 1);
						indices[offsetIndices + j * 6 + 3] = (j + 1) + i*(numLottd + 1);
						indices[offsetIndices + j * 6 + 4] = j + i*(numLottd + 1);
						indices[offsetIndices + j * 6 + 5] = (j + 1) + i*(numLottd + 1) + (numLottd + 1);
					}
					offsetIndices += numLottd * 6;
				}
				else if (i == numLattd -1){
					for (int j = 0; j < numLottd; ++j)
					{
						indices[offsetIndices + j * 3] = j + i*(numLottd + 1);
						indices[offsetIndices + j * 3 + 1] = j + i*(numLottd + 1) + (numLottd + 1);
						indices[offsetIndices + j * 3 + 2] = (j + 1) + i*(numLottd + 1);
					}
					offsetIndices += numLottd * 3;
				}
			}
			else{
				if (i == 0){
					for (int j = 0; j < numLottd; ++j)
					{
						indices[j * 3] = j;
						indices[j * 3 + 2] = j + (numLottd + 1);
						indices[j * 3 + 1] = (j + 1) + (numLottd + 1);
					}
					offsetIndices += numLottd * 3;
				}
				else if (i < numLattd -1 && i>0){
					for (int j = 0; j < numLottd; ++j)
					{
						indices[offsetIndices + j * 6] = j + i*(numLottd + 1);
						indices[offsetIndices + j * 6 + 2] = j + i*(numLottd + 1) + (numLottd + 1);
						indices[offsetIndices + j * 6 + 1] = (j + 1) + i*(numLottd + 1) + (numLottd + 1);
						indices[offsetIndices + j * 6 + 3] = (j + 1) + i*(numLottd + 1);
						indices[offsetIndices + j * 6 + 5] = j + i*(numLottd + 1);;
						indices[offsetIndices + j * 6 + 4] = (j + 1) + i*(numLottd + 1) + (numLottd + 1);
					}
					offsetIndices += numLottd * 6;
				}
				else if (i == numLattd -1){
					for (int j = 0; j < numLottd; ++j)
					{
						indices[offsetIndices + j * 3] = j + i*(numLottd + 1);
						indices[offsetIndices + j * 3 + 2] = j + i*(numLottd + 1) + (numLottd + 1);
						indices[offsetIndices + j * 3 + 1] = (j + 1) + i*(numLottd + 1);
					}
					offsetIndices += numLottd * 3;
				}
			}
		}
		
		
	}
	//CreateBuffers();
}


void Mesh::MakeScreenPlanar1(unsigned int textureID, float minx, float maxx, float miny, float maxy) {
	float verticetmp[] = {
		maxx, maxy, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,   // top right
		maxx, miny, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // bottom right
		minx, miny, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,   // bottom left
		minx, maxy, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,    // top left 
	};
	vertices.resize(4);

	for (int i = 0; i < 4; ++i) {
		vertices[i] = { glm::vec3(verticetmp[i * 8], verticetmp[i * 8 + 1], verticetmp[i * 8 + 2])
			, glm::vec3(verticetmp[i * 8 + 3], verticetmp[i * 8 + 4], verticetmp[i * 8 + 5])
			, glm::vec2(verticetmp[i * 8 + 6], verticetmp[i * 8 + 7]) };
	}

	unsigned int indicestmp[] = {
		0, 2, 1,
		2, 0, 3
	};
	indices.resize(sizeof(indicestmp) / sizeof(unsigned int));
	memcpy(indices.data(), indicestmp, sizeof(indicestmp));
	

	SetTexture(textureID);
	//CreateBuffers();
}


void Mesh::MakeScreenPlanar(unsigned int textureID, const char *textName, float depth, float minx, float maxx, float miny, float maxy) {
	if (depth<-1.0 || depth > 1.0)
		depth = 0.0;
	float verticetmp[] = {
		maxx, maxy, depth, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top right
		maxx, miny, depth, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,   // bottom right
		minx, miny, depth, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
		minx, maxy, depth, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,    // top left 
	};
	vertices.resize(4);

	for (int i = 0; i < 4; ++i) {
		vertices[i] = { glm::vec3(verticetmp[i * 8], verticetmp[i * 8 + 1], verticetmp[i * 8 + 2])
			, glm::vec3(verticetmp[i * 8 + 3], verticetmp[i * 8 + 4], verticetmp[i * 8 + 5])
			, glm::vec2(verticetmp[i * 8 + 6], verticetmp[i * 8 + 7]) };
	}

	unsigned int indicestmp[] = {
		0, 2, 1,
		2, 0, 3
	};
	indices.resize(sizeof(indicestmp) / sizeof(unsigned int));
	memcpy(indices.data(), indicestmp, sizeof(indicestmp));


	SetTexture(textureID, textName);
	//CreateBuffers();
}
bool Mesh::CreateBuffers() {
	glGenVertexArrays(1, &VAO);
	//绑定VAO
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	//复制定点数组到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	return true;
}

void Mesh::Draw(Shader &shader)
{
	if (!bufferCreated || VAO == 0)
	{
		CreateBuffers();
		bufferCreated = true;
	}
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
										  // retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name = "material." + textures[i].type;
		string name2 = "material.has" + textures[i].type;
		// now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader.programid, name.c_str()), i);
		glUniform1i(glGetUniformLocation(shader.programid, name2.c_str()), 1);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
	shader.use();
	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}