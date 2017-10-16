#pragma once
class Camera
{
public:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;
	float percpectFov;
	glm::mat4 perspectTrans;
	glm::mat4 viewTrans;

	glm::mat4 viewTransForMouse;

	float pitch;
	float yaw;

	float screenWidth;
	float screenHeight;

	float lastMouseX;
	float lastMouseY;
	float lastPressTime = -1.0;

	float sensitivityRotate;

	float cameraSpeed;

	bool lbtnPressed;

	Camera(float swidth = 800.0f, float sheight = 600.0f,glm::vec3 pos = glm::vec3(0.0f, 4.0f, 0.0f),
		glm::vec3 front = glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f), float fov = 50.0f) :

		screenWidth(swidth), screenHeight(sheight),
		cameraPos(pos), cameraFront(front), cameraUp(up), percpectFov(fov)
		, sensitivityRotate(0.05f), cameraSpeed(0.01f), lbtnPressed(false)
	{
		pitch = 0.0f;
		yaw = 0.0f;
		UpdateVectors();
	}


	~Camera();

	void InputProcess(GLFWwindow *window)
	{
		if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ||
			(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ||
			(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) ||
			(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS))
		{
			double curtime = glfwGetTime();
			if (lastPressTime >= 0.0)
			{
				float deltaTime = (float)curtime;
				if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
					cameraPos += deltaTime*cameraSpeed * cameraFront;
				if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
					cameraPos -= deltaTime*cameraSpeed * cameraFront;
				if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
					cameraPos -= deltaTime*glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
					cameraPos += deltaTime*glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			}
			lastPressTime = curtime;
		}
	}

	void ResizeViewPortSize(float width, float height) {
		screenWidth = width;
		screenHeight = height;
	}

	void DirectionProcess(GLFWwindow* window, float xpos, float ypos){
		if (!lbtnPressed)
		{
			lastMouseX = xpos;
			lastMouseY = ypos;
			lbtnPressed = true;
		}
		else
		{
			yaw += (lastMouseX - xpos)*sensitivityRotate;
			pitch += (lastMouseY - ypos)*sensitivityRotate;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			std::cout << "x: " << xpos << " y: " << ypos << " ";

			lastMouseX = xpos;
			lastMouseY = ypos;

			if(cameraFront.y != 0.0f)
				float prefy = cameraFront.y;

			cameraFront.x = -cos(glm::radians(pitch))*sin(glm::radians(yaw));
			cameraFront.y = -cos(glm::radians(pitch))*cos(glm::radians(yaw));
			cameraFront.z = -sin(glm::radians(pitch));

			std::cout <<" pitch: "<< pitch + 90 << " ";
			std::cout << "fx: " << cameraFront.x << " fy: " << cameraFront.y << " fz: " << cameraFront.z << std::endl;

		}
	}

	void ReleaseLBtn() {
		lbtnPressed = false;
	}

	glm::mat4 GetPerspectMat(){
		if(screenHeight >0 && screenWidth >0)
			perspectTrans = glm::perspective(glm::radians(percpectFov), 1.0f*screenWidth / screenHeight, 0.01f, 3000.0f);
		else
			perspectTrans = glm::perspective(glm::radians(percpectFov), 1.0f, 0.01f, 3000.0f);
		return perspectTrans;
	}

	glm::mat4 GetViewMat(){
		viewTrans = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		return viewTrans;
	}

	glm::mat4 GetBackgroundViewMat(){
		viewTrans = glm::lookAt(glm::vec3(0.0f,0.0f,0.0f), cameraFront, cameraUp);
		return viewTrans;
	}

	void UpdateVectors(){
		cameraRight = glm::cross(cameraFront, cameraUp);
		lastMouseX = screenWidth / 2;
		lastMouseY = screenHeight / 2;

		cameraFront.x = -cos(glm::radians(pitch))*sin(glm::radians(yaw));
		cameraFront.y = -cos(glm::radians(pitch))*cos(glm::radians(yaw));
		cameraFront.z = -sin(glm::radians(pitch));
	}
};

