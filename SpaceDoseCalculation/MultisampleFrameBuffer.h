#pragma once
#include <iostream>
using namespace std;
#include "FrameTextureBuffer.h"
class MultisampleFrameBuffer :
	public FrameTextureBuffer
{
	GLsizei mySamples;
public:
	MultisampleFrameBuffer(GLsizei samples,GLuint width, GLuint height, float z = 0.0f)
	{
		mySamples = samples;
		myReady = false;
		myViewWidth = width;
		myViewHeight = height;
		myOpenForRender = false;
		myDepth = z;
		//build multisample texture
		glGenTextures(1, &myTexture); GLCHECK
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, myTexture); GLCHECK
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, myViewWidth, myViewHeight, GL_TRUE); GLCHECK
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,0);

		//generate multiple frame
		glGenFramebuffers(1, &myFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, myFbo); GLCHECK
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, myTexture, 0); GLCHECK

		//multisample depth render buffer binding(only for render, will not use it, use render buffer for render speed)
		glGenRenderbuffers(1, &myRenderBufferDepthStensilNoUse);
		glBindRenderbuffer(GL_RENDERBUFFER, myRenderBufferDepthStensilNoUse);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, myViewWidth, myViewHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, myRenderBufferDepthStensilNoUse);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); GLCHECK
		if (status == GL_FRAMEBUFFER_COMPLETE) {
			myReady = true;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Resize(GLuint width, GLuint height)
	{
		if (width != myViewWidth || height != myViewHeight)
		{
			myViewWidth = width;
			myViewHeight = height;
			cout << "size changed to: " << myViewWidth << "," << myViewHeight << endl;

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, myTexture); GLCHECK
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mySamples, GL_RGB, myViewWidth, myViewHeight, GL_TRUE); GLCHECK
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0); GLCHECK
		
			glBindFramebuffer(GL_FRAMEBUFFER, myFbo); GLCHECK
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0); GLCHECK

			glDeleteRenderbuffers(1, &myRenderBufferDepthStensilNoUse); GLCHECK
			glGenRenderbuffers(1, &myRenderBufferDepthStensilNoUse); GLCHECK
			glBindRenderbuffer(GL_RENDERBUFFER, myRenderBufferDepthStensilNoUse); GLCHECK
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, mySamples, GL_DEPTH24_STENCIL8, myViewWidth, myViewHeight); GLCHECK
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, myRenderBufferDepthStensilNoUse); GLCHECK

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		}
	}
	~MultisampleFrameBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &myFbo);

		glDeleteTextures(1, &myTexture);
		//delete Mesh object
		if (myScreenPlanar)
			delete myScreenPlanar;
		if (myShader)
			delete myShader;
	}

	virtual bool DrawToScene(float l = 0.0f, float t = 0.0f, float r = 1.0f, float b = 1.0f) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); GLCHECK  // Make sure no FBO is set as the draw framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, myFbo); GLCHECK // Make sure your multisampled FBO is the read framebuffer
		glBlitFramebuffer(0, 0, myViewWidth, myViewHeight, 0, 0, myViewWidth, myViewHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST); GLCHECK
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		return true;
	}



};

