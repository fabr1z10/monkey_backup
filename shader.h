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

class Shader;
class Node;

class ShaderInitializer {
public:
    virtual void init(Shader*) = 0;
};

class ShaderPreDraw {
public:
    virtual void init(Shader*, Node*) = 0;
};

class Shader {
public:
	Shader(ShaderType, const std::string& vertexCode, const std::string& fragmentCode, const std::string& vertexFormat);
	~Shader();
	void use();
	void setupVertices();
	void bind();
	void setInt(const std::string &name, int value) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
	ShaderType getShaderType() const;
	GLuint getProgId() const;
	void preDraw(Node*);
	void addInitializer(std::shared_ptr<ShaderInitializer>);
    void addPredraw(std::shared_ptr<ShaderPreDraw>);

private:
	GLuint m_programId;
	GLuint m_vao;
	ShaderType m_shaderType;
	std::vector<VertexInfo> m_vertexFormat;
	GLsizei m_stride;
	static std::unordered_map<char, std::pair<GLenum, size_t>> m_types;
	std::vector<std::shared_ptr<ShaderInitializer>> m_initializers;
    std::vector<std::shared_ptr<ShaderPreDraw>> m_preDraw;
};

inline GLuint Shader::getProgId() const {
    return m_programId;
}

inline ShaderType Shader::getShaderType() const {
	return m_shaderType;
}


