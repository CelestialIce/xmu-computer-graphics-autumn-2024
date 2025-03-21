#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Torus.h"
#include "Utils.h"
using namespace std;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 4

float cameraX, cameraY, cameraZ;
float torLocX, torLocY, torLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

Torus myTorus(0.5f, 0.2f, 48);
int numTorusVertices = myTorus.getNumVertices();
int numTorusIndices = myTorus.getNumIndices();

float amt = 0.0f;

// variable allocation for display
GLuint mvLoc, projLoc, nLoc;

GLuint globalAmbLoc,
ambLoc, diffLoc, specLoc, posLoc,
mambLoc, mdiffLoc, mspecLoc, mshiLoc,  //材质

ambLo1, diffLo1, specLo1, posLo1; //第二光源

int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, rMat, rMa1;
glm::vec3 currentLightPos, transformed;
glm::vec3 currentLightPo1, transforme1; //第二光源


float lightPos[3];
float lightPo1[3]; //第二光源位置

glm::vec3 initialLightLoc = glm::vec3(7.0f, 2.0f, 2.0f);
glm::vec3 initialLightLo1 = glm::vec3(-7.0f, -2.0f, -2.0f); //第二光源初始位置

// red light
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
// blue light
float globalAmbien1[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbien1[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffus1[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
float lightSpecula1[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

// gold material
float* matAmb = Utils::silverAmbient(); //表面材质
float* matDif = Utils::silverDiffuse();
float* matSpe = Utils::silverSpecular();
float matShi = Utils::silverShininess();

void installLights(glm::mat4 vMatrix) {
	//使用视图矩阵将光源变化到另一个矩阵空间
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	transforme1 = glm::vec3(vMatrix * glm::vec4(currentLightPo1, 1.0));
	lightPo1[0] = transforme1.x;
	lightPo1[1] = transforme1.y;
	lightPo1[2] = transforme1.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");

	ambLo1 = glGetUniformLocation(renderingProgram, "ligh1.ambient");
	diffLo1 = glGetUniformLocation(renderingProgram, "ligh1.diffuse");
	specLo1 = glGetUniformLocation(renderingProgram, "ligh1.specular");
	posLo1 = glGetUniformLocation(renderingProgram, "ligh1.position");

	mambLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);  //红光光照
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);

	glProgramUniform4fv(renderingProgram, ambLo1, 1, lightAmbien1); //蓝光光照
	glProgramUniform4fv(renderingProgram, diffLo1, 1, lightDiffus1);
	glProgramUniform4fv(renderingProgram, specLo1, 1, lightSpecula1);
	glProgramUniform3fv(renderingProgram, posLo1, 1, lightPo1);

	glProgramUniform4fv(renderingProgram, mambLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram, mdiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram, mspecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram, mshiLoc, matShi);
}

void setupVertices(void) {
	std::vector<int> ind = myTorus.getIndices();
	std::vector<glm::vec3> vert = myTorus.getVertices();
	std::vector<glm::vec2> tex = myTorus.getTexCoords();
	std::vector<glm::vec3> norm = myTorus.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < myTorus.getNumVertices(); i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		tvalues.push_back(tex[i].s);
		tvalues.push_back(tex[i].t);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
	}
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("./PhongShaders/vertShader.glsl", "./PhongShaders/fragShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 1.0f;
	torLocX = 0.0f; torLocY = 0.0f; torLocZ = -1.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	currentLightPos = glm::vec3(initialLightLoc.x, initialLightLoc.y, initialLightLoc.z);
	currentLightPo1 = glm::vec3(initialLightLo1.x, initialLightLo1.y, initialLightLo1.z); //位置设置
	amt = currentTime * 25.0f;
	rMat = glm::rotate(glm::mat4(1.0f), toRadians(amt), glm::vec3(0.0f, 0.0f, 1.0f));
	rMa1 = glm::rotate(glm::mat4(1.0f), toRadians(-amt), glm::vec3(0.0f, 0.0f, 1.0f)); //反方向旋转
	currentLightPos = glm::vec3(rMat * glm::vec4(initialLightLoc, 1.0f));
	currentLightPo1 = glm::vec3(rMa1 * glm::vec4(initialLightLo1, 1.0f));

	installLights(vMat);

	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter 7 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}