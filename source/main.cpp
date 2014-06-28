#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>

#include "tdogl/Program.h"
#include "tdogl/Texture.h"
#include "tdogl/Camera.h"

glm::vec2 SCREEN_SIZE(800, 600);

GLFWwindow* window = NULL;

tdogl::Texture* gTexture = NULL;
tdogl::Program* gProgram = NULL;
tdogl::Camera gCamera;
GLuint gVAO = 0;
GLuint gVBO = 0;
GLfloat gDegreesRotated = 0.0f;

static std::string ResourcePath(std::string fileName) {
    return "../../resources/" + fileName;
}

void GlfwWindowSizeCallback (GLFWwindow* window, int width, int height) {
    SCREEN_SIZE.x = width;
    SCREEN_SIZE.y = height;
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
}

static void LoadShaders() {
    std::vector<tdogl::Shader> shaders;
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("vertex-shader.vert"), GL_VERTEX_SHADER));
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("fragment-shader.frag"), GL_FRAGMENT_SHADER));
    gProgram = new tdogl::Program(shaders);
}

// loads a cube into the VAO and VBO globals: gVAO and gVBO
static void LoadCube() {
    // make and bind the VAO
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);
    
    // make and bind the VBO
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    
    // Make a cube out of triangles (two triangles per side)
    GLfloat vertexData[] = {
        //  X     Y     Z       U     V
        // bottom
        -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
         1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
         1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
         1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,

        // top
        -1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
         1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
         1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
         1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

        // front
        -1.0f,-1.0f, 1.0f,   1.0f, 0.0f,
         1.0f,-1.0f, 1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,   0.0f, 0.0f,
         1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

        // back
        -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,   0.0f, 1.0f,
         1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
         1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,   0.0f, 1.0f,
         1.0f, 1.0f,-1.0f,   1.0f, 1.0f,

        // left
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,   1.0f, 0.0f,

        // right
         1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
         1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
         1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
         1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
         1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
         1.0f, 1.0f, 1.0f,   0.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(gProgram->attrib("vert"));
    glVertexAttribPointer(gProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), NULL);
        
    // connect the uv coords to the "vertTexCoord" attribute of the vertex shader
    glEnableVertexAttribArray(gProgram->attrib("vertTexCoord"));
    glVertexAttribPointer(gProgram->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE,  5*sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

    // unbind the VAO
    glBindVertexArray(0);
}

static void LoadTexture() {
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(ResourcePath("wooden-crate.jpg"));
    bmp.flipVertically();
    gTexture = new tdogl::Texture(bmp);
}

static void Render() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gProgram->use();
    gProgram->setUniform("camera", gCamera.matrix());

    gProgram->setUniform("model", glm::rotate(glm::mat4(), gDegreesRotated, glm::vec3(0,1,0)));
        
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexture->object());
    gProgram->setUniform("tex", 0);

    glBindVertexArray(gVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6*2*3);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    gProgram->stopUsing();
        
    glfwSwapBuffers(window);
}

void Update(float secondsElapsed) {
    const GLfloat degreesPerSecond = 10.0f;
    gDegreesRotated += secondsElapsed * degreesPerSecond;
    while(gDegreesRotated > 360.0f) gDegreesRotated -= 360.0f;

    //move position of camera based on WASD keys, and XZ keys for up and down
    const float moveSpeed = 2.0; //units per second
    if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.forward());
    } else if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.forward());
    }
    if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.right());
    } else if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.right());
    }
    if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Z)){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0,1,0));
    } else if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_X)){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0,1,0));
    }

    const float rotateSpeed = 50.0;
    if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP)){
        gCamera.offsetOrientation(secondsElapsed * rotateSpeed, 0);
    } else if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN)){
        gCamera.offsetOrientation(secondsElapsed * -rotateSpeed, 0);
    }
    if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT)){
        gCamera.offsetOrientation(0, secondsElapsed * -rotateSpeed);
    } else if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT)){
        gCamera.offsetOrientation(0, secondsElapsed * rotateSpeed);
    }
}

void AppMain() {
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 4);
    
    window = glfwCreateWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, "JiNXGL", NULL, NULL);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("ERROR: could not open window with GLFW3\n");
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback (window, GlfwWindowSizeCallback);
    
    glewExperimental = GL_TRUE;
    glewInit();
    // GLEW throws some errors, so discard all the errors so far
    while(glGetError() != GL_NO_ERROR) {}

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    LoadShaders();
    LoadTexture();
    LoadCube();

    gCamera.setPosition(glm::vec3(0,0,4));
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double thisTime = glfwGetTime();
        Update(thisTime - lastTime);
        lastTime = thisTime;

        Render();

        glfwPollEvents();

        if (GLFW_PRESS == glfwGetKey (window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose (window, 1);
        }        
    }

    glfwTerminate();
}

int main(int argc, char *argv[]) {
    try {
        AppMain();
    } catch (const std::exception& e){
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

