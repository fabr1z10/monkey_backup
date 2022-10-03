#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <limits>
#include <functional>

#include "symbols.h"

struct VertexInfo {
    int size;
    GLenum type;
    unsigned long byteSize;
};

class Shader {
public:
	Shader(ShaderType, const std::string& vertexCode, const std::string& fragmentCode, const std::string& vertexFormat);
	virtual ~Shader();
	virtual void use();
	virtual void setup();
	virtual void setupVertices();
	void bind();
	void setInt(const std::string &name, int value) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;
	void setVec4(const std::string &name, const glm::vec4 &value) const;
	ShaderType getShaderType() const;
	GLuint getProgId() const;
private:
	GLuint m_programId;
	GLuint m_vao;
	ShaderType m_shaderType;
	std::vector<VertexInfo> m_vertexFormat;
	GLsizei m_stride;
	static std::unordered_map<char, std::pair<GLenum, size_t>> m_types;
};

inline GLuint Shader::getProgId() const {
    return m_programId;
}

inline ShaderType Shader::getShaderType() const {
	return m_shaderType;
}

//class VCShader : public Shader {
//public:
//	VCShader(ShaderType, const std::string& vertexCode, const std::string& fragmentCode);
//	void setupVertices() override;
//};
//
//class VTCShader : public Shader {
//public:
//	VTCShader(ShaderType, const std::string& vertexCode, const std::string& fragmentCode);
//	void setupVertices() override;
//};
