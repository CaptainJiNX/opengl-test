#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <list>

#include "tdogl/Program.h"
#include "tdogl/Texture.h"
#include "tdogl/Camera.h"

/*
 Represents a textured geometry asset

 Contains everything necessary to draw arbitrary geometry with a single texture:

  - shaders
  - a texture
  - a VBO
  - a VAO
  - the parameters to glDrawArrays (drawType, drawStart, drawCount)
 */
struct ModelAsset {
    tdogl::Program* shaders;
    tdogl::Texture* texture;
    GLuint vbo;
    GLuint vao;
    GLenum drawType;
    GLint drawStart;
    GLint drawCount;

    ModelAsset() :
        shaders(NULL),
        texture(NULL),
        vbo(0),
        vao(0),
        drawType(GL_TRIANGLES),
        drawStart(0),
        drawCount(0)
    {}
};

/*
 Represents an instance of an `ModelAsset`

 Contains a pointer to the asset, and a model transformation matrix to be used when drawing.
 */
struct ModelInstance {
    ModelAsset* asset;
    glm::mat4 transform;

    ModelInstance() :
        asset(NULL),
        transform()
    {}
};

glm::vec2 SCREEN_SIZE(800, 600);

GLFWwindow* window = NULL;

tdogl::Camera gCamera;
ModelAsset gWoodenCrate;
std::list<ModelInstance> gInstances;
GLfloat gDegreesRotated = 0.0f;

static std::string ResourcePath(std::string fileName) {
    return "../../resources/" + fileName;
}

void GlfwWindowSizeCallback (GLFWwindow* window, int width, int height) {
    SCREEN_SIZE.x = width;
    SCREEN_SIZE.y = height;
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);
}

static tdogl::Program* LoadShaders(const char* vertFileName, const char* fragFileName) {
    std::vector<tdogl::Shader> shaders;
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath(vertFileName), GL_VERTEX_SHADER));
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath(fragFileName), GL_FRAGMENT_SHADER));
    return new tdogl::Program(shaders);
}

static tdogl::Texture* LoadTexture(const char* filename) {
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(ResourcePath(filename));
    bmp.flipVertically();
    return new tdogl::Texture(bmp);
}

// initialises the gWoodenCrate global
static void LoadWoodenCrateAsset() {
    // set all the elements of gWoodenCrate
    gWoodenCrate.shaders = LoadShaders("vertex-shader.vert", "fragment-shader.frag");
    gWoodenCrate.drawType = GL_TRIANGLES;
    gWoodenCrate.drawStart = 0;
    gWoodenCrate.drawCount = 6*2*3;
    gWoodenCrate.texture = LoadTexture("wooden-crate.jpg");
    glGenBuffers(1, &gWoodenCrate.vbo);
    glGenVertexArrays(1, &gWoodenCrate.vao);

    // bind the VAO
    glBindVertexArray(gWoodenCrate.vao);

    // bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.vbo);

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
    glEnableVertexAttribArray(gWoodenCrate.shaders->attrib("vert"));
    glVertexAttribPointer(gWoodenCrate.shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), NULL);

    // connect the uv coords to the "vertTexCoord" attribute of the vertex shader
    glEnableVertexAttribArray(gWoodenCrate.shaders->attrib("vertTexCoord"));
    glVertexAttribPointer(gWoodenCrate.shaders->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE,  5*sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

    // unbind the VAO
    glBindVertexArray(0);
}

glm::mat4 translate(GLfloat x, GLfloat y, GLfloat z) {
    return glm::translate(glm::mat4(), glm::vec3(x,y,z));
}

glm::mat4 scale(GLfloat x, GLfloat y, GLfloat z) {
    return glm::scale(glm::mat4(), glm::vec3(x,y,z));
}


static void CreateInstances() {
    ModelInstance dot;
    dot.asset = &gWoodenCrate;
    dot.transform = glm::mat4();
    gInstances.push_back(dot);

    ModelInstance i;
    i.asset = &gWoodenCrate;
    i.transform = translate(0,-4,0) * scale(1,2,1);
    gInstances.push_back(i);

    ModelInstance hLeft;
    hLeft.asset = &gWoodenCrate;
    hLeft.transform = translate(-8,0,0) * scale(1,6,1);
    gInstances.push_back(hLeft);

    ModelInstance hRight;
    hRight.asset = &gWoodenCrate;
    hRight.transform = translate(-4,0,0) * scale(1,6,1);
    gInstances.push_back(hRight);

    ModelInstance hMid;
    hMid.asset = &gWoodenCrate;
    hMid.transform = translate(-6,0,0) * scale(2,1,0.8);
    gInstances.push_back(hMid);
}

//renders a single `ModelInstance`
static void RenderInstance(const ModelInstance& inst) {
    ModelAsset* asset = inst.asset;
    tdogl::Program* shaders = asset->shaders;

    //bind the shaders
    shaders->use();

    //set the shader uniforms
    shaders->setUniform("camera", gCamera.matrix());
    shaders->setUniform("model", inst.transform);
    shaders->setUniform("tex", 0); //set to 0 because the texture will be bound to GL_TEXTURE0

    //bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, asset->texture->object());

    //bind VAO and draw
    glBindVertexArray(asset->vao);
    glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

    //unbind everything
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shaders->stopUsing();
}

static void Render() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // render all the instances
    std::list<ModelInstance>::const_iterator it;
    for(it = gInstances.begin(); it != gInstances.end(); ++it){
        RenderInstance(*it);
    }
        
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

    LoadWoodenCrateAsset();
    CreateInstances();

    gCamera.setPosition(glm::vec3(-4,0,17));
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

