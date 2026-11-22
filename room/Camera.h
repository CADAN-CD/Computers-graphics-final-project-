#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shaderClass.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "skybox.h"
class Camera {
public:
	float width, height;
	glm::vec3 camPos;
	glm::vec3 camTarget;
	glm::vec3 upDir;
	//z
	glm::vec3 camDirection;
	//x
	glm::vec3 camRight;
	//y
	glm::vec3 camUp;          
	glm::vec3 camFront;

	// angles (degrees)
	float yes, no;            // yes = pitch (up/down), no = yaw (left/right)
	bool firstClick = true;
	double lastX = 0.0, lastY = 0.0;

	Camera(int width, int height, glm::vec3 position, glm::vec3 target);
	void UpdateAxes();
	void updateFront();       
	glm::mat4 getViewMatrix();
	void walkAround(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
	void setSkyboxUniforms(Shader& shader, float FOVdeg, float nearPlane, float farPlane);
	void drawSkybox(Skybox& skybox, Shader& skyboxShader, float FOVdeg, float nearPlane, float farPlane);
};

#endif