#pragma once
#include "Shader.h"
#include "Mesh.h"
#include "GlobalFuncs.h"
#include "ConfigureEditor.h"
class SrcTallyPairDoseCalc;

class FrameTextureBuffer
{
	friend SrcTallyPairDoseCalc;
protected:
	FrameTextureBuffer() {

	}
	GLuint myViewWidth;
	GLuint myViewHeight;
	GLuint myFbo;
	bool myReady;
	GLuint myTexture;
	GLuint myRenderBufferDepthStensilNoUse;
	bool myOpenForRender;

	float myDepth;

	GLuint myVertexArray;
	GLuint myVertexBuffer;

	Shader *myShader;

	Mesh *myScreenPlanar;
public:


	FrameTextureBuffer(GLuint width, GLuint height, float z = 0.0f)
		:myReady(false), myViewWidth(width), myViewHeight(height)
		, myOpenForRender(false), myDepth(z) {
		//generate frame buffer, the size will not be changed later
		glGenFramebuffers(1, &myFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, myFbo);
		glGenTextures(1, &myTexture);
		glBindTexture(GL_TEXTURE_2D, myTexture);

		//color texture binding
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //the last data is used to fill the texture, here we do not need to do this

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myTexture, 0);

		//depth render buffer binding(only for render, will not use it, use render buffer for render speed)
		glGenRenderbuffers(1, &myRenderBufferDepthStensilNoUse);
		glBindRenderbuffer(GL_RENDERBUFFER, myRenderBufferDepthStensilNoUse);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, myViewWidth, myViewHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, myRenderBufferDepthStensilNoUse);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
			myReady = true;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		myShader = new Shader(u8"Media\\shaders\\vertex-simple.shader", u8"Media\\shaders\\fragment-simple.shader");

		myShader->setMat4("modelSpaceTrans", glm::mat4());
		myShader->setMat4("viewTrans", glm::lookAt(glm::vec3(0, 2, 3), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1)));// glm::mat4());
		myShader->setMat4("perspectTrans", glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 2000.0f)); //glm::mat4());


		myScreenPlanar = new Mesh();
		myScreenPlanar->MakeScreenPlanar(myTexture, "diffuse", myDepth);
	}

	FrameTextureBuffer(const char *imageFile) {

	}

	~FrameTextureBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &myFbo);

		//need to delete other buffer?? 
		glBindVertexArray(myVertexArray);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);

		glDeleteTextures(1, &myTexture);
		glDeleteRenderbuffers(1, &myRenderBufferDepthStensilNoUse);
		glDeleteVertexArrays(1, &myVertexArray);
		glDeleteBuffers(1, &myVertexBuffer);

		//delete Mesh object
		if (myScreenPlanar)
			delete myScreenPlanar;
	}

	void SetShader(Shader *pS) {
		myShader = pS;
	}

	GLsizei myPreViewWidth;
	GLsizei myPreViewHeight;

	bool BeginRenderToMe() {
		if (!myReady)
			return false;
		glViewport(0, 0, myViewWidth, myViewHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, myFbo);
		glClearColor(ConfigureEditor::bgColor.r, ConfigureEditor::bgColor.g, ConfigureEditor::bgColor.b, 1.0f); GLCHECK
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GLCHECK
			myOpenForRender = true;
		return true;
	}

	bool EndRenderToMe() {
		if (myOpenForRender) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return true;
		}
		else
			return false;
		//need to refresh the texture buffer after rendering to this buffer?? possibly not
	}

	virtual bool DrawToScene(float l = 0.0f, float t = 0.0f, float r = 1.0f, float b = 1.0f) {

		//draw quad with my texture
		/*myShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,myTexture);
		myShader->setInt("myTexture", 0);

		glBindVertexArray(myVertexArray);
		glDrawArrays(GL_TRIANGLES,0,6);
		glBindVertexArray(0);*/
		//end drawing
		myShader->setFloat("depth", myDepth);
		myScreenPlanar->Draw(*myShader); GLCHECK
			return true;
	}

	void SetZ(float z) {
		myDepth = z;
	}

	GLuint GetTexture() {
		return myTexture;
	}
};

