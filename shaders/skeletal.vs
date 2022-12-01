R"(
#version 330 core

const int MAX_JOINTS = 50;
const int MAX_WEIGHTS = 3;

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexture;
layout (location = 2) in vec3 vWeight;

out vec2 pass_texCoord;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 local_to_model;
uniform ivec3 weightIndex;
uniform float z;
uniform mat4 Bone[MAX_JOINTS];  // Array of bones that you compute (animate) on the CPU and you upload to the shader

void main()
{
    // convert local to model
    vec4 modelpos = local_to_model * vec4(vPosition, 1.0);
    vec4 totalLocalPos = vec4(0.0);
    vec4 localPosition0 = Bone[weightIndex[0]] * modelpos;
    vec4 localPosition1 = Bone[weightIndex[1]] * modelpos;
    vec4 localPosition2 = Bone[weightIndex[2]] * modelpos;
    totalLocalPos += localPosition0 * vWeight[0] + localPosition1 * vWeight[1] + localPosition2 * vWeight[2];
    totalLocalPos.z = z;//modelpos.z;
    pass_texCoord= vTexture;
    //totalLocalPos = vec4(vPosition, 1.0);
    gl_Position = projection * view * model * totalLocalPos;
    //gl_Position.z =z;
}
)"