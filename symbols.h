#pragma once


enum ShaderType {
	SHADER_COLOR = 0,
	SHADER_TEXTURE,
	SHADER_SKELETAL,
	SHADER_TEXTURE_PALETTE,
	SHADER_TEXTURE_LIGHT,
	NONE
};

enum HorizontalAlign {
    LEFT = 0,
    CENTER,
    RIGHT
};

enum FillType {
	OUTLINE = 0,
	SOLID
};
