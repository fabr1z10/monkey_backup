#pragma once


#include "shader.h"
#include "component.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class Renderer;

class Model {
public:
	Model();
	Model(int, GLuint primitive = GL_TRIANGLES);
	virtual void init(Node*) {}
	//virtual void draw(Shader*, const glm::mat4&);
	virtual void draw(Shader* shader, int offset, int count);
	virtual ~Model();
	// ** a model can be shared by multiple nodes. **
	// so a model does not contain information relevant on how to draw it
	// for instance a sprite model does not contain current animation and frame
	// but this stuff goes into the sprite renderer! When you assign a model to
	// a node, a renderer is generated and the model is linked to the renderer.
	// (renederer has a pointer to the model, not the node!)
	virtual std::shared_ptr<Renderer> getRenderer() const;
	void generateBuffers(const std::vector<float>& vertices, const std::vector<unsigned>& indices);
	ShaderType getShaderType() const;
    GLuint getElSize() const;
protected:
	ShaderType m_shaderType;
	GLuint m_size;
	GLuint m_vbo;
	GLuint m_ebo;
	GLuint m_primitive;
	GLuint m_elementSize;
    GLuint m_texId;
    GLuint m_paletteId;

};

inline GLuint Model::getElSize() const {
    return m_size;
}

inline ShaderType Model::getShaderType() const {
	return m_shaderType;
}

