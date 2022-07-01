#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <limits>
#include <functional>

#include "symbols.h"



class Shader {
public:
	Shader(ShaderType, const char* vertex, const char* fragment);
	virtual ~Shader();
	virtual void use();
	virtual void setup();
	virtual void setupVertices() = 0;
	void bind();
	void setInt(const std::string &name, int value) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;
	ShaderType getShaderType() const;
private:
	GLuint m_programId;
	GLuint m_vao;
	ShaderType m_shaderType;
};

inline ShaderType Shader::getShaderType() const {
	return m_shaderType;
}

class VCShader : public Shader {
public:
	VCShader(ShaderType, const char* vertex, const char* fragment);
	void setupVertices() override;
};

class VTCShader : public Shader {
public:
	VTCShader(ShaderType, const char* vertex, const char* fragment);
	void setupVertices() override;
};
