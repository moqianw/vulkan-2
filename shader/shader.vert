#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec3 fragcolor;
layout(location = 0) in vec2 position;
layout(set = 0, binding = 0) uniform matrix{
	mat4 model;
	mat4 proj;
	mat4 view;
};
vec3 colors[3] = vec3[](
	vec3(1.0,0.0,0.0),
	vec3(0.0,1.0,0.0),
	vec3(0.0,0.0,1.0)
);
void main(){
	gl_Position = proj * view * model * vec4(position,0.0,1.0);
	fragcolor = colors[gl_VertexIndex];
}