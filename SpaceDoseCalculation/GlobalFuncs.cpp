#include "stdafx.h"
#include "GlobalFuncs.h"
#define STB_IMAGE_IMPLEMENTATION
#include "std_image.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <codecvt>
#include <windows.h>
#include <CommDlg.h>
using namespace std;
unsigned int GlobalFuncs::LoadTexture(const char *path, GLuint *outwidth, GLuint *outheight) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		if (outwidth != nullptr)
			*outwidth = width;
		if (outheight != nullptr)
			*outheight = height;
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;

}

#include "Shlwapi.h"
#include <string>
#include <iostream>
using namespace std;

wstring GlobalFuncs::ChDirToExePath() {
	HMODULE mHandler = GetModuleHandle(NULL);
	wchar_t exeFilePath[256];
	GetModuleFileName(mHandler, exeFilePath, 256);
	wstring path = exeFilePath;
	path = path.substr(0, path.rfind(L'\\'));
	_wchdir(path.data());
	return path;
}

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		if (error.size()>0)
			std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;
	switch (source)
	{
	case GL_DEBUG_SOURCE_API_ARB:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER_ARB:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR_ARB:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_OTHER_ARB:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

string GlobalFuncs::SelectSavePath(const TCHAR * titleName, const TCHAR *filter) {
	wstring curdir = GlobalFuncs::ChDirToExePath();
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof(Ofn));
	TCHAR szFile[1024];
	szFile[0] = L'\0';
	Ofn.hwndOwner = NULL;
	Ofn.lStructSize = sizeof(Ofn);
	Ofn.lpstrFilter = filter;
	Ofn.lpstrFile = szFile;
	Ofn.nMaxFile = sizeof(szFile) / sizeof(*szFile);
	Ofn.lpstrFileTitle = NULL;
	Ofn.nMaxFileTitle = 0;
	Ofn.lpstrInitialDir = (TCHAR *)curdir.data();
	Ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
	Ofn.lpstrTitle = titleName;
	BOOL ret = GetSaveFileName(&Ofn);
	DWORD err = CommDlgExtendedError();
	wstring savePath = L"";
	if (ret)
		savePath = Ofn.lpstrFile;

	//setup converter
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.to_bytes(savePath);
}