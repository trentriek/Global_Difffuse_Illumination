#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Material.h"
#include "Light.h"
#include "SkyBox.h"

using namespace std;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = "./"; // Where the resources are loaded from

shared_ptr<Camera> camera;
shared_ptr<Program> prog;
shared_ptr<Program> Sprog;
shared_ptr<Program> Tprog;

shared_ptr<Program> SBprog; //skybox shader

shared_ptr<Shape> shape;
shared_ptr<Shape> shape2;

//Global Illumination additions:
shared_ptr<SkyBox> skybox;
DGI Irradiance;
glm::mat4x4 Global_R; glm::mat4x4 Global_G; glm::mat4x4 Global_B;
void get_MatrixCoeffeicients(glm::mat4x4& R, glm::mat4x4& G, glm::mat4x4& B, DGI* ir);

const int numofMats = 3;
const int numofLights = 2;
Material M1;
Material M2;
Material M3;
Material Toon;
Material* Mats[numofMats] = { &M1,&M2,&M3 };
Light L1;
Light L2;
Light* Lights[numofLights] = {&L1,&L2};
int mIndex = 0;
int lIndex = 0;
Material* SelectedMaterial = Mats[0];
Light* SelectedLight = Lights[0];
bool Sshader = false;
bool Tshader = false;

bool keyToggles[256] = {false}; // only for English keyboards!

// This function is called when a GLFW error occurs
static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

// This function is called when a key is pressed
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

// This function is called when the mouse is clicked
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	// Get the current mouse position.
	double xmouse, ymouse;
	glfwGetCursorPos(window, &xmouse, &ymouse);
	// Get current window size.
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(action == GLFW_PRESS) {
		bool shift = (mods & GLFW_MOD_SHIFT) != 0;
		bool ctrl  = (mods & GLFW_MOD_CONTROL) != 0;
		bool alt   = (mods & GLFW_MOD_ALT) != 0;
		camera->mouseClicked((float)xmouse, (float)ymouse, shift, ctrl, alt);
	}
}

// This function is called when the mouse moves
static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if(state == GLFW_PRESS) {
		camera->mouseMoved((float)xmouse, (float)ymouse);
	}
}

static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];

	if (key == 'm') {
		if (SelectedMaterial == Mats[numofMats - 1]) { SelectedMaterial = Mats[0];  mIndex = 0; }  //toggling standard blinn-phong shader not used - still calls input but nothing happens.
		 else {																					   //lookk at the render function
			 mIndex++;
			 SelectedMaterial = Mats[mIndex];
		 }
	}
	if (key == 'M') {
		if (SelectedMaterial == Mats[0]) { SelectedMaterial = Mats[numofMats-1];  mIndex = 2; }
		else { mIndex--; SelectedMaterial = Mats[mIndex]; }
	}
	if (key == 'l') {
		if (SelectedLight == Lights[numofLights-1]) { SelectedLight = Lights[0];  lIndex = 0; }
		else { lIndex++; SelectedLight = Lights[lIndex]; }
	}
	if (key == 'L') {
		if (SelectedLight == Lights[0]) { SelectedLight = Lights[numofLights - 1];  lIndex = 1; }
		else { lIndex--; SelectedLight = Lights[lIndex]; }
	}
	if (key == 'x') {
		SelectedLight->LightPos.x += 1.0f;
	}
	if (key == 'X') {
		SelectedLight->LightPos.x -= 1.0f;
	}
	if (key == 'y') {
		SelectedLight->LightPos.y += 1.0f;
	}
	if (key == 'Y') {
		SelectedLight->LightPos.y -= 1.0f;
	}
	if (key == 'z') {
		SelectedLight->LightPos.z += 1.0f;
	}
	if (key == 'Z') {
		SelectedLight->LightPos.z -= 1.0f;
	}
	if (key == 's') {
		Sshader = !Sshader;
	}
	if (key == 'S') {
		Sshader = !Sshader;
	}
	if (key == 't') {
		Tshader = !Tshader;
	}
	if (key == 'T') {
		Tshader = !Tshader;
	}
}

