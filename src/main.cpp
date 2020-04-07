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
float pitch = 0.0;
float yaw = 0.0;

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool isPressed(GLFWwindow *window, int key)
{
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool isReleased(GLFWwindow *window, int key)
{
    return glfwGetKey(window, key) == GLFW_RELEASE;
}

float radians(float degrees)
{
    return degrees * M_PI / 180;
}

Matrix4 processModel(const Matrix4 &model, Camera &camera, GLFWwindow *window)
{
    Matrix4 trans;
    Matrix4 pitch_and_yaw;

    const float ROT = 1;
    const float SCALE = .05;
    const float TRANS = .08;

    // ROTATE
    if (isPressed(window, GLFW_KEY_U))
    {
        trans.rotate_x(-ROT);
    }
    else if (isPressed(window, GLFW_KEY_I))
    {
        trans.rotate_x(ROT);
    }
    else if (isPressed(window, GLFW_KEY_O))
    {
        trans.rotate_y(-ROT);
    }
    else if (isPressed(window, GLFW_KEY_P))
    {
        trans.rotate_y(ROT);
    }
    else if (isPressed(window, '['))
    {
        trans.rotate_z(-ROT);
    }
    else if (isPressed(window, ']'))
    {
        trans.rotate_z(ROT);
    }
    // SCALE
    else if (isPressed(window, '-'))
    {
        trans.scale(1 - SCALE, 1 - SCALE, 1 - SCALE);
    }
    else if (isPressed(window, '='))
    {
        trans.scale(1 + SCALE, 1 + SCALE, 1 + SCALE);
    }

    // TRANSLATE
    else if (isPressed(window, GLFW_KEY_UP))
    {
        if (camera_mode == THIRD_PERSON)
            trans.translate(TRANS, 0, 0);
        else
            trans.translate(TRANS, 0, 0);
    }
    else if (isPressed(window, GLFW_KEY_DOWN))
    {
        if (camera_mode == THIRD_PERSON)
            trans.translate(-TRANS, 0, 0);
        else
            trans.translate(-TRANS, 0, 0);
    }
    else if (isPressed(window, GLFW_KEY_LEFT))
    {
        if (camera_mode == THIRD_PERSON)
            trans.translate(0, 0, -TRANS);
        else
            trans.translate(0, 0, -TRANS);
    }
    else if (isPressed(window, GLFW_KEY_RIGHT))
    {
        if (camera_mode == THIRD_PERSON)
            trans.translate(0, 0, TRANS);
        else
            trans.translate(0, 0, TRANS);
    }
    else if (isPressed(window, ','))
    {
        trans.translate(0, 0, TRANS);
    }
    else if (isPressed(window, '.'))
    {
        trans.translate(0, 0, -TRANS);
    }

    // Pitch and yaw
    else if (isPressed(window, GLFW_KEY_W) && camera_mode == FIRST_PERSON)
    {
        pitch += ROT;
        if(pitch > 87.0f)
        pitch = 87.0f;
        if(pitch < -87.0f)
        pitch = -87.0f;
    }
    else if (isPressed(window, GLFW_KEY_A) && camera_mode == FIRST_PERSON)
    {
        yaw -= ROT;
    }
    else if (isPressed(window, GLFW_KEY_S) && camera_mode == FIRST_PERSON)
    {
        pitch -= ROT;
        if(pitch > 87.0f){
            pitch = 87;
        }
        if(pitch < -87.0f){
            pitch = -87;
        }
    }
    else if (isPressed(window, GLFW_KEY_D) && camera_mode == FIRST_PERSON)
    {
        yaw += ROT;
    }

    if (camera_mode == FIRST_PERSON){
        float x = cos(radians(yaw)) * cos(radians(pitch));
        float y = sin(radians(pitch));
        float z = sin(radians(yaw)) * cos(radians(pitch));
        Vector4 direction(x,y,z);
        camera.front = direction;
    }


    return trans * model;
}

void processInput(Camera &camera, Matrix4 &projection, Matrix4 &model, GLFWwindow *window)
{

    model = processModel(model, camera, window);

    if (isPressed(window, GLFW_KEY_ESCAPE) || isPressed(window, GLFW_KEY_Q))
    {
        glfwSetWindowShouldClose(window, true);
    }

    int spacebar_status = glfwGetKey(window, GLFW_KEY_SPACE);

    if (spacebar_status == GLFW_PRESS)
    {
        spacebar_is_pressed = true;
    }

    if (camera_mode == FIRST_PERSON)
    {
        // model.scale(0.0001, 0.0001, 0.0001);
        projection.perspective(45, 1, .01, 10);
        camera.projection = projection;
        camera.eye = Vector4(model.values[12] + .05, 1, model.values[14]);
        camera.origin = Vector4(model.values[12], model.values[1] + 1, model.values[14]);
        camera.up = Vector4(0, 1, 0);
    }

    if (spacebar_status == GLFW_RELEASE && spacebar_is_pressed == true)
    {
        spacebar_is_pressed = false;
        camera_mode = !camera_mode;

        // Entering first person
        if (camera_mode == FIRST_PERSON)
        {
            model.scale(0.0001, 0.0001, 0.0001);
            projection.perspective(45, 1, .01, 10);
            camera.projection = projection;
            camera.eye = Vector4(model.values[12] + 1, 1, model.values[14]);
            camera.origin = Vector4(0, 1, -1);
            camera.up = Vector4(0, 1, 0);
            camera.front = Vector4(0,0,0);
        }

        // Entering third person
        if (camera_mode == THIRD_PERSON)
        {
            model.scale(0.9999, 0.9999, 0.9999);
            projection.ortho(-12, 12, -12, 12, 1, 11);
            camera.projection = projection;
            camera.eye = Vector4(0, 5, 0);
            camera.origin = Vector4(0, 0, 0);
            camera.up = Vector4(1, 0, 0);
            camera.front = Vector4(0, -5, 0);
        }
    }
}

void errorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}

