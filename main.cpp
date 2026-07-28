#include <iostream>

#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <stdexcept>
#include <vector>
#include <optional>

#include "Vertex.h"
#include "Shader.h"
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

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

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
    glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
    nullopt,
    nullopt
  );

  vector<Vertex> vec{v1, v2, v3};

  vector<float> vertexes = Vertex::flatten(vec);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(
    1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float))
  );
  glEnableVertexAttribArray(1);

  glBufferData(
    GL_ARRAY_BUFFER,
    vertexes.size() * sizeof(float),
    vertexes.data(),
    GL_STATIC_DRAW
  );

  Shader *shader;
  try {
    shader = new Shader("shaders/basic.vert", "shaders/basic.frag");
  } catch (const std::exception &e) {
    cout << e.what();
    return -1;
  }

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();

    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  delete shader;
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}