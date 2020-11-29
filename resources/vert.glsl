#version 120

uniform mat4 P; //
uniform mat4 MV; //model view matrix

//The "output" of the vert shader is all of the varying variables
//gl_position is built in Varying variable

attribute vec4 aPos; // in object space
attribute vec3 aNor; // in object space

varying vec3 vpos; // vertex position in camera space
varying vec3 normal; //normal in camera space 

void main()
{
	gl_Position = P * MV * aPos; //built in variable

	vec4 normal4 = MV * vec4(aNor, 0.0);
	vec4 vpos4 = MV * aPos;
	normal = normal4.xyz;
	vpos = vpos4.xyz;

}
