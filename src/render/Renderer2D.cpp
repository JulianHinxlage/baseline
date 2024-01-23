
#include "Renderer2D.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>

#define STR(x) #x

const char* shaderSource =
{
#include "shader/2d.glsl"
};

namespace baseline {

	void Renderer2D::init(bool useFrameBuffer, int resolutionX, int resolutionY) {
		shader = std::make_shared<Shader>();
		shader->loadFromSource(shaderSource);

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


		instanceBuffer = std::make_shared<Buffer>();
		instanceBuffer->init(nullptr, 0, 1, BufferType::VERTEX_BUFFER, true);
		defaultMesh->vertexArray.addVertexBuffer(instanceBuffer, {
			{Type::FLOAT, 4},
			{Type::FLOAT, 4},
			{Type::FLOAT, 4},
			{Type::FLOAT, 4},
			{Type::FLOAT, 4},
		}, 1);

		if (useFrameBuffer) {
			frameBuffer = std::make_shared<FrameBuffer>();
			frameBuffer->init(resolutionX, resolutionY, {
					{ TextureAttachment::COLOR, color::white},
					{ TextureAttachment::DEPTH},
				}
			);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Renderer2D::submitQuad(glm::vec2 pos, glm::vec2 scale, float rotation, Texture* texture, Color color) {
		glm::mat4 transform(1);
		transform = glm::translate(transform, glm::vec3(pos, 0));
		transform = glm::scale(transform, glm::vec3(scale, 0));
		transform = glm::rotate(transform, rotation, glm::vec3(0, 0, 1));

		Instance instance;
		instance.transform = transform;
		instance.color = color.vec();
		quadInstances.push_back(instance);
	}

	void Renderer2D::submitCircle(glm::vec2 pos, glm::vec2 scale, Texture* texture, Color color) {
		glm::mat4 transform(1);
		transform = glm::translate(transform, glm::vec3(pos, 0));
		transform = glm::scale(transform, glm::vec3(scale, 0));

		Instance instance;
		instance.transform = transform;
		instance.color = color.vec();
		circleInstances.push_back(instance);
	}

	void Renderer2D::begin(const glm::mat4& cameraMatrix, bool clear) {
		if (frameBuffer) {
			frameBuffer->bind();
			if (clear) {
				frameBuffer->clear();
			}
		}
		shader->set("uProjection", cameraMatrix);
	}

	void Renderer2D::end() {
		if (circleInstances.size() != 0) {
			shader->set("uSize", 1.0f);
			instanceBuffer->setData(circleInstances.data(), circleInstances.size() * sizeof(Instance), 0);
			shader->bind();
			defaultTexture->bind(0);
			shader->set("uTexture", 0);
			defaultMesh->vertexArray.submit(-1, circleInstances.size());
			circleInstances.clear();
			defaultTexture->unbind();
		}
		if (quadInstances.size() != 0) {
			shader->set("uSize", 4.0f);
			instanceBuffer->setData(quadInstances.data(), quadInstances.size() * sizeof(Instance), 0);
			shader->bind();
			defaultTexture->bind(0);
			shader->set("uTexture", 0);
			defaultMesh->vertexArray.submit(-1, quadInstances.size());
			quadInstances.clear();
			defaultTexture->unbind();
		}
		if (frameBuffer) {
			frameBuffer->unbind();
		}
	}

}
