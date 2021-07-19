#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Position;
out vec3 Normal;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main()
{
	gl_Position = P*V*M* vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	Position = vec3(M*vec4(aPos,1));
	Normal = mat3(transpose(inverse(M)))*aNorm;
}