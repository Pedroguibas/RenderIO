#include <iostream>

#include <glad/gl.h>
#include <glfw/glfw3.h>
#include "Vertex.h"
using std::cout;
using std::nullopt;

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
    glfwCreateWindow(1280, 702, "RenderIO", nullptr, nullptr);

  if (!window) {
    cout << "Failed to create window\n";
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGL(glfwGetProcAddress)) {
    cout << "Failed to load GLAD\n";
    return -1;
  }

  glViewport(0, 0, 1280, 720);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  Vertex v1(
    glm::vec3(0.0f, 0.5f, 0.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    nullopt,
    nullopt
  );
  Vertex v2(
    glm::vec3(0.5f, -0.5f, 0.0f),
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
    nullopt,
    nullopt
  );
  Vertex v3(
    glm::vec3(-0.5f, -0.5f, 0.0f),
    glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
    nullopt,
    nullopt
  );

  vector<Vertex> vec;
  vec.push_back(v1);
  vec.push_back(v2);
  vec.push_back(v3);

  glBufferData(GL_ARRAY_BUFFER, vec.size(), vec.data(), GL_STATIC_DRAW);

  vector<float> vertexes = vertexes = Vertex::flatten(vec);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}