#version 120

uniform mat4 P; //
uniform mat4 MV; //model view matrix


attribute vec4 aPos; // in object space

varying vec3 texCoords; // vertex position in camera space

void main()
{
	TexCoords = aPos;

	gl_Position = P * MV * vec4(aPos, 1.0); 


}