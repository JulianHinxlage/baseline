
#pragma once

#include "FrameBuffer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Mesh.h"

namespace baseline {

	class SimpleRenderer {
	public:
		std::shared_ptr<FrameBuffer> frameBuffer;

		void init(bool useFrameBuffer, int resolutionX = 0, int resolutionY = 0);
		void submit(glm::vec3 pos, Mesh* mesh = nullptr, Texture* texture = nullptr, Color color = color::white);
		void begin(const glm::mat4& cameraMatrix = glm::mat4(1), const glm::vec3& lightDirection = glm::vec3(0, 0, 1));
		void end();

	private:
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Texture> defaultTexture;
		std::shared_ptr<Mesh> defaultMesh;
	};

}