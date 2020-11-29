#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "MatrixStack.h"
class Program;

/**
 * A shape defined by a list of triangles
 * - posBuf should be of length 3*ntris
 * - norBuf should be of length 3*ntris (if normals are available)
 * - texBuf should be of length 2*ntris (if texture coords are available)
 * posBufID, norBufID, and texBufID are OpenGL buffer identifiers.
 */
class Shape
{
public:
	Shape();
	virtual ~Shape();
	void loadMesh(const std::string &meshName);
	void fitToUnitBox();
	void init();
	void draw(const std::shared_ptr<Program> prog) const;

	void translate(const glm::vec3& t);
	void scale(const glm::vec3& s);
	void rotate(float r, char axis);

	MatrixStack Transform;

private:
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;
	unsigned posBufID;
	unsigned norBufID;
	unsigned texBufID;


	glm::vec3 XROT;
	glm::vec3 YROT;
	glm::vec3 ZROT;


};

#endif
