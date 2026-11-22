
#include <glm/glm.hpp> 
#include <glm/ext.hpp>
#include "camera.h"
#include "shaderClass.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 target) 
//normal vector ^- the upper direction is like the roof of the room that is the upDir
	:width(width), height(height), camPos(position), camTarget(target), upDir(0.0f, 1.0f, 0.0f)
{
	//we obtain the direction of the camera by cross product of postion and target
	camFront = glm::normalize(camTarget-camPos);
	yes = glm::degrees(asin(camFront.y));
	no = glm::degrees(atan2(camFront.z, camFront.x));
	UpdateAxes();
}
void Camera:: UpdateAxes() {
	//1st we build the axis of the camera
		//we change the direction of the camera to be the opposite of
		// the front vector because the front vector is the direction where the camera is looking at and we want to move in the opposite direction
		camDirection = -camFront;
		//right vector -> represents the +x axis of the camera 
		camRight = glm::normalize(glm::cross(upDir, camDirection));
		//camera up direction ^- we obtain the real upper direction of the camera crossing the z and x axis of the camera
		camUp = glm::cross(camDirection, camRight);
}
glm::mat4 Camera::getViewMatrix() {
	//we use the lookAt function to obtain the view matrix of the camera
	return glm::lookAt(camPos, camPos+camFront, camUp);
}
void Camera::updateFront() {
	glm::vec3 f;
	f.x = cos(glm::radians(no)) * cos(glm::radians(yes));
	f.y = sin(glm::radians(yes));
	f.z = sin(glm::radians(no)) * cos(glm::radians(yes));
	camFront = glm::normalize(f);
	UpdateAxes();
}
void Camera::walkAround(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform) {
	
	glm::mat4 view = getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(FOVdeg),
		(float)width / height, nearPlane, farPlane);   
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE,
		glm::value_ptr(projection * view));

}
void Camera::setSkyboxUniforms(Shader& shader, float FOVdeg, float nearPlane, float farPlane) {
	//just view rotation , no translation bc the skybox is always centered on the camera D U H
	glm::mat4 view = glm::mat4(glm::mat3(getViewMatrix()));
	glm::mat4 projection = glm::perspective(glm::radians(FOVdeg),
		(float)width / height, nearPlane, farPlane);

	shader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void Camera::drawSkybox(Skybox& skybox, Shader& skyboxShader, float FOVdeg, float nearPlane, float farPlane) {
	glm::mat4 view = getViewMatrix();  
	glm::mat4 projection = glm::perspective(glm::radians(FOVdeg),
		(float)width / height, nearPlane, farPlane);
	skybox.Draw(skyboxShader, view, projection);
}