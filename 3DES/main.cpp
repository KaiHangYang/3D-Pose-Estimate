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

#define M_PI 3.1415926

using namespace glm;

GLuint shaderLoader(const char * vertex_file_path, const char * fragment_file_path);
unsigned char * loadBMP_custom(const char * imagepath);

void error_callback(int error, const char *description) {
	printf("%s\n", description);
}

const int wndWidth = 1280;
const int wndHeight = 960;

void SetTextureData(std::tr1::shared_ptr<unsigned char> frame, GLuint textureID);
void SetOpenGLState();
GLuint GenTexture();
GLFWwindow * InitWindow();

//GLfloat normalize(GLfloat L, GLfloat R, GLfloat x) {
//	return x*(R - L) / L + x;
//}
//
//void splitTriangle(std::vector<GLfloat> &final_vertexs, int maxDepth, float R, int depth, std::vector<GLfloat> & tri) {
//	if (depth >= maxDepth) {
//
//		// Push the point into the final arr
//		for (int i = 0; i < 3; i++) {
//			// normalize
//			GLfloat L = sqrt(pow(tri.at(3 * i + 0), 2) + pow(tri.at(3 * i + 1), 2) + pow(tri.at(3 * i + 2), 2));
//			final_vertexs.push_back(normalize(L, R, tri.at(3 * i + 0))); // x
//			final_vertexs.push_back(normalize(L, R, tri.at(3 * i + 1))); // y
//			final_vertexs.push_back(normalize(L, R, tri.at(3 * i + 2))); // z
//		}
//	}
//	else {
//		std::vector<GLfloat> tr[4];
//
//		for (int i = 0; i < 3; ++i) {
//			int a, b, c;
//			if (i == 0) {
//				a = 1;
//				b = 2;
//			}
//			else if (i == 1) {
//				a = 0;
//				b = 2;
//			}
//			else {
//				a = 0;
//				b = 1;
//			}
//			// a
//			tr[i].push_back(tri.at(3 * i + 0));
//			tr[i].push_back(tri.at(3 * i + 1));
//			tr[i].push_back(tri.at(3 * i + 2));
//			// a+b/2
//			tr[i].push_back((tri.at(3 * i + 0) + tri.at(3 * a + 0)) / 2);
//			tr[i].push_back((tri.at(3 * i + 1) + tri.at(3 * a + 1)) / 2);
//			tr[i].push_back((tri.at(3 * i + 2) + tri.at(3 * a + 2)) / 2);
//			// a+c/2
//			tr[i].push_back((tri.at(3 * i + 0) + tri.at(3 * b + 0)) / 2);
//			tr[i].push_back((tri.at(3 * i + 1) + tri.at(3 * b + 1)) / 2);
//			tr[i].push_back((tri.at(3 * i + 2) + tri.at(3 * b + 2)) / 2);
//		}
//		tr[3].push_back((tri.at(3 * 0 + 0) + tri.at(3 * 1 + 0)) / 2);
//		tr[3].push_back((tri.at(3 * 0 + 1) + tri.at(3 * 1 + 1)) / 2);
//		tr[3].push_back((tri.at(3 * 0 + 2) + tri.at(3 * 1 + 2)) / 2);
//		// a+c/2
//		tr[3].push_back((tri.at(3 * 0 + 0) + tri.at(3 * 2 + 0)) / 2);
//		tr[3].push_back((tri.at(3 * 0 + 1) + tri.at(3 * 2 + 1)) / 2);
//		tr[3].push_back((tri.at(3 * 0 + 2) + tri.at(3 * 2 + 2)) / 2);
//
//		tr[3].push_back((tri.at(3 * 1 + 0) + tri.at(3 * 2 + 0)) / 2);
//		tr[3].push_back((tri.at(3 * 1 + 1) + tri.at(3 * 2 + 1)) / 2);
//		tr[3].push_back((tri.at(3 * 1 + 2) + tri.at(3 * 2 + 2)) / 2);
//
//		for (int i = 0; i < 4; i++) {
//			splitTriangle()
//		}
//	}
//}

void createOctahedron(std::vector<GLfloat> &vertex) {
	// 八面体中间的点
	GLfloat mid[18] = {0.0f, 0.0f, 1.0f,
					  -1.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, -1.0f,
					  1.0f, 0.0f, 0.0f,
					  0.0f, 1.0f, 0.0f,
					  0.0f, -1.0f, 0.0f};
	for (int i = 4; i < 6; i++) {
		for (int j = 0; j < 4; ++j) {
			// put the top point first
			vertex.push_back(mid[3 * i]);
			vertex.push_back(mid[3 * i + 1]);
			vertex.push_back(mid[3 * i + 2]);

			vertex.push_back(mid[3 * j]);
			vertex.push_back(mid[3 * j + 1]);
			vertex.push_back(mid[3 * j + 2]);

			if (j == 3) {
				vertex.push_back(mid[0]);
				vertex.push_back(mid[1]);
				vertex.push_back(mid[2]);
			}
			else {
				vertex.push_back(mid[3 * (j + 1)]);
				vertex.push_back(mid[3 * (j + 1) + 1]);
				vertex.push_back(mid[3 * (j + 1) + 2]);
			}
		}

	}
}

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

	GLuint programID = shaderLoader("v.shader", "f.shader");
	GLuint programID2 = shaderLoader("v2.shader", "f2.shader");

	//  model view projection uniform
	GLuint matrixID = glGetUniformLocation(programID, "MVP");
	GLuint matrixID2 = glGetUniformLocation(programID2, "MVP");
	// 这个投影矩阵可能是需要 和摄像机的标定有关系的。
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
	// camera matrix
	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3.62), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 model = glm::mat4(1.0f);
	// model matrix
	glm::mat4 MVP = projection*view*model;

	// Load texture 
	GLuint texture = GenTexture();
	GLuint textureID = glGetUniformLocation(programID, "myTextureSampler");


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

	///* 尝试渲染球 */
	//GLuint sphereBuffer;

	//std::vector<GLfloat> octVertexs;
	//createOctahedron(octVertexs);

	//glGenBuffers(1, &sphereBuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, sphereBuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* octVertexs.size(), octVertexs.data(), GL_STATIC_DRAW);


	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);
		//传入投影矩阵
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

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
		

		//// 绘制圆
		//glUseProgram(programID2);
		////传入投影矩阵
		//glUniformMatrix4fv(matrixID2, 1, GL_FALSE, &MVP[0][0]);

		//glEnableVertexAttribArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, sphereBuffer);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		//glDrawArrays(GL_TRIANGLES, 0, octVertexs.size()/3);
		//glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteProgram(programID);
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

	// 开启灯光
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
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

