#include "mMeshRender.h"

MeshEntry::~MeshEntry() {
	glBindVertexArray(VAO);
	if (VBO[VERTEX_BUFFER]) {
		glDeleteBuffers(1, &VBO[VERTEX_BUFFER]);
	}

	if (VBO[TEXCOORD_BUFFER]) {
		glDeleteBuffers(1, &VBO[TEXCOORD_BUFFER]);
	}

	if (VBO[NORMAL_BUFFER]) {
		glDeleteBuffers(1, &VBO[NORMAL_BUFFER]);
	}

	if (VBO[INDEX_BUFFER]) {
		glDeleteBuffers(1, &VBO[INDEX_BUFFER]);
	}
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
}

MeshEntry::MeshEntry(aiMesh * mesh) {
	// 目前只初始化 Vertex 和 Index缓冲
	VBO[VERTEX_BUFFER] = 0;
	VBO[TEXCOORD_BUFFER] = 0;
	VBO[NORMAL_BUFFER] = 0;
	VBO[INDEX_BUFFER] = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	
	if (mesh->HasPositions()) {
		std::vector<GLfloat> vertexs;
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			vertexs.push_back(mesh->mVertices[i].x);
			vertexs.push_back(mesh->mVertices[i].y);
			vertexs.push_back(mesh->mVertices[i].z);
		}
		
		glGenBuffers(1, &VBO[VERTEX_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, vertexs.size()*sizeof(GLfloat), &vertexs[0], GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));

		glEnableVertexAttribArray(0);

	}
	
	if (mesh->HasFaces()) {
		// 注意index 需要使用uint
		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}
		glGenBuffers(1, &VBO[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 0, (void *)0);
		glEnableVertexAttribArray(1);

		elmNum = indices.size();
	}


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
		 
}
void MeshEntry::render() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, elmNum, GL_UNSIGNED_INT, NULL); // 如果没错的话最后一个参数为NULL的话要从 GL_ELEMENT_ARRAY_BUFFER读取数据
	glBindVertexArray(0);
}

mMeshRender::mMeshRender(std::string mesh_path) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(mesh_path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR: ASSIMP::" << importer.GetErrorString() << std::endl;
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		meshEntries.push_back(new MeshEntry(scene->mMeshes[i]));
	}

}

mMeshRender::~mMeshRender() {
	for (unsigned int i = 0; i < meshEntries.size(); ++i) {
		delete meshEntries[i];
	}
	meshEntries.clear();
}
void mMeshRender::render() {
	for (unsigned int i = 0; i < meshEntries.size(); ++i) {
		meshEntries.at(i)->render();
	}
}