#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <iostream>
#include <stdexcept>

#include <cmath>

#include <time.h>
#include <stdarg.h>

#include "maths_funcs.h"
#include "Program.h"
#define GL_LOG_FILE "gl.log"


// keep track of window size for things like the viewport and the mouse cursor
int g_gl_width = 640;
int g_gl_height = 480;

mat4 proj_mat;

float cam_speed = 3.0f;
float cam_yaw_speed = 30.0f;

float cam_pos[] = { 0.0f, 0.0f, 3.0f };
float cam_yaw = 0.0f;

mat4 view_mat;
mat4 model_mat;

mat4 calcProjMat() {
	float near = 0.1f;
	float far = 100.0f;
	float fov = 67.0f * ONE_DEG_IN_RAD;
	float aspect = (float)g_gl_width / (float)g_gl_height;

	float range = tan(fov * 0.5f) * near;
	float Sx = (2.0f * near) / (range * aspect + range * aspect);
	float Sy = near / range;
	float Sz = -(far + near) / (far - near);
	float Pz = -(2.0f * far * near) / (far - near);

	return mat4(
		Sx, 0.0f, 0.0f, 0.0f,
		0.0f, Sy, 0.0f, 0.0f,
		0.0f, 0.0f, Sz, -1.0f,
		0.0f, 0.0f, Pz, 0.0f
	);
}

mat4 calcViewMat() {
	mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);
	return R * T;
}

// a call-back function
void glfw_window_size_callback (GLFWwindow* window, int width, int height) {
  g_gl_width = width;
  g_gl_height = height;

  proj_mat = calcProjMat();
}

bool restart_gl_log() {
	FILE* file = fopen(GL_LOG_FILE, "w");

	if(!file) {
		fprintf(stderr, "ERROR: could not open GL_LOG_FILE log file %s for writing\n", GL_LOG_FILE);
		return false;
	}

	time_t now = time(NULL);
	char* date = ctime(&now);
	fprintf(file, "GL_LOG_FILE log. local time %s\n", date);
	fclose(file);
	return true;
}

bool gl_log (const char* message, ...) {
  va_list argptr;
  FILE* file = fopen (GL_LOG_FILE, "a");
  if (!file) {
    fprintf (
      stderr,
      "ERROR: could not open GL_LOG_FILE %s file for appending\n",
      GL_LOG_FILE
    );
    return false;
  }
  va_start (argptr, message);
  vfprintf (file, message, argptr);
  va_end (argptr);
  fclose (file);
  return true;
}

bool gl_log_err (const char* message, ...) {
  va_list argptr;
  FILE* file = fopen (GL_LOG_FILE, "a");
  if (!file) {
    fprintf (
      stderr,
      "ERROR: could not open GL_LOG_FILE %s file for appending\n",
      GL_LOG_FILE
    );
    return false;
  }
  va_start (argptr, message);
  vfprintf (file, message, argptr);
  va_end (argptr);
  va_start (argptr, message);
  vfprintf (stderr, message, argptr);
  va_end (argptr);
  fclose (file);
  return true;
}

void log_gl_params () {
  GLenum params[] = {
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE,
    GL_MAX_DRAW_BUFFERS,
    GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
    GL_MAX_TEXTURE_IMAGE_UNITS,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_VARYING_FLOATS,
    GL_MAX_VERTEX_ATTRIBS,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    GL_MAX_VERTEX_UNIFORM_COMPONENTS,
    GL_MAX_VIEWPORT_DIMS,
    GL_STEREO,
  };
  const char* names[] = {
    "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
    "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
    "GL_MAX_DRAW_BUFFERS",
    "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
    "GL_MAX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_TEXTURE_SIZE",
    "GL_MAX_VARYING_FLOATS",
    "GL_MAX_VERTEX_ATTRIBS",
    "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
    "GL_MAX_VIEWPORT_DIMS",
    "GL_STEREO",
  };
  gl_log ("GL Context Params:\n");
  char msg[256];
  // integers - only works if the order is 0-10 integer return types
  for (int i = 0; i < 10; i++) {
    int v = 0;
    glGetIntegerv (params[i], &v);
    gl_log ("%s %i\n", names[i], v);
  }
  // others
  int v[2];
  v[0] = v[1] = 0;
  glGetIntegerv (params[10], v);
  gl_log ("%s %i %i\n", names[10], v[0], v[1]);
  unsigned char s = 0;
  glGetBooleanv (params[11], &s);
  gl_log ("%s %u\n", names[11], (unsigned int)s);
  gl_log ("-----------------------------\n");
}

