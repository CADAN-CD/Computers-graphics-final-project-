#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp> //this library contains the vec4,vec3, vec2, mat4 classes and the basic operations on them
#include <glm/gtc/matrix_transform.hpp>


// Include all GLM extensions
#include <glm/ext.hpp> // perspective, translate, rotate


#include "shaderClass.h"
#include "stb_image.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"
#include "KeyboardInputs.h"


#include <direct.h>
#include <iostream>

//Models
#include "Model.h"
#include "Mesh.h"

//skybox
#include "Skybox.h"
int main() {
	// Initialize GLFW

	if (!glfwInit()) {
		return -1;
	}
	int width = 1000;
	int height = 800;
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(width, height, "Room", NULL, NULL);
	
	if (!window) {
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	// Load OpenGL functions using GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}
	// Set up OpenGL state
	glEnable(GL_DEPTH_TEST);
	{
		Shader shaderProgram ("default.vert", "default.frag");
		shaderProgram.Activate();
			// Create a cube object with a texture
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "tex0"), 0);

		Skybox skybox({
				"SkyBox/px.png",
				"SkyBox/nx.png","SkyBox/py.png","SkyBox/ny.png","SkyBox/pz.png","SkyBox/nz.png"
			});
		
		Camera camera(width, height, glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));
		KeyboardInputs keyboardInputs;
		Model diorama("Models/ancient_ruined.glb");
		Model crystal("Models/crystal.gltf");
		
		//skybox 
		// we set the skybox and the model to be drawn 
		Shader skyboxShader("skybox.vert", "skybox.frag");
		const float FOV = 45.0f, nearPlane = 0.1f, farPlane = 100.0f;
		glm::mat4 modelMatrix(1.0f);
		float lastFrame = 0.0f;
		// Main loop
		while (!glfwWindowShouldClose(window)) {
			float currentFrame = static_cast<float>(glfwGetTime());
			float deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			glClearColor(0.8f, 0.3f, 0.3f, 1.0f);   
			keyboardInputs.processInput(window, camera, deltaTime);
			//1st we clean the color and depth buffer to prepare for the next frame
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shaderProgram.Activate();
			GLint ok;
			glGetProgramiv(shaderProgram.ID, GL_LINK_STATUS, &ok);
			std::cout << "default shader link OK: " << ok << std::endl;
			//initial parameters for the camera, we pass them to the shader to obtain the view and projection matrix of the camera
			camera.walkAround(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");
			glm::mat4 cubeModel = glm::rotate(glm::mat4(1.0f), currentFrame, glm::vec3(0, 1, 0));			//cube.Draw(shaderProgram, cubeModel);
			camera.walkAround(FOV, nearPlane, farPlane, shaderProgram, "camMatrix");
			diorama.Draw(shaderProgram, camera, modelMatrix);
			

			//draw the skybox
			camera.drawSkybox(skybox, skyboxShader, FOV, nearPlane, farPlane);
			GLenum err = glGetError();
			if (err != GL_NO_ERROR) std::cout << "GL error: " << err << std::endl;
			glfwSwapBuffers(window);

			glfwPollEvents();
		}
		shaderProgram.Delete();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;

}