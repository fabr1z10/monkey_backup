#include "model.h"
#include "asset_manager.h"
#include "engine.h"
#include "components/renderer.h"
#include <iostream>

Model::Model(int shaderType) : m_shaderType(static_cast<ShaderType>(shaderType)), m_primitive(GL_TRIANGLES)
{}

Model::~Model() {
	// Cleanup VBO
	std::cout << "destroy model\n";
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}

void Model::draw(Shader* s, int offset, int size) {
	if (size == 0) size = m_size;
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	s->setupVertices();
	glDrawElements(m_primitive, size, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * offset));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::shared_ptr<Renderer> Model::getRenderer() const {
	return std::make_shared<Renderer>();
}