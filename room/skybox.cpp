#include "Skybox.h"
#include "stb_image.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Skybox::Skybox(const std::vector<std::string>& faces)
{	//declaration of the vertices of the cube that will be used to draw the skybox
	float vertices[] = {
		-1, 1,-1, -1,-1,-1,  1,-1,-1,  1,-1,-1,  1, 1,-1, -1, 1,-1,
		-1,-1, 1, -1,-1,-1, -1, 1,-1, -1, 1,-1, -1, 1, 1, -1,-1, 1,
		 1,-1,-1,  1,-1, 1,  1, 1, 1,  1, 1, 1,  1, 1,-1,  1,-1,-1,
		-1,-1, 1, -1, 1, 1,  1, 1, 1,  1, 1, 1,  1,-1, 1, -1,-1, 1,
		-1, 1,-1,  1, 1,-1,  1, 1, 1,  1, 1, 1, -1, 1, 1, -1, 1,-1,
		-1,-1,-1, -1,-1, 1,  1,-1,-1,  1,-1,-1, -1,-1, 1,  1,-1, 1
	};
	//we generate the vertex array object and the vertex buffer object for the skybox
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//we specify the layout of the vertex data for the skybox
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// cubemap texture
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	//cubemaps doesnt need to be flipped vertically, so we disable the vertical flip
	stbi_set_flip_vertically_on_load(false);
	//we load the textures for each face of the cubemap
	for (unsigned int i = 0; i < faces.size(); i++)
	{	
		int w, h, ch;
		//pointer to the image data, who is loaded using stb_image library transformed in bytes 
		//3 for RGB, 4 for RGBA, 1 for grayscale
		unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &ch, 3); 
		if (data) {
			//opengl reads multiples of 4 bytes, so we set the unpack alignment to 1 to avoid padding issues
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0,
				GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cerr << "se perdio la textura D: " << faces[i]
				<< " | " << stbi_failure_reason() << std::endl;
		}
	}
		//parameters
	//target/ adjust/ how
	//for the skycube the transitions between faces should be smooth -glLinear is used for that :P 
		/*the min - mag filter is used when the pixel being textured maps to an area that is bigger - smaller respectively
		 than one texture element*/
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//the wrap parameters defines what happens when the texture coordinates are outside the range [0,1]
		/*clamping to edge means that the texture will
		 be clamped to the edge of the texture, so the color of the edge pixels will be used*/

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//horizontal
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//vertical
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);//depth
}

Skybox::~Skybox()
{
	//we delete the vertex array object, the vertex buffer object and the texture of the skybox
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteTextures(1, &texID);
}

void Skybox::Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection)
{
	//we set the depth function to
	// less than or equal to, so that the skybox is drawn behind all other objects
	glDepthFunc(GL_LEQUAL);
	shader.Activate();

	//we disable the traslation part of the view matrix, so that the skybox is always centered around the camera
	glm::mat4 viewNoMove = glm::mat4(glm::mat3(view));
	//we set the view and projection matrices for the skybox shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(viewNoMove));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1i(glGetUniformLocation(shader.ID, "skybox"), 0);
	//we bind the vertex array object and the cubemap texture, and draw the skybox
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	//we set the depth function back to less than, so that other objects are drawn in front of the skybox
	glDepthFunc(GL_LESS);
}
