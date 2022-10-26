#pragma once

#include "../model.h"
#include "../font.h"

class Text : public Model  {
public:
	explicit Text(const py::kwargs&);
	Text(const std::string& font, const std::string& text, int size);
//	void draw(Shader*, const glm::mat4&) override;
    void setText(const std::string& text);
    glm::vec4 getSize() const;
private:
    glm::vec4 m_textBounds;
	std::shared_ptr<Font> m_font;
	float m_fontSize;
	HorizontalAlign m_halign;
	struct RowInfo {
	    float xMax;
	    size_t endIndex;
	};
};