#include "mCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glew/glew.h>

mCamera::~mCamera() {
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
}
mCamera::mCamera(int wndWidth, int wndHeight, mShader *camShader) {
	this->wndWidth = wndWidth;
	this->wndHeight = wndHeight;
	this->camShader = camShader;
	glGenVertexArrays(1, &VAO);

	m_camera = std::tr1::shared_ptr<CCameraDS>(new CCameraDS);
}
bool mCamera::init() {
	glBindVertexArray(VAO);
	bool result = m_camera->OpenCamera(0, false, wndWidth, wndHeight);
	if (false == result) {
		std::cout << "ERROR: Camera init failed!" << std::endl;
		return false;
	}
	initGLFrame();
	glBindVertexArray(0);
	return true;
}

void mCamera::initGLFrame() {

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// camera matrix
	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3.62), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 model = glm::mat4(1.0f);
	// model matrix
	MVP = projection*view*model;

	textureID = genTexture();

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

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_data), uv_data, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
}
GLuint mCamera::genTexture() {

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	return texture;
}
void mCamera::setTextureData(std::tr1::shared_ptr<unsigned char> frame) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wndWidth, wndHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, frame.get());
	glGenerateMipmap(GL_TEXTURE_2D);
}
void mCamera::drawFrame() {
	glBindVertexArray(VAO);

	camShader->use();
	camShader->setVal("MVP", MVP);
	std::tr1::shared_ptr<unsigned char> frame = m_camera->QueryFrame();
	setTextureData(frame);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	camShader->setVal("myTextureSampler", 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, 6); // 第三个参数的含义是 顶点的数目

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}
