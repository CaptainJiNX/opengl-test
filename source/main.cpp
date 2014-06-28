#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>

#include "tdogl/Program.h"
#include "tdogl/Texture.h"

const glm::vec2 SCREEN_SIZE(800, 600);

GLFWwindow* window = NULL;

tdogl::Texture* gTexture = NULL;
tdogl::Program* gProgram = NULL;
GLuint gVAO = 0;
GLuint gVBO = 0;

static std::string ResourcePath(std::string fileName) {
    return "../../resources/" + fileName;
}

static void LoadShaders() {
    std::vector<tdogl::Shader> shaders;
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("vertex-shader.vert"), GL_VERTEX_SHADER));
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("fragment-shader.frag"), GL_FRAGMENT_SHADER));
    gProgram = new tdogl::Program(shaders);
}

static void LoadTriangle() {
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);

    GLfloat vertexData[] = {
        //  X     Y     Z       U     V
         0.0f, 0.8f, 0.0f,   0.5f, 1.0f,
        -0.8f,-0.8f, 0.0f,   0.0f, 0.0f,
         0.8f,-0.8f, 0.0f,   1.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(gProgram->attrib("vert"));
    glVertexAttribPointer(gProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), NULL);
        
    glEnableVertexAttribArray(gProgram->attrib("vertTexCoord"));
    glVertexAttribPointer(gProgram->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE,  5*sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);    
}

static void LoadTexture() {
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(ResourcePath("hazard.png"));
    bmp.flipVertically();
    gTexture = new tdogl::Texture(bmp);
}

static void Render() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    gProgram->use();
        
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexture->object());
    gProgram->setUniform("tex", 0);

    glBindVertexArray(gVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    gProgram->stopUsing();
        
    glfwSwapBuffers(window);
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
    
    glewExperimental = GL_TRUE;
    glewInit();

    LoadShaders();
    LoadTexture();
    LoadTriangle();

    while (!glfwWindowShouldClose(window)) {
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

