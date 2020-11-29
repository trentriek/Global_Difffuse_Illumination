#version 130

varying vec3 texCoords; // vertex position in camera space

uniform samplerCube skybox;
void main()
{
	gl_FragColor = texture(skybox, texCoords);
}