int main(void)
{
    GLFWwindow *window;

    glfwSetErrorCallback(errorCallback);

    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "proj01", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // tell glfw what to do on resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // init glad
    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
        return -1;
    }

    // create character model
    std::vector<float> characterCoords;
    loadObj(characterCoords, "../models/character.obj");
    Model characterObj(
        characterCoords,
        Shader("../vert.glsl", "../frag.glsl"));
    // create character texture
    glActiveTexture(GL_TEXTURE0);
    GLuint textureFabric = loadTexture("../img/fabric.jpg");
    glBindTexture(GL_TEXTURE_2D, textureFabric);

    // create maze model
    std::vector<float> mazeCoords;
    loadObj(mazeCoords, "../models/maze.obj");
    Model mazeObj(
        mazeCoords,
        Shader("../vert.glsl", "../frag.glsl"));
    // create the texture for the maze
    glActiveTexture(GL_TEXTURE1);
    GLuint textureBrick = loadTexture("../img/brick.jpg");
    glBindTexture(GL_TEXTURE_2D, textureBrick);

    // setup projection
    Matrix4 projection;
    //projection.perspective(45, 1, .01, 10);
    projection.ortho(-12, 12, -12, 12, 1, 11);

    // setup camera
    Camera camera;
    camera.projection = projection;
    camera.eye = Vector4(0, 5, 0);
    camera.origin = Vector4(0, 0, 0);
    camera.up = Vector4(1, 0, 0);
    camera.front = Vector4(0, -5, 0);

    // and use z-buffering
    glEnable(GL_DEPTH_TEST);

    // create a renderer
    Renderer renderer;

    Vector4 lightPos(5.0f, 5.0f, -8.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // process input
        processInput(camera, projection, characterObj.model, window);

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render the character with texture 0
        renderer.render(camera, characterObj, lightPos, 0);

        // render the maze
        renderer.render(camera, mazeObj, lightPos, 1);

        /* Swap front and back and poll for io events */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
