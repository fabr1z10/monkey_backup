#include "model.h"
#include <iostream>

Model::Model()
{}
Model::~Model() {
	// Cleanup VBO
	std::cout << "destroy model\n";
	glDeleteBuffers(1, &m_vbo);
}

RawModel::RawModel(py::array_t<float> vertices) {


	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.nbytes(), vertices.data(0), GL_STATIC_DRAW);


}

void RawModel::draw(Shader * ) {
	// Draw the triangle !
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
}