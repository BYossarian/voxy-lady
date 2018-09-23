
#pragma once

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Window {

public:

    Window(const std::string &name, GLint windowWidth = 800, GLint windowHeight = 600);
    ~Window();

    GLint getBufferWidth() const { return bufferWidth; }
    GLint getBufferHeight() const { return bufferHeight; }
    GLfloat getAspectRatio() const { return (GLfloat)bufferWidth/(GLfloat)bufferHeight; }

    bool shouldWindowClose() { return glfwWindowShouldClose(window); }
    const bool* getKeyStates() const { return keys; }
    GLfloat getMouseXChange();
    GLfloat getMouseYChange();

    void swapBuffers() { glfwSwapBuffers(window); }
    void setSwapInterval(int interval) { glfwSwapInterval(interval); }

private:

    GLFWwindow* window;
    GLint bufferWidth, bufferHeight;
    bool keys[1024];
    double currentMouseX, currentMouseY;
    double lastMouseX, lastMouseY;

    // prevent copy and copy-assignment
    Window& operator=(const Window &other);
    Window(const Window &other);

    void handleKeyEvent(int key, int code, int action, int mode);
    void handleMouseMoveEvent(double x, double y);
    void handleResizeEvent(int width, int height);
    void attachEventListeners();

};

Window::Window(const std::string &name, GLint windowWidth, GLint windowHeight): 
    window(nullptr), bufferWidth(0), bufferHeight(0), keys{false} {

    if (!glfwInit()) {
        std::cout << "GLFW init fail\n";
        glfwTerminate();
        throw;
    }

    // window properties:
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // create window:
    window = glfwCreateWindow(windowWidth, windowHeight, name.c_str(), nullptr, nullptr);
    if (!window) {
        std::cout << "Unable to open window\n";
        glfwTerminate();
        throw;
    }
    glfwMakeContextCurrent(window);

    // load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        throw;
    }

    // get buffer size information
    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
    // setup viewport size:
    glViewport(0, 0, bufferWidth, bufferHeight);

    attachEventListeners();

    // capture cursor in window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwGetCursorPos(window, &currentMouseX, &currentMouseY);
    lastMouseX = currentMouseX;
    lastMouseY = currentMouseY;

}

Window::~Window() {

    glfwDestroyWindow(window);
    window = nullptr;

}

// getMouseXChange and getMouseYChange will return the 
// delta/change since the last time they were called:
GLfloat Window::getMouseXChange() {

    GLfloat change = currentMouseX - lastMouseX;
    lastMouseX = currentMouseX;
    return change;

}

GLfloat Window::getMouseYChange() {

    GLfloat change = currentMouseY - lastMouseY;
    lastMouseY = currentMouseY;
    return change;

}

void Window::handleKeyEvent(int key, int code, int action, int mode) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }

}

void Window::handleMouseMoveEvent(double x, double y) {

    currentMouseX = x;
    currentMouseY = y;

}

void Window::handleResizeEvent(int width, int height) {

    bufferWidth = width;
    bufferHeight = height;
    glViewport(0, 0, bufferWidth, bufferHeight);

}

void Window::attachEventListeners() {

    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int code, int action, int mode) {
        // events include a reference to pass event down to specific Window instance:
        static_cast<Window*>(glfwGetWindowUserPointer(window))->handleKeyEvent(key, code, action, mode);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
        // events include a reference to pass event down to specific Window instance:
        static_cast<Window*>(glfwGetWindowUserPointer(window))->handleMouseMoveEvent(x, y);
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        // events include a reference to pass event down to specific Window instance:
        static_cast<Window*>(glfwGetWindowUserPointer(window))->handleResizeEvent(width, height);
    });

}
