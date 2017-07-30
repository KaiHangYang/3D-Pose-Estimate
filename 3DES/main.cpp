#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <glew/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CameraDS.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mshader.h"


#define M_PI 3.1415926

using namespace glm;

void error_callback(int error, const char *description) {
	printf("%s\n", description);
}

const int wndWidth = 1280;
const int wndHeight = 960;

void SetTextureData(std::tr1::shared_ptr<unsigned char> frame, GLuint textureID);
void SetOpenGLState();
GLuint GenTexture();
GLFWwindow * InitWindow();

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


std::tr1::shared_ptr<CCameraDS> m_cap = std::tr1::shared_ptr<CCameraDS>(new CCameraDS);


int main(void) {
	// init glfw 
	GLFWwindow * window;
	
	if ((window = InitWindow()) == NULL) {
		system("pause");
		return -1;
	}
	

	if (false == m_cap->OpenCamera(0, false, wndWidth, wndHeight)) {
		std::cout << "摄像头打开失败!" << std::endl;
		system("pause");
		return -1;
	}


	SetOpenGLState();
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	mShader shader1("v.shader", "f.shader");
	mShader shader2("v2.shader", "f2.shader");

	// 这个投影矩阵可能是需要 和摄像机的标定有关系的。
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
	// camera matrix
	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3.62), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 model = glm::mat4(1.0f);
	// model matrix
	glm::mat4 MVP = projection*view*model;

	// Load texture 

	// 加载视频帧
	GLuint texture = GenTexture();
	GLuint textureID = glGetUniformLocation(shader1.ID, "myTextureSampler");

	static const GLfloat g_vertex_buffer_data[] = {
		-2.0f, 1.5f, 0.0f,
		-2.0f, -1.5f, 0.0f,
		2.0f, -1.5f, 0.0f,
		2.0f, -1.5f, 0.0f,
		2.0f, 1.5f, 0.0f,
		-2.0f, 1.5f, 0.0f
	};
	// 通过修改这个uv矩阵可以控制图像显示的效果 比如镜像等。
	static const GLfloat uv_data[] = {
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};
	GLuint vertexBuffer;
	GLuint uvBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_data), uv_data, GL_STATIC_DRAW);

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

		//glBindVertexArray(vertexArrayID);
		shader1.use();
		//传入投影矩阵
		shader1.setVal("MVP", MVP);

		std::tr1::shared_ptr<unsigned char> frame = m_cap->QueryFrame();
		SetTextureData(frame, texture);

		glBindTexture(GL_TEXTURE_2D, texture);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(textureID, 0);

		//// 允许顶点着色器读取GPU的数据
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, (void *)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

		glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
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

GLuint GenTexture() {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	return texture;
}

void SetTextureData(std::tr1::shared_ptr<unsigned char> frame, GLuint texture) {
	unsigned char * data = frame.get();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wndWidth, wndHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	// 下面的东西必须每次在改变texture的之后进行修改
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
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