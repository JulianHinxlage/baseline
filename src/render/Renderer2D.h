
#pragma once

#include "FrameBuffer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Mesh.h"

namespace baseline {

	class Renderer2D {
	public:
		std::shared_ptr<FrameBuffer> frameBuffer;

		void init(bool useFrameBuffer, int resolutionX = 0, int resolutionY = 0);
		void submitQuad(glm::vec2 pos, glm::vec2 scale, float rotation = 0, Texture* texture = nullptr, Color color = color::white);
		void submitCircle(glm::vec2 pos, glm::vec2 scale, Texture* texture = nullptr, Color color = color::white);
		void begin(const glm::mat4& cameraMatrix = glm::mat4(1), bool clear = true);
		void end();

	private:
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Texture> defaultTexture;
		std::shared_ptr<Mesh> defaultMesh;
		std::shared_ptr<Buffer> instanceBuffer;

		class Instance {
		public:
			glm::mat4 transform;
			glm::vec4 color;
		};
		std::vector<Instance> quadInstances;
		std::vector<Instance> circleInstances;
	};

}