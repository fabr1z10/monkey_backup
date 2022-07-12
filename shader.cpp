#include <fstream>
#include <sstream>
#include <iostream>
#include "shader.h"


Shader::Shader(ShaderType type, const char* vertexPath, const char* fragmentPath) : m_shaderType(type) {
	std::string vertexCode;
	std::string fragmentCode;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch (std::ifstream::failure& e) {
		std::cout << "Error: shader file " << vertexPath << ", " << fragmentPath << " not successfully read" << std::endl;
		exit(1);
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	GLint result;
	GLuint vid = glCreateShader(GL_VERTEX_SHADER);
	GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vid, 1, &vShaderCode, 0);
	glShaderSource(fid, 1, &fShaderCode, 0);

	glCompileShader(vid);
	glGetShaderiv(vid, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		std::cerr << "Error while compiling vertex shader\n";
		GLint blen;
		glGetShaderiv(vid, GL_INFO_LOG_LENGTH, &blen);
		if (blen > 1) {
			char* compiler_log = (char*)malloc(blen);
			glGetInfoLogARB(vid, blen, 0, compiler_log);
			std::cerr << compiler_log << "\n";
			free(compiler_log);
		}
		glDeleteShader(vid);
		glDeleteShader(fid);
	}

	glCompileShader(fid);
	glGetShaderiv(fid, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		std::cerr << "Error while compiling fragment shader\n";
		GLint blen;
		glGetShaderiv(fid, GL_INFO_LOG_LENGTH, &blen);
		if (blen > 1) {
			char* compiler_log = (char*)malloc(blen);
			glGetInfoLogARB(fid, blen, 0, compiler_log);
			std::cerr << compiler_log << "\n";
			free(compiler_log);
		}
		glDeleteShader(vid);
		glDeleteShader(fid);

	}

	GLuint progId = glCreateProgram();
	glAttachShader(progId, vid);
	glAttachShader(progId, fid);
	glLinkProgram(progId);

	glGetProgramiv(progId, GL_LINK_STATUS, &result);
	if (GL_FALSE == result) {
		std::cerr << "Error while linking program\n";
		GLchar infoLog[1024];
		glGetProgramInfoLog(progId, 1024, NULL, infoLog);
		std::cerr << infoLog << "\n";
		exit(1);

	}
	m_programId = progId;


	//
	std::cout << " --- creating vao\n";
	glGenVertexArrays(1, &m_vao);
}

Shader::~Shader() {
	glDeleteProgram(m_programId);
}

void Shader::use() {
	glUseProgram(m_programId);
	glBindVertexArray(m_vao);
	setupVertices();
}
void Shader::setup() {
	glUseProgram(m_programId);
	glBindVertexArray(m_vao);

}
VCShader::VCShader(ShaderType type, const char* vertex, const char* fragment) : Shader(type, vertex, fragment) {}

void VCShader::setupVertices() {
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 28, (void*)nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 28, (void*)12);
}

VTCShader::VTCShader(ShaderType type, const char* vertex, const char* fragment) : Shader(type, vertex, fragment) {}

void VTCShader::setupVertices() {
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 36, (void*)nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 36, (void*)12);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 36, (void*)20);
}
// ------------------------------------------------------------------------
void Shader:: setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(m_programId, name.c_str()), value);
}

void Shader::bind() {
	glBindVertexArray(m_vao);

}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	auto pippo = glGetUniformLocation(m_programId, name.c_str());
	glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(m_programId, name.c_str()), 1, &value[0]);
}