// If the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// This function is called once to initialize the scene and OpenGL
static void init()
{
	// Initialize time.
	glfwSetTime(0.0);
	
	// Set background color.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	//create program shader1
	prog = make_shared<Program>();
	prog->setShaderNames(RESOURCE_DIR + "vert.glsl", RESOURCE_DIR + "frag.glsl");
	prog->setVerbose(true);
	prog->init();
	prog->addAttribute("aPos");
	prog->addAttribute("aNor");
	prog->addUniform("MV");
	prog->addUniform("P");
	prog->addUniform("lightPos");
	prog->addUniform("lightInt");
	prog->addUniform("lightPos2");
	prog->addUniform("lightInt2");
	prog->addUniform("ka");
	prog->addUniform("kd");
	prog->addUniform("ks");
	prog->addUniform("s");
	prog->setVerbose(false);

	//create shader2
	Sprog = make_shared<Program>();
	Sprog->setShaderNames(RESOURCE_DIR + "Svert.glsl", RESOURCE_DIR + "Sfrag.glsl");
	Sprog->setVerbose(true);
	Sprog->init();
	Sprog->addAttribute("aPos");
	Sprog->addAttribute("aNor");
	Sprog->addUniform("MV");
	Sprog->addUniform("P");
	Sprog->addUniform("E_R");
	Sprog->addUniform("E_G");
	Sprog->addUniform("E_B");
	Sprog->setVerbose(false);

	//create shader2
	Tprog = make_shared<Program>();
	Tprog->setShaderNames(RESOURCE_DIR + "tvert.glsl", RESOURCE_DIR + "tfrag.glsl");
	Tprog->setVerbose(true);
	Tprog->init();
	Tprog->addUniform("MV");
	Tprog->addUniform("P");
	Tprog->addAttribute("aPos");
	Tprog->addAttribute("aNor");
	Tprog->addUniform("lightPos");
	Tprog->addUniform("lightInt");
	Tprog->addUniform("lightPos2");
	Tprog->addUniform("lightInt2");
	Tprog->addUniform("E_R");
	Tprog->addUniform("E_G");
	Tprog->addUniform("E_B");
	Tprog->addUniform("ka");
	Tprog->addUniform("kd");
	Tprog->addUniform("ks");
	Tprog->addUniform("s");
	Tprog->setVerbose(false);


	//shader 3
	SBprog = make_shared<Program>();
	SBprog->setShaderNames(RESOURCE_DIR + "sbvert.glsl", RESOURCE_DIR + "sbfrag.glsl");
	SBprog->setVerbose(true);
	SBprog->init();
	SBprog->addAttribute("aPos");
	SBprog->addUniform("MV");
	SBprog->addUniform("P");
	SBprog->addUniform("skybox");
	SBprog->setVerbose(false);


	//create camera
	camera = make_shared<Camera>();
	camera->setInitDistance(2.0f);
	
	//load mesh
	shape = make_shared<Shape>();
	shape->loadMesh(RESOURCE_DIR + "teapot.obj");
	shape->fitToUnitBox();
	shape->init();
	
	shape2 = make_shared<Shape>();
	shape2->loadMesh(RESOURCE_DIR + "cube.obj");
	shape2->fitToUnitBox();
	shape2->init();
	shape2->translate(glm::vec3(-1.0, 0.0, -1.0));

	//Final project addition - added a skybox as well as a global

	Irradiance.add_coefficient(0, 0);
	Irradiance.add_coefficient(1, 1);
	Irradiance.add_coefficient(1, 0);
	Irradiance.add_coefficient(1, -1);
	Irradiance.add_coefficient(2, 1);
	Irradiance.add_coefficient(2, -1);
	Irradiance.add_coefficient(2, -2);
	Irradiance.add_coefficient(2, 0);
	Irradiance.add_coefficient(2, 2);


	skybox = make_shared<SkyBox>(RESOURCE_DIR + "skybox3/");
	skybox->prog = SBprog;
	skybox->init(&Irradiance); //we pass in the Irradiance class so that we can pass in the color infomration of the skybox as its being read in.

	//this is for testing - manutally setting the coefficients
	//Irradiance.sample_coefficients_1();

	get_MatrixCoeffeicients(Global_R, Global_G, Global_B, &Irradiance);


	//set Materials
	Material::CreateMaterialsForProject(prog, M1, M2, M3, Toon);

	//set lighting
	Light::createLightForProject(prog, L1, L2);
	
	GLSL::checkError(GET_FILE_LINE);
}

