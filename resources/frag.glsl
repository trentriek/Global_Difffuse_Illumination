#version 120

uniform vec3 lightPos; //already in camera space, and camera is 0 0 0 in camera space
uniform float lightInt; //light intensity
uniform vec3 lightPos2; 
uniform float lightInt2; 
uniform vec3 ka; //
uniform vec3 kd; //
uniform vec3 ks; //specularity
uniform float s; //

varying vec3 vpos; // vertex position in camera space
varying vec3 normal; //normal in camera space

void main()
{

	//average the light pos
	//light1
	vec3 l = normalize(lightPos - vpos); //for cd
	vec3 cd = kd * max(0, dot(l,normal));
	vec3 e = normalize(-vpos); //redundant to subrant from the camera space of 0 0 0 
	vec3 h = normalize(l + e);
	vec3 cs = ks * pow(max(0,dot(h,normal)),s);
	vec3 color1 = lightInt * (ka + cd + cs);

	//light2
	l = normalize(lightPos2 - vpos); //for cd
	cd = kd * max(0, dot(l,normal));
	e = normalize(-vpos); //redundant to subrant from the camera space of 0 0 0 
	h = normalize(l + e);
	cs = ks * pow(max(0,dot(h,normal)),s);
	vec3 color2 = lightInt2 * (ka + cd + cs);

	//add color1 color2
	vec3 color = color1 + color2;
	//color = color2;
	gl_FragColor = vec4(color.r, color.g, color.b, 1.0);
}
