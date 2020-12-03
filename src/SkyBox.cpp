
#include "SkyBox.h"
#include <algorithm>
#include <iostream>

//#include "GLSL.h"
//#include "Program.h"

//#include <glm/glm.hpp>

#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
SkyBox::SkyBox(std::string resource_directory) {
	Resource_dir = resource_directory;
	faces = {
			Resource_dir + "right.jpg",
			Resource_dir + "left.jpg",
			Resource_dir + "top.jpg",
			Resource_dir + "bottom.jpg",
			Resource_dir + "front.jpg",
			Resource_dir + "back.jpg"
	};

}
SkyBox::~SkyBox() {

}

unsigned int SkyBox::loadCubemapTexture(std::vector<std::string> faces, DGI* I)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			if (I) {
				I->getrgbvalues(data, width, height);
			}


			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	I->skyboxsize = 3.0;
	I->setupsides(width, height);
	I->calculateCoefficients();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void SkyBox::init(DGI* I) {
	posBuf = {
		// positions

		-3.0f,  3.0f, -3.0f,
		-3.0f, -3.0f, -3.0f,
		 3.0f, -3.0f, -3.0f,
		 3.0f, -3.0f, -3.0f,
		 3.0f,  3.0f, -3.0f,
		-3.0f,  3.0f, -3.0f,

		-3.0f, -3.0f,  3.0f,
		-3.0f, -3.0f, -3.0f,
		-3.0f,  3.0f, -3.0f,
		-3.0f,  3.0f, -3.0f,
		-3.0f,  3.0f,  3.0f,
		-3.0f, -3.0f,  3.0f,


		 3.0f, -3.0f, -3.0f,
		 3.0f, -3.0f,  3.0f,
		 3.0f,  3.0f,  3.0f,
		 3.0f,  3.0f,  3.0f,
		 3.0f,  3.0f, -3.0f,
		 3.0f, -3.0f, -3.0f,

		-3.0f, -3.0f,  3.0f,
		-3.0f,  3.0f,  3.0f,
		 3.0f,  3.0f,  3.0f,
		 3.0f,  3.0f,  3.0f,
		 3.0f, -3.0f,  3.0f,
		-3.0f, -3.0f,  3.0f,

		-3.0f,  3.0f, -3.0f,
		 3.0f,  3.0f, -3.0f,
		 3.0f,  3.0f,  3.0f,
		 3.0f,  3.0f,  3.0f,
		-3.0f,  3.0f,  3.0f,
		-3.0f,  3.0f, -3.0f,

		-3.0f, -3.0f, -3.0f,
		-3.0f, -3.0f,  3.0f,
		 3.0f, -3.0f, -3.0f,
		 3.0f, -3.0f, -3.0f,
		-3.0f, -3.0f,  3.0f,
		 3.0f, -3.0f,  3.0f
	};


	//PART 2: create the position and texture buffers.

	glGenBuffers(1, &posid);
	glBindBuffer(GL_ARRAY_BUFFER, posid);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	//I think these next lines are meant to unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLSL::checkError(GET_FILE_LINE);

	tid = loadCubemapTexture(faces, I);

}

void SkyBox::draw() {

		// Bind position buffer
		int h_pos = prog->getAttribute("aPos");
		glEnableVertexAttribArray(h_pos);
		glBindBuffer(GL_ARRAY_BUFFER, posid);
		glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

		// Bind texcoords buffer
		int h_tex = prog->getUniform("skybox");
		if (h_tex != -1 && tid != 0) {
			glEnableVertexAttribArray(h_tex);
			glBindBuffer(GL_ARRAY_BUFFER, tid);
			glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);
		}

		glDepthMask(GL_FALSE);
		// Draw
		int count = posBuf.size() / 3; // number of indices to be rendered
		glDrawArrays(GL_TRIANGLES, 0, count);

		// Disable and unbind
		if (h_tex != -1) {
			glDisableVertexAttribArray(h_tex);
		}
		glDisableVertexAttribArray(h_pos);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDepthMask(GL_TRUE);
		GLSL::checkError(GET_FILE_LINE);
}

void SkyBox::bindtexture(GLint handle) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tid);
	glUniform1i(handle, 0);
}
void SkyBox::unbindtexture() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}







