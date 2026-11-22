#include "KeyboardInputs.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "camera.h"
void KeyboardInputs::processInput(GLFWwindow* window, Camera& camera, float deltaTime) {
	const float sensitivity = 0.05f;
	const float cameraSpeed = 2.5f * deltaTime;
	float lastFrame = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.camPos += cameraSpeed * camera.camFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.camPos -= cameraSpeed * camera.camFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.camPos -= glm::normalize(glm::cross(camera.camFront, camera.camUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.camPos += glm::normalize(glm::cross(camera.camFront, camera.camUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		camera.camPos += cameraSpeed * camera.camUp;

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		camera.camPos -= cameraSpeed * camera.camUp;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			if (camera.firstClick) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwGetCursorPos(window, &camera.lastX, &camera.lastY);

				camera.firstClick = false;
			}
			else {
				double mouseX, mouseY;
				glfwGetCursorPos(window, &mouseX, &mouseY);
				camera.no += (float)(mouseX - camera.lastX) * sensitivity;
				camera.yes -= (float)(mouseY - camera.lastY) * sensitivity;
				camera.lastX = mouseX;
				camera.lastY = mouseY;
			}
		if (camera.yes > 89.0f)camera.yes = 89.0f;
		if (camera.yes < -89.0f)camera.yes = -89.0f;
		glm::vec3 front;
		front.x = cos(glm::radians(camera.no)) * cos(glm::radians(camera.yes));
		front.z = sin(glm::radians(camera.no)) * cos(glm::radians(camera.yes));
		front.y = sin(glm::radians(camera.yes));
		
		camera.camFront = glm::normalize(front);
		camera.UpdateAxes(); //recalculates the vectors from camDir, camRight, camUp
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		camera.firstClick = true;
	}
	
}
	