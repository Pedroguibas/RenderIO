#include <iostream>

#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <stdexcept>
#include <vector>
#include <optional>

#include "Vertex.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "MeshFactory.h"
#include "Texture.h"
#include "Lights.h"
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

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glViewport(0, 0, 1280, 720);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

  MeshFactory mf;

  Mesh cube = mf.box().withUv().withNormals().build();
  Mesh floor(
    {Vertex(
       vec3{-15.0f, -0.5f, 15.0f},
       nullopt,
       glm::vec2{0.0f, 0.0f},
       vec3{0.0f, 1.0f, 0.0f}
     ),
     Vertex(
       vec3{15.0f, -0.5f, 15.0f},
       nullopt,
       glm::vec2{5.0f, 0.0f},
       vec3{0.0f, 1.0f, 0.0f}
     ),
     Vertex(
       vec3{-15.0f, -0.5f, -15.0f},
       nullopt,
       glm::vec2{0.0f, 5.0f},
       vec3{0.0f, 1.0f, 0.0f}
     ),
     Vertex(
       vec3{15.0f, -0.5f, -15.0f},
       nullopt,
       glm::vec2{5.0f, 5.0f},
       vec3{0.0f, 1.0f, 0.0f}
     )},
    {0, 1, 2, 2, 1, 3}
  );

  Shader *shader;
  try {
    shader = Shader::create(
      "shaders/basic.vert", "shaders/basic.frag", {"HAS_UV", "HAS_NORMAL"}
    );
  } catch (const std::exception &e) {
    cout << e.what();
    return -1;
  }

  Camera cam({1.5f, 2.0f, 1.5f});
  shader->use();
  shader->setInt("diffuse", 0);

  Texture *tex;
  Texture *groundTex;
  try {
    tex = new Texture("textures/cat.jpg");
    tex->bind(0);
    tex->setDefaultParams();

    groundTex = new Texture("textures/ground.jpg");
    groundTex->bind(0);
    groundTex->setDefaultParams();
  } catch (const std::exception &e) {
    cout << e.what();
    return -1;
  }

  DirectionLight light(vec4(1.0f), vec3(-1.0f, -1.0f, -1.0f));
  PointLight pLight(vec4(1.0f, 0.0f, 1.0f, 1.0f), vec3(0.0f, 2.0f, 0.0f));

  int w_width, w_height;
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float time = glfwGetTime();

    cam.setPosition(vec3(3 * std::cos(time), 3, 3 * std::sin(time)));
    cam.lookAt(vec3(0.0f));
    glfwGetWindowSize(window, &w_width, &w_height);

    shader->use();

    mat4 model(1.0f);
    mat4 view = cam.getView();
    mat4 perspective = cam.getPerspective((float)w_width / (float)w_height);

    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("perspective", perspective);
    shader->setFloat("ambientStrength", 0.25f);
    shader->setVec3("cameraPos", cam.getPosition());
    shader->setFloat("shininess", 128.0f);
    light.upload(*shader, "light");
    pLight.upload(*shader, "pLight");

    groundTex->bind(0);
    floor.draw();

    tex->bind(0);
    cube.draw();

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}