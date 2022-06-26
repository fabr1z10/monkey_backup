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
private:
	GLuint m_programId;
};

