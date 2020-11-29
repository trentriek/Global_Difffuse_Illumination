#pragma once

#include <string>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include "GLSL.h"
#include <glm/glm.hpp>
//#include <GL/glew.h>
//#include "Shape.h"

class SkyBox {

public:
	SkyBox(std::string resource_directory = "");
	virtual ~SkyBox();
	unsigned int loadCubemapTexture(std::vector<std::string> faces);
	void init();
	std::vector<std::string> faces;
	std::string Resource_dir;

	void setShaderNames(const std::string& v, const std::string& f);
	
	virtual void bind();
	virtual void unbind();
	void draw();

private:
	std::vector<float> posBuf;
	GLuint pid;
	GLuint posid;
	GLuint tid;
	std::string vShaderName;
	std::string fShaderName;

	bool isVerbose = false;

};