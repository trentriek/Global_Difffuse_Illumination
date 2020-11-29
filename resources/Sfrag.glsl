#version 120

varying vec3 vpos; // vertex position in camera space
varying vec3 normal; //normal in camera space

void main()
{
	float angle = dot(normal,normalize(-vpos));
	vec3 color = vec3(1.0,1.0,1.0);
	if(abs(angle) < 0.2) {color = vec3(0.0, 0.0, 0.0);}
	gl_FragColor = vec4(color.r, color.g, color.b, 1.0);
}
