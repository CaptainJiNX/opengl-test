#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <glm/glm.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>

#include "Program.h"

const glm::vec2 SCREEN_SIZE(800, 600);

tdogl::Program* gProgram = NULL;
GLuint gVAO = 0;
GLuint gVBO = 0;

static std::string ResourcePath(std::string fileName) {
	return "./../../src/" + fileName;
}

static void LoadShaders() {
	std::vector<tdogl::Shader> shaders;
	shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("vertex-shader.txt"),GL_VERTEX_SHADER));
	shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("fragment-shader.txt"), GL_FRAGMENT_SHADER));
	gProgram = new tdogl::Program(shaders);
}

static void LoadTriangle() {
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);

	GLfloat vertexData[] = {
		 0.0f, 0.8f, 0.0f,
		-0.8f,-0.8f, 0.0f,
		 0.8f,-0.8f, 0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(gProgram->attrib("vert"));
	glVertexAttribPointer(gProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

static void Render() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(gProgram->object());
	glBindVertexArray(gVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glUseProgram(0);
}

void AppMain() {

	GLFWwindow* window;

    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, "JiNXGL", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("could not create window");
    }

    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    // if(glewInit() != GLEW_OK)
    //     throw std::runtime_error("glewInit failed");

    glfwMakeContextCurrent(window);

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // if(!GLEW_VERSION_3_2)
    //     throw std::runtime_error("OpenGL 3.2 API is not available.");

    LoadShaders();
    LoadTriangle();

    while (!glfwWindowShouldClose(window))
    {
    	Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}


int main (int argc, char* argv[])
{
	try {
        AppMain();
    } catch (const std::exception& e){
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}