#include "camera.h"
#include "window.h"

void PerspectiveCamera::updateProjectionMatrix(const Window& window) {
	float aspectRatio = (float)window.Width / (float)window.Height;
	projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1000.0f, 1000.0f);
}