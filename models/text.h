#pragma once

#include "../model.h"
#include "../font.h"


class Text : public Model  {
public:
	Text(const py::kwargs&);
private:
	std::shared_ptr<Font> m_font;
};