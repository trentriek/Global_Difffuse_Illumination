#pragma once

#include <string>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include "GLSL.h"
#include <glm/glm.hpp>

#include "program.h"

class SkyBox {

public:
	SkyBox(std::string resource_directory = "");
	virtual ~SkyBox();
	unsigned int loadCubemapTexture(std::vector<std::string> faces);
	void init();
	std::vector<std::string> faces;
	std::string Resource_dir;

	std::shared_ptr<Program> prog;

	void draw();

	void bindtexture(GLint handle);
	void unbindtexture();

private:
	std::vector<float> posBuf;
	GLuint posid;
	GLuint tid;

	bool isVerbose = false;

};