#pragma once
#include <iostream>
#include "CameraDS.h"
#include "mShader.h"

class mCamera {
public:
	std::tr1::shared_ptr<CCameraDS> m_camera;
	int wndWidth;
	int wndHeight;
	mCamera(int wndWidth, int wndHeight, mShader *camShader);
	~mCamera();
	// 必须在创建 VAO之后调用
	bool init();
	void drawFrame();
private:
	mShader * camShader;
	GLuint vertexBuffer;
	GLuint uvBuffer;
	glm::mat4 MVP;
	GLuint textureID;
	GLuint samplerID;

	void initGLFrame();
	GLuint genTexture();
	void setTextureData(std::tr1::shared_ptr<unsigned char> frame);
};