#pragma once

#include "../model.h"
#include "../font.h"


class Text : public Model  {
public:
	explicit Text(const py::kwargs&);
	Text(const std::string& font, const std::string& text, int size);
	void draw(Shader*, const glm::mat4&) override;
    void setText(const std::string& text);
    glm::vec2 getSize() const;
private:
    glm::vec2 m_textSize;
	std::shared_ptr<Font> m_font;
	GLuint m_texId;
	float m_fontSize;
};