static std::string ResourcePath(std::string fileName) {
	return "./../../src/resources/" + fileName;
}

static std::vector<tdogl::Shader> LoadShaders() {
	std::vector<tdogl::Shader> shaders;
	shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("test_vs.vert") ,GL_VERTEX_SHADER));
	shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("test_fs.frag"), GL_FRAGMENT_SHADER));
	return shaders;
}

void glfw_error_callback (int error, const char* description) {
	gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}


int main() {
	assert(restart_gl_log());

	gl_log ("starting GLFW\n%s\n", glfwGetVersionString ());
	glfwSetErrorCallback (glfw_error_callback);

	if (!glfwInit ()) {
		fprintf (stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_SAMPLES, 4);


	// GLFWmonitor* mon = glfwGetPrimaryMonitor ();
	// const GLFWvidmode* vmode = glfwGetVideoMode (mon);
	// GLFWwindow* window = glfwCreateWindow (
	// 	vmode->width, vmode->height, "Extended GL Init", mon, NULL
	// );

    GLFWwindow* window = glfwCreateWindow(g_gl_width, g_gl_height, "JiNXGL", NULL, NULL);

    if (!window)
    {
    	fprintf (stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback (window, glfw_window_size_callback);

	glewExperimental = GL_TRUE;
	glewInit();

	const GLubyte* renderer = glGetString (GL_RENDERER);
	const GLubyte* version = glGetString (GL_VERSION);
	printf ("Renderer: %s\n", renderer);
	printf ("OpenGL version supported %s\n", version);
	log_gl_params();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable (GL_CULL_FACE); // cull face
	glCullFace (GL_BACK); // cull back face
	glFrontFace (GL_CCW); // GL_CCW for counter clock-wise

    std::vector<tdogl::Shader> shaders = LoadShaders();
	tdogl::Program* gProgram = new tdogl::Program(shaders);

	vec3 points[] = {
		// Front
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),

		vec3(0.0f, -1.0f, 0.0f),
		vec3(1.0f, -1.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),

		// Top
		vec3(0.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, -1.0f),

		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, -1.0f),
		vec3(0.0f, 0.0f, -1.0f),

		// Left
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(0.0f, -1.0f, 0.0f),

		vec3(0.0f, 0.0f, -1.0f),
		vec3(0.0f, -1.0f, -1.0f),
		vec3(0.0f, -1.0f, 0.0f),

		// Back
		vec3(1.0f, -1.0f, -1.0f),
		vec3(0.0f, -1.0f, -1.0f),
		vec3(1.0f, 0.0f, -1.0f),

		vec3(0.0f, -1.0f, -1.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(1.0f, 0.0f, -1.0f),

		// Bottom
		vec3(1.0f, -1.0f, -1.0f),
		vec3(1.0f, -1.0f, 0.0f),
		vec3(0.0f, -1.0f, -1.0f),

		vec3(1.0f, -1.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f),
		vec3(0.0f, -1.0f, -1.0f),

		// Right
		vec3(1.0f, -1.0f, -1.0f),
		vec3(1.0f, 0.0f, -1.0f),
		vec3(1.0f, -1.0f, 0.0f),

		vec3(1.0f, 0.0f, -1.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, -1.0f, 0.0f),
	};

	int numberOfPoints = (sizeof(points) / sizeof(*points));

	GLuint points_vbo = 0;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, numberOfPoints * 3 * sizeof(float), points, GL_STATIC_DRAW);

	float normals[] = {
	  0.0f, 0.0f, 1.0f,
	  0.0f, 0.0f, 1.0f,
	  0.0f, 0.0f, 1.0f,

	  0.0f, 0.0f, 1.0f,
	  0.0f, 0.0f, 1.0f,
	  0.0f, 0.0f, 1.0f,


	  0.0f, 1.0f, 0.0f,
	  0.0f, 1.0f, 0.0f,
	  0.0f, 1.0f, 0.0f,

	  0.0f, 1.0f, 0.0f,
	  0.0f, 1.0f, 0.0f,
	  0.0f, 1.0f, 0.0f,


	  -1.0f, 0.0f, 0.0f,
	  -1.0f, 0.0f, 0.0f,
	  -1.0f, 0.0f, 0.0f,

	  -1.0f, 0.0f, 0.0f,
	  -1.0f, 0.0f, 0.0f,
	  -1.0f, 0.0f, 0.0f,


	  0.0f, 0.0f, -1.0f,
	  0.0f, 0.0f, -1.0f,
	  0.0f, 0.0f, -1.0f,

	  0.0f, 0.0f, -1.0f,
	  0.0f, 0.0f, -1.0f,
	  0.0f, 0.0f, -1.0f,

	  0.0f, -1.0f, 0.0f,
	  0.0f, -1.0f, 0.0f,
	  0.0f, -1.0f, 0.0f,

	  0.0f, -1.0f, 0.0f,
	  0.0f, -1.0f, 0.0f,
	  0.0f, -1.0f, 0.0f,


	  1.0f, 0.0f, 0.0f,
	  1.0f, 0.0f, 0.0f,
	  1.0f, 0.0f, 0.0f,

	  1.0f, 0.0f, 0.0f,
	  1.0f, 0.0f, 0.0f,
	  1.0f, 0.0f, 0.0f,

	};


	GLuint normals_vbo = 0;
	glGenBuffers (1, &normals_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, normals_vbo);
	glBufferData (GL_ARRAY_BUFFER, numberOfPoints * 3 * sizeof(float), normals, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glBindBuffer (GL_ARRAY_BUFFER, normals_vbo);
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);

	proj_mat = calcProjMat();
	view_mat = calcViewMat();
	model_mat = translate(identity_mat4(), vec3(-0.5f, 0.5f, 0.5f));

	int view_mat_location = gProgram->uniform("view_mat");
	int proj_mat_location = gProgram->uniform("projection_mat");
	int model_mat_location = gProgram->uniform("model_mat");
	int current_time_location = gProgram->uniform("current_time");

    while (!glfwWindowShouldClose(window))
    {
    	//_update_fps_counter(window);
		static double previous_seconds = glfwGetTime ();
		double current_seconds = glfwGetTime ();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		model_mat = rotate_x_deg(model_mat, elapsed_seconds * 45 * sin(current_seconds));
		model_mat = rotate_y_deg(model_mat, elapsed_seconds * 120 * cos(current_seconds));
		//model_mat = rotate_z_deg(model_mat, elapsed_seconds * 180 * sin(current_seconds));

		glUseProgram(gProgram->object());
		glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
		glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat.m);
		glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat.m);
		glUniform1f(current_time_location, (float)current_seconds);

 		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport (0, 0, g_gl_width * 2, g_gl_height * 2);

 		glBindVertexArray(vao);
 		glDrawArrays(GL_TRIANGLES, 0, numberOfPoints);

    	glfwSwapBuffers(window);
        glfwPollEvents();


		// control keys
		bool cam_moved = false;
		if (glfwGetKey (window, GLFW_KEY_A)) {
		  cam_pos[0] -= cam_speed * elapsed_seconds;
		  cam_moved = true;
		}
		if (glfwGetKey (window, GLFW_KEY_D)) {
		  cam_pos[0] += cam_speed * elapsed_seconds;
		  cam_moved = true;
		}
		if (glfwGetKey (window, GLFW_KEY_UP)) {
		  cam_pos[1] += cam_speed * elapsed_seconds;
		  cam_moved = true;
		}
		if (glfwGetKey (window, GLFW_KEY_DOWN)) {
		  cam_pos[1] -= cam_speed * elapsed_seconds;
		  cam_moved = true;
		}
		if (glfwGetKey (window, GLFW_KEY_W)) {
		  cam_pos[2] -= cam_speed * elapsed_seconds;
		  cam_moved = true;
		}
		if (glfwGetKey (window, GLFW_KEY_S)) {
		  cam_pos[2] += cam_speed * elapsed_seconds;
		  cam_moved = true;
		}
		if (glfwGetKey (window, GLFW_KEY_LEFT)) {
		  cam_yaw += cam_yaw_speed * elapsed_seconds;
		  cam_moved = true;
		}
		if (glfwGetKey (window, GLFW_KEY_RIGHT)) {
		  cam_yaw -= cam_yaw_speed * elapsed_seconds;
		  cam_moved = true;
		}

		// update view matrix
		if (cam_moved) {
			view_mat = calcViewMat();
		}


		if (GLFW_PRESS == glfwGetKey (window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose (window, 1);
		}
    }

    glfwTerminate();
    return 0;
}
