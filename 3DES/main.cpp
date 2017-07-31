#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <glew/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mShader.h"
#include "mCamera.h"
#include "mDefs.h"
#include "mMeshRender.h"
#include "test.h"

using namespace glm;


GLFWwindow * InitWindow();
void SetOpenGLState();
void error_callback(int error, const char *description) {
	printf("%s\n", description);
}

int main(void) {
	// init glfw 
	GLFWwindow * window;
	
	if ((window = InitWindow()) == NULL) {
		system("pause");
		return -1;
	}

	SetOpenGLState();
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	mShader camShader("v.shader", "f.shader");
	mShader objShader("v2.shader", "f2.shader");

	mCamera mcam(wndWidth, wndHeight, &camShader);
	if (false == mcam.init()) {
		system("pause");
		return -1;
	}

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// camera matrix
	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

	//model = glm::translate(model, glm::vec3(0.4, 0, 0));
	//model = glm::rotate(model, (float)glm::radians(30.f), glm::vec3(1, 2, 1));
	

	// model matrix
	glm::mat4 MVP = projection*view*model;

	// 所有的mesh 使用同一个投影矩阵
	// 圆柱本身长度为0.1 宽度为0.03 
	mMeshRender meshes(view, projection, &objShader);
	meshes.addMesh("sphere.ply");
	meshes.addMesh("cylinder.ply");
	
	
	// 为模型添加光照信息

	std::vector<float> vertexs({0, 0, 0, 0.5f, 0.5f, 0, 1, 0, 0, 0, 0.3f, 0.5f, -0.5f, 1.f, -0.5f});
	std::vector<unsigned int> indics({0, 1, 1, 2, 2, 3, 3, 4, 4, 0});

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(vertexArrayID);
		mcam.drawFrame();

		meshes.render(vertexs, indics);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteVertexArrays(1, &vertexArrayID);
	glfwTerminate();
	
	return 0;
}

GLFWwindow * InitWindow() {
	glfwSetErrorCallback(error_callback);
	if (false == glfwInit()) {
		std::cout << "glfwInit failed!" << std::endl;
		return NULL;
	}
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);// ? Don't know what's it;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(wndWidth, wndHeight, "3D Pose Estimate", NULL, NULL);
	if (!window) {
		std::cout << "Window create failed!" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSwapInterval(0);

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW\n" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	return window;
}

void SetOpenGLState() {
	// enable depth test and accept fragment if it closer to the camera than the former one
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}