#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <limits>
#include <functional>




class Shader {
public:
	Shader(const char* vertex, const char* fragment);
	virtual ~Shader();
	virtual void use();
	virtual void setupVertices() = 0;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;
private:
	GLuint m_programId;
	GLuint m_vao;
};

class VCShader : public Shader {
public:
	VCShader(const char* vertex, const char* fragment);
	void setupVertices() override;
};

