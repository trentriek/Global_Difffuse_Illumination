
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

unsigned int SkyBox::loadCubemapTexture(std::vector<std::string> faces)
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
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void SkyBox::setShaderNames(const string& v, const string& f)
{
	vShaderName = v;
	fShaderName = f;
}

void SkyBox::init() {
	posBuf = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	//PART 1: settup the skkybox shaders
	GLint rc;

	// Create shader handles
	GLuint VS = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);

	// Read shader sources
	const char* vshader = GLSL::textFileRead(vShaderName.c_str());
	const char* fshader = GLSL::textFileRead(fShaderName.c_str());
	glShaderSource(VS, 1, &vshader, NULL);
	glShaderSource(FS, 1, &fshader, NULL);

	// Compile vertex shader
	glCompileShader(VS);
	glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
	if (!rc) {
		if (isVerbose) {
			GLSL::printShaderInfoLog(VS);
			cout << "Error compiling vertex shader " << vShaderName << endl;
		}
		return;
	}

	// Compile fragment shader
	glCompileShader(FS);
	glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
	if (!rc) {
		if (isVerbose) {
			GLSL::printShaderInfoLog(FS);
			cout << "Error compiling fragment shader " << fShaderName << endl;
		}
		return;
	}

	// Create the program and link
	pid = glCreateProgram();
	glAttachShader(pid, VS);
	glAttachShader(pid, FS);
	glLinkProgram(pid);
	glGetProgramiv(pid, GL_LINK_STATUS, &rc);
	if (!rc) {
		if (isVerbose) {
			GLSL::printProgramInfoLog(pid);
			cout << "Error linking shaders " << vShaderName << " and " << fShaderName << endl;
		}
		return;
	}



	GLSL::checkError(GET_FILE_LINE);

	//PART 2: create the position and texture buffers.

	glGenBuffers(1, &posid);
	glBindBuffer(GL_ARRAY_BUFFER, posid);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	//I think these next lines are meant to unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLSL::checkError(GET_FILE_LINE);

	tid = loadCubemapTexture(faces);

}

void SkyBox::bind() {
	glUseProgram(pid);
}
void SkyBox::unbind() {
	glUseProgram(0);
}

void SkyBox::draw() {
	
	//glGetAttribLocation(pid, name.c_str());

	//glUniformMatrix4fv(glGetAttribLocation(pid, "test");, 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
	//glUniformMatrix4fv(Sprog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
}







