#include <locale>
#include <codecvt>
#include <iostream>
#include "text.h"
#include "../asset_manager.h"


Text::Text(const py::kwargs& args)  : Model(ShaderType::SHADER_TEXTURE) {
	auto fontId = args["font"].cast<std::string>();
	auto text = args["text"].cast<std::string>();
	auto size = args["size"].cast<float>();
	auto& am = AssetManager::instance();
	m_font = am.getFont(fontId);

	auto sss = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t >().from_bytes(text.c_str());
	for (char32_t c : sss) {
		std::cout << c << "\n";
	}
	exit(1);
}