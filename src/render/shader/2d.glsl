R"(
#type vertex
#version 400 core

layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 vTexCoords;

layout (location=3) in mat4 iTransform;
layout (location=7) in vec4 iColor;

uniform mat4 uProjection = mat4(1);

out vec2 fTexCoords;
out vec3 fNormal;
out vec3 fPosition;
out vec4 fColor;

void main(){
	gl_Position = uProjection * iTransform * vec4(vPosition, 1.0);
	fTexCoords = vTexCoords;
	fPosition = vPosition * 2;
	fNormal = normalize(vec3(transpose(inverse(iTransform)) * vec4(vNormal, 0.0)));
	fColor = iColor;
}

#type fragment
#version 400 core

in vec2 fTexCoords;
in vec3 fNormal;
in vec3 fPosition;
in vec4 fColor;

uniform sampler2D uTexture;
uniform float uSize = 1;

out vec4 oColor;

void main(){
	if(fPosition.x * fPosition.x + fPosition.y * fPosition.y > uSize){
		discard;
	}
	oColor = texture(uTexture, fTexCoords) * fColor;
}
)"