GLuint shaderLoader(const char * vertex_file_path, const char * fragment_file_path) {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertex_file_path, std::ios::in);
	if (vertexShaderStream.is_open()) {
		std::string line = "";
		while (getline(vertexShaderStream, line)) {
			vertexShaderCode += "\n" + line;
		}
		vertexShaderStream.close();
	}
	else {
		std::cout << "Open vertex shader file failed!" << std::endl;
		system("pause");
		return 0;
	}

	std::string fragShaderCode;
	std::ifstream fragShaderStream(fragment_file_path, std::ios::in);
	if (fragShaderStream.is_open()) {
		std::string line = "";
		while (getline(fragShaderStream, line)) {
			fragShaderCode += "\n" + line;
		}
		fragShaderStream.close();
	}
	else {
		std::cout << "Open fragment shader file failed!" << std::endl;
		return 0;
	}

	GLint result = GL_FALSE;
	int infoLogLength;
	
	// Compile the shader;
	std::cout << "Compiling shader: " << vertex_file_path << std::endl;
	
	char const * vertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
	glCompileShader(vertexShaderID);
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> vertexShaderErrorMsg(infoLogLength + 1);
		glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMsg[0]);
		printf("%s\n", &vertexShaderErrorMsg[0]);
	}

	std::cout << "Compiling shader: " << fragment_file_path << std::endl;
	char const * fragSourcePointer = fragShaderCode.c_str();
	glShaderSource(fragShaderID, 1, &fragSourcePointer, NULL);
	glCompileShader(fragShaderID);
	glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 0) {
		std::vector<char> fragShaderErrorMsg(infoLogLength + 1);
		glGetShaderInfoLog(fragShaderID, infoLogLength + 1, NULL, &fragShaderErrorMsg[0]);
		printf("%s\n", &fragShaderErrorMsg[0]);
	}

	// Link the program
	printf("Link the shader program!\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragShaderID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 0) {
		std::vector<char> programErrorMsg(infoLogLength + 1);
		glGetShaderInfoLog(programID, infoLogLength + 1, NULL, &programErrorMsg[0]);
		printf("%s\n", programErrorMsg[0]);
	}
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragShaderID);

	return programID;

}


//unsigned char * loadBMP_custom(const char * imagepath){
//
//	printf("Reading image %s\n", imagepath);
//
//	// Data read from the header of the BMP file
//	unsigned char header[54];
//	unsigned int dataPos;
//	unsigned int imageSize;
//	unsigned int width, height;
//	// Actual RGB data
//	unsigned char * data;
//
//	// Open the file
//	FILE * file = fopen(imagepath, "rb");
//	if (!file){
//		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
//		getchar();
//		return 0;
//	}
//
//	// Read the header, i.e. the 54 first bytes
//
//	// If less than 54 bytes are read, problem
//	if (fread(header, 1, 54, file) != 54){
//		printf("Not a correct BMP file\n");
//		fclose(file);
//		return 0;
//	}
//	// A BMP files always begins with "BM"
//	if (header[0] != 'B' || header[1] != 'M'){
//		printf("Not a correct BMP file\n");
//		fclose(file);
//		return 0;
//	}
//	// Make sure this is a 24bpp file
//	if (*(int*)&(header[0x1E]) != 0)         { printf("Not a correct BMP file\n");    fclose(file); return 0; }
//	if (*(int*)&(header[0x1C]) != 24)         { printf("Not a correct BMP file\n");    fclose(file); return 0; }
//
//	// Read the information about the image
//	dataPos = *(int*)&(header[0x0A]);
//	imageSize = *(int*)&(header[0x22]);
//	width = *(int*)&(header[0x12]);
//	height = *(int*)&(header[0x16]);
//
//	// Some BMP files are misformatted, guess missing information
//	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
//	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
//
//	// Create a buffer
//	data = new unsigned char[imageSize];
//
//	// Read the actual data from the file into the buffer
//	fread(data, 1, imageSize, file);
//
//	// Everything is in memory now, the file can be closed.
//	fclose(file);
//
//	//// Create one OpenGL texture
//	//GLuint textureID;
//	//glGenTextures(1, &textureID);
//
//	//// "Bind" the newly created texture : all future texture functions will modify this texture
//	//glBindTexture(GL_TEXTURE_2D, textureID);
//
//	// Give the image to OpenGL
//	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
//
//	// OpenGL has now copied the data. Free our own version
//	//delete[] data;
//
//	//// Poor filtering, or ...
//	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
//
//	//// ... nice trilinear filtering ...
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	//// ... which requires mipmaps. Generate them automatically.
//	//glGenerateMipmap(GL_TEXTURE_2D);
//
//	//// Return the ID of the texture we just created
//	//return textureID;
//	return data;
//}
