#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec3 fragcolor;
layout(location = 0) in vec2 position;
layout(set = 0, binding = 1) uniform matrixs{
	mat4 proj;
	mat4 view;
	mat4 model;

};
void main(){
	gl_Position = proj * view * model * vec4(position,0.0,1.0);
}