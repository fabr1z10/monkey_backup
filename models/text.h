#pragma once

#include "../model.h"
#include "../font.h"


class Text : public Model  {
public:
	Text(const py::kwargs&);
	void draw(Shader*, int, int) override;
    void setText(const std::string& text);
private:
	std::shared_ptr<Font> m_font;
	GLuint m_texId;
	float m_fontSize;
};