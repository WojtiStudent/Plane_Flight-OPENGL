#version 330 core

layout(location = 0) in vec3 position;

out vec3 texCoord;

uniform mat4 P;
uniform mat4 V;

void main(){
	texCoord = position * vec3(1,-1,1);
	vec4 almostSkybox = P * V * vec4(position,1);
	gl_Position = almostSkybox.xyww;
}
