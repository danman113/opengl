#pragma once
#include <glm/mat4x4.hpp>
#include "shader.h"
class Window;

struct Camera {
	glm::vec3 position = { 0.0f, 0.0f, -1.0f };
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	virtual void updateProjectionMatrix(const Window& w) = 0;

	void updateView() {
		view = glm::translate(glm::mat4(1.0f), position);
	}

	void applyToShader(ShaderProgram& p) {
		p.setUniformMat4("projection", projection)->setUniformMat4("view", view);
	}
};

struct PerspectiveCamera : public Camera {
	virtual void updateProjectionMatrix(const Window& window) override;
};