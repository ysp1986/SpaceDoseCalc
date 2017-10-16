#pragma once
class Shader
{
	

public:

	unsigned int programid;
	bool success;

	Shader() {
		success = false;
		programid = 0;
	}

	Shader(const char *vertexPath, const char *fragmentPath);

	Shader(Shader &other) :programid(other.programid),success(other.success) {
	}

	~Shader();

	void use();

	//load shader
	void LoadShader(const char *vertexPath, const char *fragmentPath);

	//set shader directly
	void SetShader(const char *verShaderCode, const char *fragShaderCode);

	//uniform
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMat4(const std::string &name, glm::mat4 &value) const;
	void setVec3(const std::string &name, float v1, float v2, float v3) const;
};

