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

//interface
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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
	//imGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

		// Configure theme for imgui
	ImGui::StyleColorsDark();

		// Initialize GLFW and binds it wi
	ImGui_ImplGlfw_InitForOpenGL(window, true);

		// Initialize OpenGL backend
	ImGui_ImplOpenGL3_Init("#version 330");

	// Set up OpenGL state
	glEnable(GL_DEPTH_TEST);
	{
		Shader shaderProgram ("default.vert", "default.frag");
		Shader lightShader("light.vert", "light.frag");
		shaderProgram.Activate();

			// Create a cube object with a texture
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "tex0"), 0);

		Skybox skybox({
				"SkyBox/px.png",
				"SkyBox/nx.png","SkyBox/py.png","SkyBox/ny.png","SkyBox/pz.png","SkyBox/nz.png"
			});
		
		Camera camera(width, height, glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));
		KeyboardInputs keyboardInputs;
		Model diorama("Models/scene.gltf");
		//Model diorama("Models/ancient_ruined.glb");
		
		//cristal config :)
		Model crystal("Models/purple_crystal.glb");
		//parameters to control their transformation 
		glm::vec3 crystalPosition(0.0f, 3.0f, 0.0f);
		glm::vec3 crystalRotation(0.0f);
		float crystalScale = 0.5f;
		glm::vec3 lightColor(1.0f, 0.9f, 0.7f);
		float lightIntensity = 1.0f;
		//skybox 
		// we set the skybox and the model to be drawn 
		Shader skyboxShader("skybox.vert", "skybox.frag");
		const float FOV = 45.0f, nearPlane = 0.1f, farPlane = 100.0f;
		glm::mat4 modelMatrix(1.0f);
		float lastFrame = 0.0f;
		// Main loop

		while (!glfwWindowShouldClose(window))
		{
			// 1 Process GLFW events
			glfwPollEvents();

			// 2 Calculate delta time
			float currentFrame = static_cast<float>(glfwGetTime());
			float deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// 4 Start ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			//avoid to confuse the keyboard inputs with the sliders
			ImGuiIO& io = ImGui::GetIO();
			// 5 Process camera inputs
			keyboardInputs.processInput(window,camera,deltaTime,io.WantCaptureMouse,io.WantCaptureKeyboard);
			

			// 6 ImGui interface
			ImGui::Begin("Crystal Controls");
			//color of the light
			ImGui::ColorEdit3("Light Color", &lightColor.x);
			// intensity 
			ImGui::SliderFloat("Intensity", &lightIntensity, 0.0f, 5.0f);
			//transformations 
			ImGui::Text("Crystal Transformation");

			ImGui::SliderFloat3(
				"Position",
				&crystalPosition.x,
				-20.0f,
				20.0f
			);

			ImGui::SliderFloat3(
				"Rotation",
				&crystalRotation.x,
				-180.0f,
				180.0f
			);

			ImGui::SliderFloat(
				"Scale",
				&crystalScale,
				0.1f,
				3.0f
			);

			ImGui::End();

			// 7 Clear buffers
			glClearColor(0.8f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 8 Activate shader
			shaderProgram.Activate();

			camera.walkAround(FOV, nearPlane, farPlane, shaderProgram, "camMatrix");

			glUniform3f(glGetUniformLocation(shaderProgram.ID, "pointLightPos"),
				crystalPosition.x, crystalPosition.y, crystalPosition.z);

			// color final = color * intensidad
			glm::vec3 finalColor = lightColor * lightIntensity;
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "pointLightColor"),
				finalColor.r, finalColor.g, finalColor.b);

			glUniform1f(glGetUniformLocation(shaderProgram.ID, "ambientStrength"), 0.15f);
			glUniform1f(glGetUniformLocation(shaderProgram.ID, "diffuseStrength"), 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.ID, "constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shaderProgram.ID, "linear"), 0.014f);
			glUniform1f(glGetUniformLocation(shaderProgram.ID, "quadratic"), 0.0007f);

			diorama.Draw(shaderProgram, camera, modelMatrix);

			// 11 Build crystal transformation
			glm::mat4 crystalModel(1.0f);

			crystalModel = glm::translate(
				crystalModel,
				crystalPosition
			);

			crystalModel = glm::rotate(
				crystalModel,
				glm::radians(crystalRotation.x),
				glm::vec3(1.0f, 0.0f, 0.0f)
			);

			crystalModel = glm::rotate(
				crystalModel,
				glm::radians(crystalRotation.y),
				glm::vec3(0.0f, 1.0f, 0.0f)
			);

			crystalModel = glm::rotate(
				crystalModel,
				glm::radians(crystalRotation.z),
				glm::vec3(0.0f, 0.0f, 1.0f)
			);

			crystalModel = glm::scale(
				crystalModel,
				glm::vec3(crystalScale)
			);
			//Draw Diorama
			camera.walkAround(FOV, nearPlane, farPlane, shaderProgram, "camMatrix");
			diorama.Draw(shaderProgram, camera, modelMatrix);

			//Draw crystal
			lightShader.Activate();
			camera.walkAround(FOV, nearPlane, farPlane, lightShader, "camMatrix");
			glUniform3f(glGetUniformLocation(lightShader.ID, "pointLightColor"),
				lightColor.r, lightColor.g, lightColor.b);
			crystal.Draw(lightShader, camera, crystalModel);

			// 13 Draw skybox
			camera.drawSkybox(
				skybox,
				skyboxShader,
				FOV,
				nearPlane,
				farPlane
			);

			// 14 Render ImGui
			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData(
				ImGui::GetDrawData()
			);

			// 15 swap 
			glfwSwapBuffers(window);
		}
	}
	//destroy the ImGui events
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;

}