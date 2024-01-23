
#include "SimpleRenderer.h"
#include <glm/gtc/matrix_transform.hpp>

namespace baseline {

	void SimpleRenderer::init(bool useFrameBuffer, int resolutionX, int resolutionY) {
		shader = std::make_shared<Shader>();
		shader->loadFromSource(R"(
#type vertex
#version 400 core

layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 vTexCoords;

uniform mat4 uProjection = mat4(1);
uniform mat4 uTransform = mat4(1);

out vec2 fTexCoords;
out vec3 fNormal;

void main(){
	gl_Position = uProjection * uTransform * vec4(vPosition, 1.0);
	fTexCoords = vTexCoords;
	fNormal = normalize(vec3(transpose(inverse(uTransform)) * vec4(vNormal, 0.0)));
}

#type fragment
#version 400 core

in vec2 fTexCoords;
in vec3 fNormal;

uniform sampler2D uTexture;
uniform vec4 uColor = vec4(1);
uniform vec3 uLightDirection = vec3(0.5, -0.6, 0.7);

out vec4 oColor;

void main(){
	float ndotl = max(0.0, dot(fNormal, normalize(-uLightDirection)));
	float attenuation = ndotl * 0.4 + 0.6;
	oColor = texture(uTexture, fTexCoords) * uColor * attenuation;
}
		)");

		defaultTexture = std::make_shared<Texture>();
		defaultTexture->create(1, 1);
		Color color = color::white;
		defaultTexture->setData(&color, 1);

		defaultMesh = std::make_shared<Mesh>();
		float vertecies[] = {
			-0.5f, -0.5f, 0.0f,  0, 0, -1,   0, 0,
			+0.5f, -0.5f, 0.0f,  0, 0, -1,   1, 0,
			+0.5f, +0.5f, 0.0f,  0, 0, -1,   1, 1,
			-0.5f, +0.5f, 0.0f,  0, 0, -1,   0, 1,
		};
		int32_t indices[] = {
			0, 1, 2,
			0, 2, 3,
		};
		defaultMesh->create(vertecies, sizeof(vertecies) / 4, indices, sizeof(indices) / 4);

		if (useFrameBuffer) {
			frameBuffer = std::make_shared<FrameBuffer>();
			frameBuffer->init(resolutionX, resolutionY, {
					{ TextureAttachment::COLOR, color::white},
					{ TextureAttachment::DEPTH},
				}
			);
		}
	}

	void SimpleRenderer::submit(glm::vec3 pos, Mesh* mesh, Texture* texture, Color color) {
		if (!texture) {
			texture = defaultTexture.get();
		}
		if (!mesh) {
			mesh = defaultMesh.get();
		}
		texture->bind(0);
		shader->bind();
		shader->set("uTexture", 0);
		shader->set("uColor", color.vec());
		shader->set("uTransform", glm::translate(glm::mat4(1), pos));
		mesh->vertexArray.submit();
		texture->unbind();
	}

	void SimpleRenderer::begin(const glm::mat4& cameraMatrix, const glm::vec3& lightDirection) {
		if (frameBuffer) {
			frameBuffer->bind();
			frameBuffer->clear();
		}
		shader->set("uProjection", cameraMatrix);
		shader->set("uLightDirection", lightDirection);
	}

	void SimpleRenderer::end() {
		if (frameBuffer) {
			frameBuffer->unbind();
		}
	}

}
