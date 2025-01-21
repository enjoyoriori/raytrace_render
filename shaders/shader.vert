#version 450
#extension GL_ARB_separate_shader_objects : enable
// Vertex shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inUV;
layout(location = 4) in vec4 inColor;
layout(location = 5) in vec4 inBoneIDs;
layout(location = 6) in vec4 inBoneWeights;

layout(location = 7) in vec4 inInstanceMatrix0;
layout(location = 8) in vec4 inInstanceMatrix1;
layout(location = 9) in vec4 inInstanceMatrix2;
layout(location = 10) in vec4 inInstanceMatrix3;


void main() {
    gl_Position = vec4(inPos, 1.0);
}