#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>
#include <memory>

#include "window.h"
#include "fs.h"
#include "texture.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    std::cout << "hello world" << std::endl;

    std::shared_ptr<DefaultWindow> window = std::make_shared<DefaultWindow>("Hello world", SCR_WIDTH, SCR_HEIGHT);

    std::vector<std::shared_ptr<Window>> windows{ window };

    Window::runWindows(windows);

    glfwTerminate();

    return 0;
}
