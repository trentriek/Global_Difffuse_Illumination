#version 120

uniform vec3 lightPos; //already in camera space, and camera is 0 0 0 in camera space
uniform vec3 lightPos2;

varying vec3 vpos; // vertex position in camera space
varying vec3 normal; //normal in camera space 
uniform mat4 E_R; //irradiance R
uniform mat4 E_G; //irradiance G
uniform mat4 E_B; //irradiance B
void main()
{
	float intensity;
	vec4 color;
	vec3 lightDir = normalize(lightPos - vpos); //for cd
	vec3 lightDir2 = normalize(lightPos2 - vpos); //for cd
	intensity = dot(lightDir,normalize(normal)) + dot(lightDir2,normalize(normal));

	if (intensity > 0.95)
		color = vec4(1.0,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = vec4(0.6,0.3,0.3,1.0);
	else if (intensity > 0.25)
		color = vec4(0.4,0.2,0.2,1.0);
	else
		color = vec4(0.2,0.1,0.1,1.0);

	float angle = dot(normal,normalize(-vpos));
	if(abs(angle) < 0.2) {
		color = vec4(0.0,0.0,0.0, 1.0);
		}

	gl_FragColor = color;

}
