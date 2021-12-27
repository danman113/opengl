#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <cmath>
#include <random>
#include <unordered_set>
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
class PerspectiveCamera;

#include "soloud.h"
#include "soloud_wav.h"

#include "utils.h"
#include "renderer.h"
#include "fs.h"
#include "texture.h"
#include "mesh.h"
#include "camera.h"
#include "text.h"


#include "resourceLoader.h"

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
    double lastTime = 0;
    int nbFrames = 0;
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
            glfwSetErrorCallback([](int code, const char* msg) {
                std::cout << "GLFW error code " << code << " \"" << msg << "\"" << std::endl;
            });
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
        }
        window = glfwCreateWindow(Width, Height, Name.c_str(), NULL, NULL);
        if (window == NULL) {
            cout << "Failed to create GLFW window" << endl;
            return;
        }
        if (!initialized) {
            glfwMakeContextCurrent(window);
            lastTime = glfwGetTime();
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                cout << "Failed to initialize GLAD" << endl;
                return;
            } else {
                cout << "Initialized glad" << endl;
            }
            if (!gladLoadGL()) {
                std::cout << "Failed to initialize OpenGL context" << std::endl;
                return;
            }
        }

        initialized = true;
        windowsOpen++;
        glfwSetWindowUserPointer(window, this);
        glfwSwapInterval(0);
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
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) {
            std::cout << (1000.0 / double(nbFrames)) << " MS/Frame " << std::endl;
            nbFrames = 0;
            lastTime += 1.0;
        }
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

    std::shared_ptr<SoLoud::Soloud> soloud;
    SoLoud::Wav sample;
    std::unique_ptr<SoundLoader> sl;
    std::unique_ptr<ShaderLoader> shaderLoader;
    std::unique_ptr<TextureLoader> textureLoader;
    TexturedMesh* TextureMesh;
    glm::mat4 example = glm::mat4(1.0);
    glm::mat4 texture = glm::mat4(1.0);
    std::unique_ptr <SpriteRenderer> Render;
    std::shared_ptr <PerspectiveCamera> camera;
    std::shared_ptr<Texture> FontTexture;

    DefaultWindow(string windowName, unsigned int w, unsigned int h) : Window(windowName, w, h) {
        shaderLoader = std::make_unique<ShaderLoader>();
        textureLoader = std::make_unique<TextureLoader>();
        soloud = std::make_unique<SoLoud::Soloud>();
        soloud->init();
        camera = std::make_shared<PerspectiveCamera>();
        camera->position.x = 1;
        camera->updateView();
        sl = std::make_unique<SoundLoader>(soloud);
        sl->load({
            {"resources/sounds/pickupCoin.wav", "coin"},
            {"resources/sounds/bookFlip2.ogg", "bookflip"}
        });

        Font f { "resources/fonts/font.ttf" };
        auto size = 60;
        auto& bitmap = f.GenerateBitmap(size, "The quick brown fox");
        f.GenerateImage(size, "The quick brown fox");
        FontTexture = std::make_shared<Texture>();
        FontTexture->width = bitmap.capacity() / size;
        FontTexture->height = size;
        FontTexture->colorSpace = GL_RED;
        FontTexture->disableAliasing = true;
        FontTexture->Init(bitmap.data());

        shaderLoader->load({
            {{"resources/shaders/image.vert", "resources/shaders/image.frag"}, "image"},
            {{"resources/shaders/triangle.vert", "resources/shaders/triangle.frag"}, "triangle"},
            {{"resources/shaders/sprite.vert", "resources/shaders/sprite.frag"}, "sprite"},
            {{"resources/shaders/text.vert", "resources/shaders/text.frag"}, "text"}
        });

        textureLoader->load({
            {"resources/images/wood.jpg", "wood"},
            {"resources/images/bg_layer4.png", "bg"}
        });

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);

        auto imageShader = *(shaderLoader->get("image"));
        auto triangleShader = *(shaderLoader->get("triangle"));
        auto textShader = *(shaderLoader->get("text"));
        auto bgTexture = *(textureLoader->get("bg"));
        auto woodTexture = *(textureLoader->get("wood"));

        float zIndex = -0.000001f;
        
        TextureMesh = new TexturedMesh(
            {
                0.5f, 0.5f,  // top right
                0.5f, -0.5f,  // bottom right
                -0.5f, 0.5f,  // top left 
                // second triangle
                0.5f, -0.5f,  // bottom right
                -0.5f, -0.5f,  // bottom left
                -0.5f, 0.5f,   // top left
            },
            {
                1.0f, 1.0f, 
                1.0f, 0.0f,
                0.0f, 1.0f,
                // second triangle
                1.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 1.0f 
            },
            textShader,
            FontTexture
        );

        Render = std::make_unique<SpriteRenderer>(std::make_unique<TexturedMesh>(
            std::vector<float> {
                0.5f, 0.5f,  // top right
                0.5f, -0.5f,  // bottom right
                -0.5f, 0.5f,  // top left 
                // second triangle
                0.5f, -0.5f,  // bottom right
                -0.5f, -0.5f,  // bottom left
                -0.5f, 0.5f,   // top left
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
            imageShader,
            bgTexture
        ), camera);

        Render->add(1, bgTexture, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
        Render->add(2, woodTexture, glm::translate(glm::mat4(1.0f), glm::vec3(-1.25f, 0.0f, 0.0f)));

        std::random_device rd;
        std::mt19937 gen{ rd() };
        std::uniform_real_distribution<float> position { -100.0f, 100.0f };
        std::uniform_real_distribution<float> normal { 0.1, 1.0f };
        // std::normal_distribution normal {0.0f, 1.0f};

        for (unsigned int i = 3; i < 500; ++i) {
            float x = position(gen);
            float y = position(gen);
            float width = normal(gen);
            float height = normal(gen);
            bool texture = position(gen) > 0;
            auto transform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 1.0f));
            transform = glm::scale(transform, glm::vec3(width, height, 1.0f));
            Render->add(i, texture ? woodTexture : bgTexture, transform);
        }

        exampleMesh = std::make_unique<Shape>(
            std::vector<float> {
                0.5f,  0.5f,  // top right
                0.5f, -0.5f,  // bottom right
                -0.5f,  0.5f, // top left 
                // second triangle
                0.5f, -0.5f,  // bottom right
                -0.5f, -0.5f, // bottom left
                -0.5f,  0.5f, // top left
            },
            triangleShader
        );
    };
    virtual void draw() {
        float red = (float)mouseX / Width;
        glClearColor(0.1f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float time = glfwGetTime();

        camera->updateProjectionMatrix(*this);
        glm::vec4 mouse = glm::vec4(
            Math::map(mouseX, 0.0f, Width, -1.0f, 1.0f), Math::map(mouseY, Height, 0.0f, -1.0f, 1.0f),
            1.0f, 1.0f
        );
        example = glm::scale(glm::identity<glm::mat4>(), glm::vec3(std::sin(time), std::sin(time), 5.0));
        
        exampleMesh->shader->use()->setUniform4f("color", red, 0.3, 0.4, 0.5)->setUniformMat4("model", example);
        camera->applyToShader(*(exampleMesh->shader));
        //exampleMesh->draw();
        TextureMesh->shader->use()
            ->setUniform4f("color", red, 0.3, 0.4, 1.0)
            ->setUniformMat4("model", glm::scale(glm::identity<glm::mat4>(), glm::vec3(5.0f, 1.0f, 5.0)));
        camera->applyToShader(*(TextureMesh->shader));
        auto out = (glm::inverse(camera->projection * camera->view) * mouse);
        auto& firstT = Render->entities[0].transform;
        auto& secondT = Render->entities[1].transform;
        firstT = glm::translate(glm::identity<glm::mat4>(), glm::vec3(out.x, out.y, 1.0f));
        firstT = glm::rotate(firstT, std::sin(time) * (3.415927f), glm::vec3(0.0f, 0.0f, 1.0f));
        secondT = glm::translate(glm::identity<glm::mat4>(), glm::vec3(-1.5f, 0.0f, 1.0f));
        secondT = glm::scale(secondT, glm::vec3(std::sin(time), std::sin(time), 1.0f));
        //glDisable(GL_DEPTH_TEST);
        //glEnable(GL_DEPTH_TEST);
        Render->Render();
        TextureMesh->draw();
    }

    virtual void update(GLFWwindow* window) {
        if (getKeyReleased(GLFW_KEY_R)) {
            cout << "Reload" << id << endl;
        }
        if (getKeyPressed(GLFW_KEY_ESCAPE)) {
            cout << "killing window" << id << endl;
            closeWindow();
        }

        float speed = 0.01;

        if (getKeyDown(GLFW_KEY_A)) {
            camera->position.x += speed;
        }

        if (getKeyDown(GLFW_KEY_D)) {
            camera->position.x -= speed;
        }

        if (getKeyDown(GLFW_KEY_W)) {
            camera->position.y += speed;
        }

        if (getKeyDown(GLFW_KEY_S)) {
            camera->position.y -= speed;
        }

        camera->updateView();

        if (getMouseReleased(GLFW_MOUSE_BUTTON_1)) {
            cout << "Click!" << endl;
            if (auto handle = sl->get("coin")) {
                auto raw = (*handle).get();
                soloud->play(*raw);
            }
        }

        Render->Render();

        if (getMouseReleased(GLFW_MOUSE_BUTTON_2)) {
            cout << "Clock!" << endl;
            if (auto handle = sl->get("bookflip")) {
                auto raw = (*handle).get();
                soloud->play(*raw);
            }
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
