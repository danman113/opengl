#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
using std::string;

class Window {
    static inline int windowsOpen = 0;
    static inline int windowsCreated = 0;
    static inline int activeWindow = -1;
    static inline bool initialized = false;
public:
    int id;
    string Name;
    unsigned int Width;
    unsigned int Height;
    GLFWwindow* window;

    Window() = delete;
    Window(string windowName, unsigned int w, unsigned int h): Name(windowName), Width(w), Height(h) {
        id = windowsCreated++;
        windowsOpen++;
        std::cout << "Creating window " << id << std::endl;
        if (!initialized) {
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
        window = glfwCreateWindow(Width, Height, Name.c_str(), NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            return;
        }
        if (!initialized) {
            if (!getActive()) makeActive();
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD" << std::endl;
                return;
            } else {
                std::cout << "Initialized glad" << std::endl;
            }
            initialized = true;
        }
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
            Window* self = (Window*)glfwGetWindowUserPointer(window);
            self->Width = width;
            self->Height = height;
            self->onResize(window, width, height);
        });
        glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
            Window* self = (Window*)glfwGetWindowUserPointer(window);
            self->closeWindow();
            self->onClose();
        });
    }

    static void runWindows(std::vector<std::shared_ptr<Window>>& windows) {
        while(windowsOpen > 0) {
            std::cout << windowsOpen << std::endl;
            for (auto& window: windows) {
                if (!glfwWindowShouldClose(window->window)) {
                    window->makeActive();
                    window->run();
                }
            }
            glfwPollEvents();
        }
    }

    virtual void run() {
        update(window);
        draw();
        glfwSwapBuffers(window);
    }

    virtual void draw() = 0;

    virtual void update(GLFWwindow *window) = 0;

    virtual void onClose() = 0;

    virtual void onResize(GLFWwindow* window, int w, int h) = 0;

protected:

    virtual void closeWindow() {
        glfwSetWindowShouldClose(window, true);
        windowsOpen--;
    }

    virtual bool getActive() {
        return id == activeWindow;
    }

    virtual void makeActive() {
        activeWindow = id;
        glfwMakeContextCurrent(window);
    }
};

class DefaultWindow : public Window {
public:
    DefaultWindow(string windowName, unsigned int w, unsigned int h): Window(windowName, w, h) {};
    virtual void draw() {
        float red = 0.2;
        float redVelocity = 0.001f;    

        if (red + redVelocity > 1.0f || red + redVelocity < 0.0f)
            redVelocity = -redVelocity;
        red = std::clamp(red + redVelocity, 0.0f, 1.0f);
        glClearColor(red, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    virtual void update(GLFWwindow *window) {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            std::cout << "killing window" << id << std::endl;
            closeWindow();
        }
    }

    virtual void onClose() {
        std::cout << "Attempting to close window " << id << std::endl;
    }

    virtual void onResize(GLFWwindow* window, int w, int h) {
        std::cout << "New size: " << Width << " x " << Height << std::endl;
        glViewport(0, 0, Width, Height);
    }
};
