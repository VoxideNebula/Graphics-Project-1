#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <csci441/shader.h>
#include <csci441/matrix4.h>
#include <csci441/matrix3.h>
#include <csci441/vector4.h>
#include <csci441/uniform.h>

#include "shape.h"
#include "model.h"
#include "camera.h"
#include "renderer.h"
#include "obj_loader.h"
#include "tex_loader.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;

// Custom stuff

#define FIRST_PERSON 0
#define THIRD_PERSON 1

int camera_mode = THIRD_PERSON;
bool spacebar_is_pressed = false;

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool isPressed(GLFWwindow *window, int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool isReleased(GLFWwindow *window, int key) {
    return glfwGetKey(window, key) == GLFW_RELEASE;
}

Matrix4 processModel(const Matrix4& model, GLFWwindow *window) {
    Matrix4 trans;

    const float ROT = 1;
    const float SCALE = .05;
    const float TRANS = .01;

    // ROTATE
    if (isPressed(window, GLFW_KEY_U)) { trans.rotate_x(-ROT); }
    else if (isPressed(window, GLFW_KEY_I)) { trans.rotate_x(ROT); }
    else if (isPressed(window, GLFW_KEY_O)) { trans.rotate_y(-ROT); }
    else if (isPressed(window, GLFW_KEY_P)) { trans.rotate_y(ROT); }
    else if (isPressed(window, '[')) { trans.rotate_z(-ROT); }
    else if (isPressed(window, ']')) { trans.rotate_z(ROT); }
    // SCALE
    else if (isPressed(window, '-')) { trans.scale(1-SCALE, 1-SCALE, 1-SCALE); }
    else if (isPressed(window, '=')) { trans.scale(1+SCALE, 1+SCALE, 1+SCALE); }
    // TRANSLATE
    else if (isPressed(window, GLFW_KEY_UP)) { trans.translate(TRANS, 0, 0); }
    else if (isPressed(window, GLFW_KEY_DOWN)) { trans.translate(-TRANS, 0, 0); }
    else if (isPressed(window, GLFW_KEY_LEFT)) { trans.translate(0, 0, -TRANS); }
    else if (isPressed(window, GLFW_KEY_RIGHT)) { trans.translate(0, 0, TRANS); }
    else if (isPressed(window, ',')) { trans.translate(0,0,TRANS); }
    else if (isPressed(window, '.')) { trans.translate(0,0,-TRANS); }

    return trans * model;
}

void processInput(Camera& camera, Matrix4& projection, Matrix4& model, GLFWwindow *window) {
    model = processModel(model, window);
    
    if (isPressed(window, GLFW_KEY_ESCAPE) || isPressed(window, GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(window, true);
    }

    int spacebar_status = glfwGetKey(window, GLFW_KEY_SPACE);

    if (spacebar_status == GLFW_PRESS) {
        spacebar_is_pressed = true;
    }

    if (spacebar_status == GLFW_RELEASE && spacebar_is_pressed == true) {
        spacebar_is_pressed = false;
        camera_mode = !camera_mode;

        // Entering first person
        if (camera_mode == FIRST_PERSON) {
            model.scale(0.0001, 0.0001, 0.0001);
        }

        // Entering third person
        if (camera_mode == THIRD_PERSON) { 
            model.scale(0.9999, 0.9999, 0.9999);
            projection.perspective(45, 1, .01, 10);
            camera.projection = projection;
            camera.eye = Vector4 (0, 5, 0);
            camera.origin = Vector4 (0, 0, 0);
            camera.up = Vector4 (1, 0, 0);
        }

        printf("camera_mode = %d\n", camera_mode);
    }
}

void errorCallback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}

int main(void) {
    GLFWwindow* window;

    glfwSetErrorCallback(errorCallback);

    /* Initialize the library */
    if (!glfwInit()) { return -1; }

    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "proj01", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // tell glfw what to do on resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // init glad
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
        return -1;
    }

    // create character model
    std::vector<float> characterCoords;
    loadObj(characterCoords, "../models/character.obj");
    Model characterObj(
        characterCoords,
        Shader("../vert.glsl", "../frag.glsl")
    );
    // create character texture
    glActiveTexture(GL_TEXTURE0);
    GLuint textureQuestion = loadTexture("../img/fabric.jpg");
    glBindTexture(GL_TEXTURE_2D, textureQuestion);

    // create maze model

    // setup projection
    Matrix4 projection;
    projection.perspective(45, 1, .01, 10);

    // setup camera
    Camera camera;
    camera.projection = projection;
    camera.eye = Vector4 (0, 5, 0);
    camera.origin = Vector4 (0, 0, 0);
    camera.up = Vector4 (1, 0, 0);

    // and use z-buffering
    glEnable(GL_DEPTH_TEST);

    // create a renderer
    Renderer renderer;

    Vector4 lightPos(0.0f, 1.0f, -1.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        // process input
        processInput(camera, projection, characterObj.model, window);

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render the character with texture 0
        renderer.render(camera, characterObj, lightPos, 0);

        /* Swap front and back and poll for io events */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
