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
	bool hasPalette() const;
private:
    void load_png(const std::string& file);
    void load_generic(const std::string& file);
	int m_width;
	int m_height;
	GLuint m_texId;

	bool m_palette;
};

inline int Tex::getWidth () const { return m_width; }
inline int Tex::getHeight() const { return m_height; }
inline GLuint Tex::getTexId() const { return m_texId; }
inline bool Tex::hasPalette() const { return m_palette; }

