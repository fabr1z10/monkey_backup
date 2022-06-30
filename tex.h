#pragma once

#include <GL/glew.h>
#include <string>

enum TexFilter { nearest, linear, linear_mipmap };

class Tex {
public:
	Tex (const std::string& file);
	~Tex () ;
	int getWidth () const;
	int getHeight() const;
	GLuint getTexId() const;
private:
	int m_width;
	int m_height;
	GLuint m_texId;
};

inline int Tex::getWidth () const { return m_width; }
inline int Tex::getHeight() const { return m_height; }
inline GLuint Tex::getTexId() const { return m_texId; }


