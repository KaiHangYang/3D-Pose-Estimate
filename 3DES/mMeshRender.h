#pragma once
#include <iostream>
#include <vector>
#include <glew/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class MeshEntry {
public:
	enum BUFFERS {
		VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER,INDEX_BUFFER
	};
	GLuint VAO;
	GLuint VBO[4];
	unsigned int elmNum;
	MeshEntry(aiMesh * mesh);
	~MeshEntry();

	void render();
};

class mMeshRender {
public:

	std::vector<MeshEntry *> meshEntries;

	mMeshRender(std::string mesh_path);
	~mMeshRender();

	void render();
};