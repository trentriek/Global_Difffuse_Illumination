#version 130

uniform mat4 P; //
uniform mat4 MV; //model view matrix

attribute vec3 aPos; // in object space

varying vec3 texCoords; // vertex position in camera space

void main()
{
	texCoords = aPos;
	gl_Position = P * MV * vec4(aPos, 1.0);

}