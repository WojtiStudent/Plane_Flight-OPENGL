#version 330 core

in vec3 texCoord;

out vec4 color;

uniform samplerCube cubeTexture;

void main(){
	color = texture(cubeTexture, texCoord);
}


