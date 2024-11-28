#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 outColor;
layout(set = 0 ,binding = 0) uniform s{
	vec3 color;
};
void main(){
	outColor = vec4(color,1.0);
}