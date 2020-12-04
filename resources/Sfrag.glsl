#version 120

varying vec3 vpos; // vertex position in camera space
varying vec3 normal; //normal in camera space
uniform mat4 E_R; //irradiance R
uniform mat4 E_G; //irradiance G
uniform mat4 E_B; //irradiance B

void main()
{
	vec3 color = vec3(0.0,0.0,0.0);
	normalize(normal);

	vec4 Ambiant = vec4(normal, 1.0) * E_R;
	color.x = dot(Ambiant,vec4(normal, 1.0));

	Ambiant = vec4(normal, 1.0) * E_G;
	color.y = dot(Ambiant,vec4(normal, 1.0));

	Ambiant = vec4(normal, 1.0) * E_B;
	color.z = dot(Ambiant,vec4(normal, 1.0));

	//color.x = color.x * 0.1; color.y = color.y * 0.1; color.z = color.z * 0.1;

	gl_FragColor = vec4(color.x, color.y, color.z, 1.0);
}
