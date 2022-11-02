#include <iostream>
#include "shader.h"

std::unordered_map<char, std::pair<GLenum, size_t>> Shader::m_types {
    {'b', {GL_BYTE, sizeof(GLbyte)}},
    {'B', {GL_UNSIGNED_BYTE, sizeof(GLubyte)}},
    {'s', {GL_SHORT, sizeof(GLshort)}},
    {'B', {GL_UNSIGNED_SHORT, sizeof(GLushort)}},
    {'i', {GL_INT, sizeof(GLint)}},
    {'I', {GL_UNSIGNED_INT, sizeof(GLuint)}},
    {'f', {GL_FLOAT, sizeof(GLfloat)}},
    {'d', {GL_DOUBLE, sizeof(GLdouble)}}
};


Shader::Shader(ShaderType type, const std::string& vertexCode, const std::string& fragmentCode,
               const std::string& vertexFormat) : m_shaderType(type), m_stride(0) {

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

	size_t start = 0;

    for (size_t i = 0; i < vertexFormat.size(); ++i) {
        if (std::isalpha(vertexFormat[i])) {
            auto size = std::stoi(vertexFormat.substr(start, i-start));
            const auto& typeInfo = m_types.at(vertexFormat[i]);
            m_vertexFormat.emplace_back(VertexInfo{size, typeInfo.first, size * typeInfo.second});
            start = i+1;
            m_stride += size * typeInfo.second;
        }
    }
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
	for (const auto& init : m_initializers) {
	    init->init(this);
	}
}


void Shader::setupVertices() {
    GLuint i{0};
    unsigned long ptr{0};
    for (const auto& vertex : m_vertexFormat) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, vertex.size, GL_FLOAT, GL_FALSE, m_stride, (void*)ptr);
        ptr += vertex.byteSize;
        i++;
    }
}

//VCShader::VCShader(ShaderType type, const std::string& vertexCode, const std::string& fragmentCode) : Shader(type, vertexCode, fragmentCode) {}
//
//void VCShader::setupVertices() {
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 28, (void*)nullptr);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 28, (void*)12);
//}
//
//VTCShader::VTCShader(ShaderType type, const std::string& vertexCode, const std::string& fragmentCode) : Shader(type, vertexCode, fragmentCode) {}
//
//void VTCShader::setupVertices() {
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 36, (void*)nullptr);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 36, (void*)12);
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 36, (void*)20);
//}
// ------------------------------------------------------------------------
void Shader:: setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(m_programId, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(m_programId, name.c_str()), value);
}

void Shader::bind() {
	glBindVertexArray(m_vao);

}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	auto pippo = glGetUniformLocation(m_programId, name.c_str());
	glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    auto pippo = glGetUniformLocation(m_programId, name.c_str());
    glUniformMatrix3fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(m_programId, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(m_programId, name.c_str()), 1, &value[0]);
}

void Shader::addInitializer(std::shared_ptr<ShaderInitializer> i) {
    m_initializers.push_back(i);
}

void Shader::addPredraw(std::shared_ptr<ShaderPreDraw> i) {
    m_preDraw.push_back(i);
}

void Shader::preDraw(Node * node) {
    for (const auto& p : m_preDraw) {
        p->init(this, node);
    }
}