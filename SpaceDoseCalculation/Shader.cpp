#include "stdafx.h"
#include "Shader.h"


Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
	LoadShader(vertexPath, fragmentPath);
}




Shader::~Shader()
{
	if (success) {
		glUseProgram(0);
		glDeleteProgram(this->programid);
	}
}

void Shader::use()
{
	glUseProgram(this->programid);
}

void Shader::SetShader(const char *vertexSource, const char *fragmentSource) {
	if (success) {
		glUseProgram(0);
		glDeleteProgram(this->programid);
	}
	this->success = false;

	unsigned int vShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShaderID, 1, &vertexSource, NULL);
	glCompileShader(vShaderID);

	int success;
	char infoLog[1024];

	glGetShaderiv(vShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vShaderID, 1024, NULL, infoLog);
		std::cout << vertexSource << "ERROR::SHADER::compiling vertex shader: " << infoLog << std::endl;
		return;
	}

	unsigned int fShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShaderID, 1, &fragmentSource, NULL);
	glCompileShader(fShaderID);

	glGetShaderiv(fShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fShaderID, 1024, NULL, infoLog);
		std::cout << fragmentSource << "ERROR::SHADER::compiling fragment shader: " << infoLog << std::endl;
		return;
	}


	this->programid = glCreateProgram();
	glAttachShader(this->programid, vShaderID);
	glAttachShader(this->programid, fShaderID);
	glLinkProgram(this->programid);

	glGetProgramiv(this->programid, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->programid, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER:: linking shader program " << infoLog << std::endl;
		return;
	}

	glDeleteShader(vShaderID);
	glDeleteShader(fShaderID);

	this->success = true;
}

void  Shader::LoadShader(const char *vertexPath, const char *fragmentPath) {
	if (success) {
		glUseProgram(0);
		glDeleteProgram(this->programid);
	}
	this->success = false;
	std::ifstream vShaderStream;
	std::ifstream fShaderStream;
	std::string vShaderCode;
	std::string fShaderCode;
	vShaderStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderStream.open(vertexPath);
		fShaderStream.open(fragmentPath);
		std::stringstream vShaderSS, fShaderSS;
		vShaderSS << vShaderStream.rdbuf();
		fShaderSS << fShaderStream.rdbuf();
		vShaderStream.close();
		fShaderStream.close();
		vShaderCode = vShaderSS.str();
		fShaderCode = fShaderSS.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		return;
	}

	const char *vertexSource = vShaderCode.c_str();
	const char *fragmentSource = fShaderCode.c_str();

	unsigned int vShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShaderID, 1, &vertexSource, NULL);
	glCompileShader(vShaderID);

	int success;
	char infoLog[1024];

	glGetShaderiv(vShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vShaderID, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER::compiling vertex shader: " << infoLog << std::endl;
		return;
	}

	unsigned int fShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShaderID, 1, &fragmentSource, NULL);
	glCompileShader(fShaderID);

	glGetShaderiv(fShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fShaderID, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER::compiling fragment shader: " << infoLog << std::endl;
		return;
	}


	this->programid = glCreateProgram();
	glAttachShader(this->programid, vShaderID);
	glAttachShader(this->programid, fShaderID);
	glLinkProgram(this->programid);

	glGetProgramiv(this->programid, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->programid, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER:: linking shader program " << infoLog << std::endl;
		return;
	}

	glDeleteShader(vShaderID);
	glDeleteShader(fShaderID);

	this->success = true;
}


//uniform
void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(this->programid, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(this->programid, name.c_str()),value);
}

void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(this->programid, name.c_str()),value);
}


void Shader::setMat4(const std::string &name, glm::mat4 &value) const
{
	glUniformMatrix4fv(glGetUniformLocation(this->programid, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, float v1, float v2, float v3) const
{
	glUniform3f(glGetUniformLocation(this->programid,name.c_str()),v1,v2,v3);
}