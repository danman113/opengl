#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <cmath>
#include <unordered_set>
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include "utils.h"
#include "renderer.h"
#include "fs.h"
#include "texture.h"

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
    unsigned int BufferWidth;
    unsigned int BufferHeight;
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
        glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
            Window* self = (Window*)glfwGetWindowUserPointer(window);
            self->Width = width;
            self->Height = height;
            // Do we need a resize handler? Will this ever *not* be the same as framebufferSizeCallback?
        });

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
            Window* self = (Window*)glfwGetWindowUserPointer(window);
            self->BufferWidth = width;
            self->BufferHeight = height;
            self->onResize(window, width, height);
        });

        // Do we need this?
        // glfwSetWindowContentScaleCallback(window, [](GLFWwindow* window, float width, float height) {
        //     Window* self = (Window*)glfwGetWindowUserPointer(window);
        // });
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

    void run() {
        update(window);
        glfwGetCursorPos(window, &mouseX, &mouseY);
        draw();
        clearReleased();
        glfwSwapBuffers(window);
    }

    bool getKeyPressed(int key) {
        return keyPressed.find(key) != keyPressed.end();
    }

    bool getKeyDown(int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    bool getKeyReleased(int key) {
        return keyReleased.find(key) != keyReleased.end();
    }

    bool getMousePressed(int button) {
        return mousePressed.find(button) != mousePressed.end();
    }

    bool getMouseDown(int button) {
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    bool getMouseReleased(int button) {
        return mouseReleased.find(button) != mouseReleased.end();
    }

    virtual void onKeyPressed(int key, int scancode, int action, int mods) { };

    virtual void draw() = 0;

    virtual void update(GLFWwindow* window) = 0;

    virtual void onClose() = 0;

    virtual void onResize(GLFWwindow* window, int bufferWidth, int bufferHeight) = 0;

    void closeWindow() {
        glfwSetWindowShouldClose(window, true);
        windowsOpen--;
    }

    bool getActive() {
        return id == activeWindow;
    }

    void makeActive() {
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
    TexturedMesh* TextureMesh;
    glm::mat4 example = glm::mat4(1.0);
    glm::mat4 texture = glm::mat4(1.0);
    DefaultWindow(string windowName, unsigned int w, unsigned int h) : Window(windowName, w, h) {
        TextureMesh = new TexturedMesh(
            std::vector<float> {
                0.5f,  0.5f, 0.0f,  // top right
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f,  0.5f, 0.0f,  // top left 
                // second triangle
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  // bottom left
                -0.5f,  0.5f, 0.0f   // top left
            },
            std::vector<float> {
                1.0f, 1.0f, 
                1.0f, 0.0f,
                0.0f, 1.0f,
                // second triangle
                1.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 1.0f 
            },
            std::make_unique<ShaderProgram>(
                std::make_unique<Shader>(
                    FS::readFile("resources/shaders/image.vert"),
                    GL_VERTEX_SHADER
                ),
                std::make_unique<Shader>(
                    FS::readFile("resources/shaders/image.frag"),
                    GL_FRAGMENT_SHADER
                )
            ),
            new Texture("resources/images/wood.jpg")
        );

        exampleMesh = std::make_unique<Mesh>(
            std::vector<float> {
                0.5f,  0.5f, 0.0f,  // top right
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f,  0.5f, 0.0f,  // top left 
                // second triangle
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  // bottom left
                -0.5f,  0.5f, 0.0f   // top left
            },
            std::make_unique<ShaderProgram>(
                std::make_unique<Shader>(
                    FS::readFile("resources/shaders/triangle.vert"),
                    GL_VERTEX_SHADER
                ),
                std::make_unique<Shader>(
                    FS::readFile("resources/shaders/triangle.frag"),
                    GL_FRAGMENT_SHADER
                )
            )
        );
    };
    virtual void draw() {
        float red = mouseX / Width;
        glClearColor(0.1f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        float time = glfwGetTime();
        example = glm::scale(glm::identity<glm::mat4>(), glm::vec3 (std::sin(time), std::sin(time), 0.0));
        exampleMesh->shader->use()->setUniform4f("color", red, 0.3, 0.4, 0.5)->setUniformMat4("transform", example);
        exampleMesh->draw();
        texture = glm::translate(glm::identity<glm::mat4>(), glm::vec3(Math::map(mouseX, 0.0, Width, -1.0, 1.0), Math::map(mouseY, Height, 0.0, -1.0, 1.0), 0.0));
        texture = glm::rotate(texture, std::sin(time) * (3.1415927f), glm::vec3(0.0, 0.0, 1.0));
        TextureMesh->shader->use()->setUniformMat4("transform", texture);
        TextureMesh->draw();
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
        glViewport(0, 0, w, h);
    }
};
