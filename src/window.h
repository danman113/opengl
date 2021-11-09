#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_set>
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

    Window(string windowName, unsigned int w, unsigned int h): Name(windowName), Width(w), Height(h) {
        id = windowsCreated++;
        windowsOpen++;
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
            } else {
                cout << "Initialized glad" << endl;
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
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Window* self = (Window*)glfwGetWindowUserPointer(window);
            if (action == GLFW_RELEASE) {
                self->keyReleased.insert(key);
                self->keyPressed.erase(key);
            } else if (action == GLFW_PRESS) {
                self->keyPressed.insert(key);
            }
            self->onKeyPressed(key, scancode, action, mods);
        });
    }
    ~Window() {
        glfwDestroyWindow(window);
    }

    static void runWindows(std::vector<shared_ptr<Window>> windows) {
        while(windowsOpen > 0) {
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

    virtual void onKeyPressed(int key, int scancode, int action, int mods) { };

    virtual void draw() = 0;

    virtual void update(GLFWwindow *window) = 0;

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
        glfwMakeContextCurrent(window);
    }
private:
    void clearReleased() {
        keyReleased.clear();
        keyPressed.clear();
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
        if(getKeyPressed(GLFW_KEY_W)) {
            cout << "Forward!" << id << endl;
        }

        if(getKeyReleased(GLFW_KEY_R)) {
            cout << "Reload" << id << endl;
        }
        if(getKeyPressed(GLFW_KEY_ESCAPE)) {
            cout << "killing window" << id << endl;
            closeWindow();
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
