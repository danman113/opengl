#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_set>
#include "renderer.h"
using std::string;
using std::cout;
using std::endl;
using std::shared_ptr;
using std::unordered_set;

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
    unordered_set<int> keyPressed;
    unordered_set<int> keyReleased;
    unordered_set<int> mousePressed;
    unordered_set<int> mouseReleased;
    double mouseX;
    double mouseY;

    Window(string windowName, unsigned int w, unsigned int h) : Name(windowName), Width(w), Height(h) {
        id = windowsCreated++;
        if (!initialized) {
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
        window = glfwCreateWindow(Width, Height, Name.c_str(), NULL, NULL);
        if (window == NULL) {
            cout << "Failed to create GLFW window" << endl;
            return;
        }
        if (!initialized) {
            glfwMakeContextCurrent(window);
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                cout << "Failed to initialize GLAD" << endl;
                return;
            }
            else {
                cout << "Initialized glad" << endl;
            }
        }

        initialized = true;
        windowsOpen++;
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
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Window* self = (Window*)glfwGetWindowUserPointer(window);
            if (action == GLFW_RELEASE) {
                self->keyReleased.insert(key);
                self->keyPressed.erase(key);
            }
            else if (action == GLFW_PRESS) {
                self->keyPressed.insert(key);
            }
            self->onKeyPressed(key, scancode, action, mods);
        });
        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
            Window* self = (Window*)glfwGetWindowUserPointer(window);
            if (action == GLFW_RELEASE) {
                self->mouseReleased.insert(button);
                self->mousePressed.erase(button);
            }
            else if (action == GLFW_PRESS) {
                self->mousePressed.insert(button);
            }
        });
    }
    ~Window() {
        if (window != NULL)
            glfwDestroyWindow(window);
    }

    static void runWindows(std::vector<shared_ptr<Window>> windows) {
        while (windowsOpen > 0) {
            for (auto& window : windows) {
                if (window->window != nullptr && !glfwWindowShouldClose(window->window)) {
                    window->makeActive();
                    window->run();
                }
            }
            glfwPollEvents();
        }
    }

    virtual void run() {
        update(window);
        glfwGetCursorPos(window, &mouseX, &mouseY);
        draw();
        clearReleased();
        glfwSwapBuffers(window);
    }

    virtual bool getKeyPressed(int key) {
        return keyPressed.find(key) != keyPressed.end();
    }

    virtual bool getKeyDown(int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    virtual bool getKeyReleased(int key) {
        return keyReleased.find(key) != keyReleased.end();
    }

    virtual bool getMousePressed(int button) {
        return mousePressed.find(button) != mousePressed.end();
    }

    virtual bool getMouseDown(int button) {
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    virtual bool getMouseReleased(int button) {
        return mouseReleased.find(button) != mouseReleased.end();
    }

    virtual void onKeyPressed(int key, int scancode, int action, int mods) { };

    virtual void draw() = 0;

    virtual void update(GLFWwindow* window) = 0;

    virtual void onClose() = 0;

    virtual void onResize(GLFWwindow* window, int w, int h) = 0;

    virtual void closeWindow() {
        glfwSetWindowShouldClose(window, true);
        windowsOpen--;
    }

    virtual bool getActive() {
        return id == activeWindow;
    }

    virtual void makeActive() {
        activeWindow = id;
        if (window != NULL)
            glfwMakeContextCurrent(window);
    }
private:
    void clearReleased() {
        keyReleased.clear();
        keyPressed.clear();
        mousePressed.clear();
        mouseReleased.clear();
    }
};

class DefaultWindow : public Window {
public:
    std::unique_ptr<Mesh> exampleMesh;
    DefaultWindow(string windowName, unsigned int w, unsigned int h) : Window(windowName, w, h) {;
        exampleMesh = std::make_unique<Mesh>(
            std::vector<float> {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                0.0f,  0.5f, 0.0f
            },
            std::make_unique<ShaderProgram>(
                std::make_unique<Shader>(
                    "#version 330 core\n"
                    "layout (location = 0) in vec3 aPos;\n"
                    "void main()\n"
                    "{\n"
                    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                    "}\0",
                    GL_VERTEX_SHADER
                ),
                std::make_unique<Shader>(
                    "#version 330 core\n"
                    "out vec4 FragColor;\n"
                    "void main()\n"
                    "{\n"
                    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                    "}\0",
                    GL_FRAGMENT_SHADER
                )
            )
        );
    };
    virtual void draw() {
        float red = mouseX / Width;
        glClearColor(red, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        exampleMesh->draw();
    }

    virtual void update(GLFWwindow* window) {
        if (getKeyDown(GLFW_KEY_W)) {
            cout << "Forward!" << id << endl;
        }

        if (getKeyReleased(GLFW_KEY_R)) {
            cout << "Reload" << id << endl;
        }
        if (getKeyPressed(GLFW_KEY_ESCAPE)) {
            cout << "killing window" << id << endl;
            closeWindow();
        }

        if (getMouseReleased(GLFW_MOUSE_BUTTON_1)) {
            cout << "Click!" << endl;
        }

        if (getMouseDown(GLFW_MOUSE_BUTTON_2)) {
            cout << "Right drag!" << endl;
        }
    }

    virtual void onClose() {
        cout << "Attempting to close window " << id << endl;
    }

    virtual void onResize(GLFWwindow* window, int w, int h) {
        cout << "New size: " << Width << " x " << Height << endl;
        glViewport(0, 0, Width, Height);
    }
};