// This function is called every frame to draw the scene.
static void render()
{
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(keyToggles[(unsigned)'c']) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
	if(keyToggles[(unsigned)'w']) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	
	// Matrix stacks
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	
	// Apply camera transforms
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	MV->pushMatrix();
	camera->applyViewMatrix(MV);

	//draw the skybox
	SBprog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));

	skybox->bindtexture(prog->getUniform("skybox"));
	skybox->draw();
	skybox->unbindtexture();
	SBprog->unbind();


	
	if (Sshader) {
		Sprog->bind();
		glUniformMatrix4fv(Sprog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(Sprog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(Sprog->getUniform("E_R"), 1, GL_FALSE, glm::value_ptr(Global_R));
		glUniformMatrix4fv(Sprog->getUniform("E_G"), 1, GL_FALSE, glm::value_ptr(Global_G));
		glUniformMatrix4fv(Sprog->getUniform("E_B"), 1, GL_FALSE, glm::value_ptr(Global_B));
		shape->draw(Sprog);

		//draw object 2
		MV->multMatrix(shape2->Transform.topMatrix());
		glUniformMatrix4fv(Sprog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		shape2->draw(Sprog);

		Sprog->unbind();
	
	}
	else if(Tshader) {
		Tprog->bind();
		glUniformMatrix4fv(Tprog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(Tprog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(Tprog->getUniform("E_R"), 1, GL_FALSE, glm::value_ptr(Global_R));
		glUniformMatrix4fv(Tprog->getUniform("E_G"), 1, GL_FALSE, glm::value_ptr(Global_G));
		glUniformMatrix4fv(Tprog->getUniform("E_B"), 1, GL_FALSE, glm::value_ptr(Global_B));
		//set light
		L1.SetShaderLight(Tprog);
		L2.SetShaderLight(Tprog);
		//set material
		M1.SetShaderToMat(Tprog);
		shape->draw(Tprog);

		//draw object 2
		MV->multMatrix(shape2->Transform.topMatrix());
		glUniformMatrix4fv(Tprog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		shape2->draw(Tprog);

		Tprog->unbind();
	}
	else {
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		//set light
		L1.SetShaderLight(prog);
		L2.SetShaderLight(prog);
		//set material
		M1.SetShaderToMat(prog);
		shape->draw(prog);

		//draw object 2
		MV->multMatrix(shape2->Transform.topMatrix());
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		shape2->draw(prog);

		prog->unbind();
	}

	MV->popMatrix();
	P->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Trent Riek", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	GLSL::checkVersion();
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set mouse button callback.
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
void get_MatrixCoeffeicients(glm::mat4x4& R, glm::mat4x4& G, glm::mat4x4& B, DGI* ir) {

	double c1 = 0.429043;
	double c2 = 0.511664;
	double c3 = 0.743125;
	double c4 = 0.886227;
	double c5 = 0.247708;
	
	R[0][0] = c1 * ir->get_coefficient(2, 2, 0);
	R[0][1] = c1 * ir->get_coefficient(2, -2, 0);
	R[0][2] = c1 * ir->get_coefficient(2, 1, 0);
	R[0][3] = c2 * ir->get_coefficient(1, 1, 0);
	R[1][0] = c1 * ir->get_coefficient(2, -2, 0);
	R[1][1] = -c1 * ir->get_coefficient(2, 2, 0);
	R[1][2] = c1 * ir->get_coefficient(2, -1, 0);
	R[1][3] = c2 * ir->get_coefficient(1, -1, 0);
	R[2][0] = c1 * ir->get_coefficient(2, 1, 0);
	R[2][1] = c1 * ir->get_coefficient(2, -1, 0);
	R[2][2] = c3 * ir->get_coefficient(2, 0, 0);
	R[2][3] = c2 * ir->get_coefficient(1, 0, 0);
	R[3][0] = c2 * ir->get_coefficient(1, 1, 0);
	R[3][1] = c2 * ir->get_coefficient(1, -1, 0);
	R[3][2] = c2 * ir->get_coefficient(1, 0, 0);
	R[3][3] = ( c4 * ir->get_coefficient(0, 0, 0) ) - (c5 * ir->get_coefficient(2, 0, 0));
	cout << R[3][3];

	G[0][0] = c1 * ir->get_coefficient(2, 2, 1);
	G[0][1] = c1 * ir->get_coefficient(2, -2, 1);
	G[0][2] = c1 * ir->get_coefficient(2, 1, 1);
	G[0][3] = c2 * ir->get_coefficient(1, 1, 1);
	G[1][0] = c1 * ir->get_coefficient(2, -2, 1);
	G[1][1] = -c1 * ir->get_coefficient(2, 2, 1);
	G[1][2] = c1 * ir->get_coefficient(2, -1, 1);
	G[1][3] = c2 * ir->get_coefficient(1, -1, 1);
	G[2][0] = c1 * ir->get_coefficient(2, 1, 1);
	G[2][1] = c1 * ir->get_coefficient(2, -1, 1);
	G[2][2] = c3 * ir->get_coefficient(2, 0, 1);
	G[2][3] = c2 * ir->get_coefficient(1, 0, 1);
	G[3][0] = c2 * ir->get_coefficient(1, 1, 1);
	G[3][1] = c2 * ir->get_coefficient(1, -1, 1);
	G[3][2] = c2 * ir->get_coefficient(1, 0, 1);
	G[3][3] = (c4 * ir->get_coefficient(0, 0, 1)) - (c5 * ir->get_coefficient(2, 0, 1));
	cout << G[3][3];

	B[0][0] = c1 * ir->get_coefficient(2, 2, 2);
	B[0][1] = c1 * ir->get_coefficient(2, -2, 2);
	B[0][2] = c1 * ir->get_coefficient(2, 1, 2);
	B[0][3] = c2 * ir->get_coefficient(1, 1, 2);
	B[1][0] = c1 * ir->get_coefficient(2, -2, 2);
	B[1][1] = -c1 * ir->get_coefficient(2, 2, 2);
	B[1][2] = c1 * ir->get_coefficient(2, -1, 2);
	B[1][3] = c2 * ir->get_coefficient(1, -1, 2);
	B[2][0] = c1 * ir->get_coefficient(2, 1, 2);
	B[2][1] = c1 * ir->get_coefficient(2, -1, 2);
	B[2][2] = c3 * ir->get_coefficient(2, 0, 2);
	B[2][3] = c2 * ir->get_coefficient(1, 0, 2);
	B[3][0] = c2 * ir->get_coefficient(1, 1, 2);
	B[3][1] = c2 * ir->get_coefficient(1, -1, 2);
	B[3][2] = c2 * ir->get_coefficient(1, 0, 2);
	B[3][3] = (c4 * ir->get_coefficient(0, 0, 2)) - (c5 * ir->get_coefficient(2, 0, 2));
	cout << B[3][3];
}
