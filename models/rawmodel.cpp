#include "rawmodel.h"
#include "../engine.h"
#include "../asset_manager.h"

RawModel::RawModel(int shaderType, py::array_t<float> vertices, py::array_t<unsigned> elements, const py::kwargs& kwargs) : Model(shaderType),
																															m_texId(GL_INVALID_VALUE) {

	auto& engine = Engine::instance();
	//auto shader = engine.getShader(static_cast<ShaderType>(shaderType));
	//shader->setup();

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.nbytes(), vertices.data(0), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.nbytes(), elements.data(0), GL_STATIC_DRAW);

	//shader->setupVertices();


	m_elementSize = elements.size();
	m_size = m_elementSize;

	if (kwargs) {
		m_primitive = (kwargs.contains("prim") ? kwargs["prim"].cast<GLuint>() : GL_TRIANGLES);
		if (kwargs.contains("tex")) {
			auto texFile = kwargs["tex"].cast<std::string>();
			auto& am = AssetManager::instance();
			auto tex = am.getTex(texFile);
			m_texId = tex->getTexId();

		}

	}

}

void RawModel::draw(Shader* s, int offset, int size) {

	if (m_texId != GL_INVALID_VALUE) {
		s->setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texId);
	}

	Model::draw(s, offset, size);



}