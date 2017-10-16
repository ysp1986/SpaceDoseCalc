#include "stdafx.h"
#include "Shader.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <glad/glad.h>
#include <iostream>

unsigned int create_shader_program1()
{
	char *vertexShader =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(aPos.x,aPos.y,aPos.z, 1.0);\n"
		"}\n";
	char *fragmentShader =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n";
	int success;
	char infoLog[512];

	unsigned int vertexShaderId;
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
	glCompileShader(vertexShaderId);

	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "Error compiling vertex shader " << infoLog << std::endl;
	}

	unsigned int fragmentShaderId;
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
	glCompileShader(fragmentShaderId);
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog);
		std::cout << "Error compiling fragment shader " << infoLog << std::endl;
	}

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaderId);
	glAttachShader(shaderProgram, fragmentShaderId);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Error linking shader program " << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
	return shaderProgram;
}

unsigned int create_shader_program2()
{
	char *vertexShader =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"out vec3 fakeColor;"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(aPos.x,aPos.y,aPos.z, 1.0);\n"
		"	fakeColor = aPos;\n"
		"}\n";
	char *fragmentShader =
		"#version 330 core\n"
		"in vec3 fakeColor;"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(fakeColor.x+0.5, fakeColor.y+0.5, fakeColor.z+0.5, 1.0f);\n"
		"}\n";
	int success;
	char infoLog[512];

	unsigned int vertexShaderId;
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
	glCompileShader(vertexShaderId);

	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "Error compiling vertex shader " << infoLog << std::endl;
	}

	unsigned int fragmentShaderId;
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
	glCompileShader(fragmentShaderId);
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog);
		std::cout << "Error compiling fragment shader " << infoLog << std::endl;
	}

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaderId);
	glAttachShader(shaderProgram, fragmentShaderId);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Error linking shader program " << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
	return shaderProgram;
}


unsigned int create_vertex_attrib_array()
{
	float vertices[] = {
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

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
		4, 5, 6,
		6, 7, 4,
		8, 9, 10,
		10, 11, 8,
		12, 13, 14,
		14, 15, 12,
		16, 17, 18,
		18, 19, 16,
		20, 21, 22,
		22, 23, 20
	};

	unsigned int vaoid;
	glGenVertexArrays(1, &vaoid);
	//绑定VAO
	glBindVertexArray(vaoid);
	unsigned int vboid;
	glGenBuffers(1, &vboid);
	//复制定点数组到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int eboid;
	glGenBuffers(1, &eboid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	return vaoid;
}
//
//glm::vec3 cubePositions[] = {
//	glm::vec3(0.0f, 0.0f, 0.0f),
//	glm::vec3(2.0f, 5.0f, -15.0f),
//	glm::vec3(-1.5f, -2.2f, -2.5f),
//	glm::vec3(-3.8f, -2.0f, -12.3f),
//	glm::vec3(2.4f, -0.4f, -3.5f),
//	glm::vec3(-1.7f, 3.0f, -7.5f),
//	glm::vec3(1.3f, -2.0f, -2.5f),
//	glm::vec3(1.5f, 2.0f, -2.5f),
//	glm::vec3(1.5f, 0.2f, -1.5f),
//	glm::vec3(-1.3f, 1.0f, -1.5f)
//};

//{glBindVertexArray(vaoid);
//shader.use();
//perspectTrans = camera.GetPerspectMat();
//shader.setMat4("perspectTrans", perspectTrans);
//
//
//
//viewTrans = camera.GetViewMat();
//shader.setMat4("viewTrans", viewTrans);
//shader.setVec3("mat.ambient", 0.2f, 0.1f, 0.05f);
//shader.setVec3("mat.diffuse", 0.3f, 0.2f, 0.1f);
//shader.setVec3("mat.specular", 0.8f, 0.8f, 0.8f);
//shader.setFloat("mat.shininess", 30.0f);
//shader.setVec3("lightpos", 0.0f, 0.0f, -15.0f);
//
//shader.setVec3("camerapos", camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);
//
//for (int i = 0; i < 10; ++i)
//{
//	glm::mat4 modelTrans;
//	modelTrans = glm::translate(modelTrans, glm::vec3(cubePositions[i].x, cubePositions[i].y, cubePositions[i].z + moveFar));
//	modelTrans = glm::rotate(modelTrans, (float)glfwGetTime()*i * glm::radians(5.0f), glm::vec3(1.0f, 1.0f, 0.0f));
//
//
//	shader.setMat4("modelTrans", modelTrans);
//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
//}
//		{
//			//draw the light
//			glm::mat4 modelTrans;
//			modelTrans = glm::translate(modelTrans, glm::vec3(0.0f, 0.0f, -15.0f));
//			modelTrans = glm::scale(modelTrans, glm::vec3(0.1, 0.1, 0.1));
//			shader.setMat4("modelTrans", modelTrans);
//			shader.setVec3("mat.ambient", 1.0f, 1.0f, 1.0f);
//			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
//		}
//}