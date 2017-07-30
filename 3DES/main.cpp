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


using namespace glm;

void error_callback(int error, const char *description) {
	printf("%s\n", description);
}

GLFWwindow * InitWindow();
void SetOpenGLState();

struct Vertex {
	glm::vec3 Position;
};


class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;
	//std::vector<Texture> textures;
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	void setupMesh();
	void draw();
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};



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
	mCamera mcam(wndWidth, wndHeight, &camShader);

	if (false == mcam.init()) {
		system("pause");
		return -1;
	}

	// 加载模型
	//Assimp::Importer importer;
	//const aiScene * sphereScene = importer.ReadFile("./cylinder.ply", aiProcess_Triangulate | aiProcess_FlipUVs);

	//if (!sphereScene || sphereScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !sphereScene->mRootNode) {
	//	std::cout << "ERROR: ASSIMP::" << importer.GetErrorString() << std::endl;
	//	system("pause");
	//	return -1;
	//}
	//aiNode * snode = sphereScene->mRootNode;
	//aiMesh * mesh = sphereScene->mMeshes[snode->mMeshes[0]];
	//
	//std::vector<Vertex> vertices;
	//std::vector<unsigned int> indices;
	//
	//for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
	//	Vertex v;
	//	glm::vec3 tmpv;
	//	tmpv.x = mesh->mVertices[i].x;
	//	tmpv.y = mesh->mVertices[i].y;
	//	tmpv.z = mesh->mVertices[i].z;
	//	v.Position = tmpv; // glm中对象是copy赋值

	//	vertices.push_back(v);
	//}
	//for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
	//	aiFace face = mesh->mFaces[i];

	//	for (unsigned int j = 0; j < face.mNumIndices; j++) {
	//		indices.push_back(face.mIndices[j]);
	//	}
	//}
	//Mesh myMesh(vertices, indices);

	do {
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mcam.drawFrame();

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

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
	this->vertices = vertices;
	this->indices = indices;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//this->textures = textures;
}
void Mesh::setupMesh() {
	// VAO保存了每个绘制过程中的数据
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}
void Mesh::draw() {
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
}