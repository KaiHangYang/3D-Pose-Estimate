#include "mMeshRender.h"
#include <string.h>

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
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

}

MeshEntry::MeshEntry(aiMesh * mesh, GLuint vao) {
	VAO = vao;
	// 目前只初始化 Vertex 和 Index缓冲
	VBO[VERTEX_BUFFER] = 0;
	VBO[TEXCOORD_BUFFER] = 0;
	VBO[NORMAL_BUFFER] = 0;
	VBO[INDEX_BUFFER] = 0;

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
		elmNum = indices.size();
	}

	if (mesh->HasNormals()) {
		std::vector<GLfloat> normals;
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			normals.push_back(mesh->mNormals[i].x);
			normals.push_back(mesh->mNormals[i].y);
			normals.push_back(mesh->mNormals[i].z);
		}
		glGenBuffers(1, &VBO[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
		 
}
void MeshEntry::render() {
	glBindVertexArray(VAO);

	if (0 != VBO[VERTEX_BUFFER]) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO[VERTEX_BUFFER]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
		glEnableVertexAttribArray(0);
	}
	if (0 != VBO[NORMAL_BUFFER]) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO[NORMAL_BUFFER]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
		glEnableVertexAttribArray(2);
		
	}
	if (0 != VBO[INDEX_BUFFER]) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[INDEX_BUFFER]);
		glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 0, (void *)0);
		glEnableVertexAttribArray(1);
	}
	glDrawElements(GL_TRIANGLES, elmNum, GL_UNSIGNED_INT, NULL); // 如果没错的话最后一个参数为NULL的话要从 GL_ELEMENT_ARRAY_BUFFER读取数据
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}

void mMeshRender::addMesh(std::string mesh_path) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(mesh_path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR: ASSIMP::" << importer.GetErrorString() << std::endl;
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		meshEntries.push_back(new MeshEntry(scene->mMeshes[i], VAO));
	}

}
mMeshRender::mMeshRender(glm::mat4 &view, glm::mat4 &proj, mShader * shader) {
	// 初始化过程初始一些关键的矩阵
	glGenVertexArrays(1, &VAO);
	VP = proj*view;
	this->shader = shader;
}
mMeshRender::~mMeshRender() {
	for (unsigned int i = 0; i < meshEntries.size(); ++i) {
		delete meshEntries[i];
	}
	meshEntries.clear();
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
}
void mMeshRender::render(std::vector<float> vertexs, std::vector<unsigned int> indices){
	// 这里是渲染给定点坐标和连线的indices 的模型
	glBindVertexArray(VAO);
	const unsigned int vertexNum = vertexs.size() / 3;
	const unsigned int lineNum = indices.size() / 2;
	bool * vertexFlags = new bool[vertexNum];
	memset(vertexFlags, 0, sizeof(bool)*vertexNum);
	// 接下来进行绘制，先使用相应的shader
	shader->use();
	shader->setVal("viewPos", glm::vec3(0, 0, 3));
	shader->setVal("lightPos", glm::vec3(10, 10, 10)); // 设置光源
	shader->setVal("fragColor", glm::vec3(0, 0, 1));
	for (unsigned int i = 0; i < lineNum; ++i) {
		unsigned int line[2] = { indices[2 * i], indices[2 * i + 1] };

	
		// 先把两个点画出来
		for (unsigned int j = 0; j < 2; ++j) {
			if (!vertexFlags[line[j]]) {
				vertexFlags[line[j]] = true;
				// 暂时认为 加载的第一个模型为圆球，第二个为圆柱
				
				// 创建变化矩阵，点的话只包含平移
				// 这里假设输入的为直接可以平移的数据

				glm::mat4 model(1.0f);
				model = glm::translate(model, glm::vec3(vertexs[3*line[j]], vertexs[3 * line[j] + 1], vertexs[3 * line[j] + 2]));
				glm::mat4 MVP = VP * model;
				shader->setVal("MVP", MVP);
				shader->setVal("modelMat", model);
				shader->setVal("normMat", glm::transpose(glm::inverse(model)));
				meshEntries[0]->render();
			}
		}
		// 然后画横线

		// 计算线的中心 圆柱的长度为0.1 根据此进行scale
		float lineCen[3] = {(vertexs[3 * line[0]] + vertexs[3 * line[1]]) / 2.0f, (vertexs[3 * line[0] + 1] + vertexs[3 * line[1] +1]) / 2.0f, (vertexs[3 * line[0] + 2] + vertexs[3 * line[1] + 2]) / 2.0f };
		float length = sqrt(pow((vertexs[3 * line[0]] - vertexs[3 * line[1]]), 2) + pow((vertexs[3 * line[0] + 1] - vertexs[3 * line[1] + 1]), 2) + pow((vertexs[3 * line[0] + 2] - vertexs[3 * line[1] + 2]), 2));

		glm::mat4 model(1.0f);

		// 求点乘得到两个向量夹角

		glm::vec3 vFrom(0, 1, 0);
		glm::vec3 vTo = glm::normalize(glm::vec3((vertexs[3 * line[0]] - vertexs[3 * line[1]]), (vertexs[3 * line[0] + 1] - vertexs[3 * line[1] + 1]), (vertexs[3 * line[0] + 2] - vertexs[3 * line[1] + 2])));

		// 涉及到 已知初始向量和最终向量求旋转过程。可以考虑点乘和×乘的问题

		model = glm::translate(model, glm::vec3(lineCen[0], lineCen[1], lineCen[2]));
		// 注意：两个方向重合的向量的叉乘是没有意义的
		float angle = (float)glm::acos(glm::dot(vFrom, vTo));
		if (angle <= 0.0000001) {
			model = model;
		}
		else {
			model = glm::rotate(model, angle, glm::normalize(glm::cross(vFrom, vTo)));
		}
		model = glm::scale(model, glm::vec3(1, length/0.1, 1));
		
		glm::mat4 MVP = VP * model;
		shader->setVal("MVP", MVP);
		shader->setVal("modelMat", model);
		shader->setVal("fragColor", glm::vec3(1, 0, 0));
		shader->setVal("normMat", glm::transpose(glm::inverse(model)));
		meshEntries[1]->render();
		shader->setVal("fragColor", glm::vec3(0, 0, 1));
	}

}