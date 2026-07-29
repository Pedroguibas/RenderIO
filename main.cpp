#include <iostream>

#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <stdexcept>
#include <vector>
#include <optional>

#include "Vertex.h"
#include "Shader.h"
#include "Mesh.h"
using std::cout;
using std::nullopt;
using std::vector;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main() {

  if (!glfwInit()) {
    cout << "Failed to initialize GLFW\n";
    return -1;
  }

  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
    glfwCreateWindow(1280, 720, "RenderIO", nullptr, nullptr);

  if (!window) {
    cout << "Failed to create window\n";
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (!gladLoadGL(glfwGetProcAddress)) {
    cout << "Failed to load GLAD\n";
    return -1;
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_FRONT);

  glViewport(0, 0, 1280, 720);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

  Vertex v1(
    glm::vec3(0.0f, 0.5f, -0.1f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    nullopt,
    nullopt
  );
  Vertex v2(
    glm::vec3(0.5f, -0.5f, -0.1f),
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
    nullopt,
    nullopt
  );
  Vertex v3(
    glm::vec3(-0.5f, -0.5f, -0.1f),
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    nullopt,
    nullopt
  );

  vector<Vertex> vec{v1, v2, v3};
  vector<unsigned int> indices{0, 1, 2};

  vector<float> vertexes = Vertex::flatten(vec);

  Mesh triangle(vec, indices);

  Shader *shader;
  try {
    shader = Shader::create(
      "shaders/basic.vert", "shaders/basic.frag", vector<string>{"HAS_COLOR"}
    );
  } catch (const std::exception &e) {
    cout << e.what();
    return -1;
  }

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();

    triangle.draw();

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}