#include <iostream>

#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <stdexcept>
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vertex.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "MeshFactory.h"
#include "Texture.h"
#include "Lights.h"
#include "Material.h"
#include "Model.h"
#include "InputHandler.h"
using std::cout;
using std::nullopt;
using std::vector;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main() {
  const int DEFAULT_INPUTSET = 0;
  const int DEFAULT_MOUSE_INPUTSET = 0;

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
  InputHandler inputs(window, DEFAULT_INPUTSET);
  MouseInputHandler mouseInputs(window, DEFAULT_MOUSE_INPUTSET);

  if (!gladLoadGL(glfwGetProcAddress)) {
    cout << "Failed to load GLAD\n";
    return -1;
  }

  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
  // glFrontFace(GL_CCW);

  glViewport(0, 0, 1280, 720);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

  Shader *shader;
  try {
    shader = Shader::create(
      "shaders/basicPbr.vert", "shaders/basicPbr.frag", {"HAS_UV", "HAS_NORMAL"}
    );
  } catch (const std::exception &e) {
    cout << e.what();
    return -1;
  }

  Camera cam({0.0f, 3.0f, 4.0f});
  shader->use();

  Texture *groundTex;
  Texture *boxDiffuse;
  Texture *boxSpecular;
  Texture *lampDiffuse;
  Texture *lampSpecular;
  Texture *lampEmission;
  try {
    groundTex = new Texture("textures/ground.jpg", 0);

    boxDiffuse = new Texture("textures/box_diffuse.png", 1);

    boxSpecular = new Texture("textures/box_specular.png", 2);

    lampDiffuse = new Texture("textures/lamp_diffuse.png", 3);

    lampSpecular = new Texture("textures/lamp_specular.png", 4);

    lampEmission = new Texture("textures/lamp_emission.png", 5);
  } catch (const std::exception &e) {
    cout << e.what();
    return -1;
  }

  DirectionLight light(vec4(1.0f), vec3(-1.0f, -1.0f, -1.0f));
  PointLight pLight(
    vec4(1.0f, 1.0f, 0.0f, 1.0f), vec3(1.0f, 0.5f, 0.0f), {.constant = 0.7f}
  );
  SpotLight sLight(
    vec4(0.2f, 0.5f, 0.8f, 1.0f),
    vec3(3.0f, 3.0f, 0.0f),
    glm::normalize(vec3(-1.0f, -1.0f, 0.0f))
  );

  Material box = Material::createPhong(
    {.albedo = {.index = 1, .mapEnabled = true},
     .specular = {.index = 2, .enabled = true, .mapEnabled = true},
     .emission = {.enabled = false}}
  );
  Material ground = Material::createPhong({.specular = {.enabled = true}});
  Material pbrGround = Material::createPBR({});
  Material lampMaterial = Material::createPhong({
    .albedo = {.index = 3, .mapEnabled = true},
    .specular = {.index = 4, .enabled = true, .mapEnabled = true},
    .emission = {.index = 5, .enabled = true, .mapEnabled = true},
  });

  MeshFactory mf;

  Mesh cube = mf.box().withUv().withNormals().build(box);
  Mesh lamp = mf.box().withUv().withNormals().build(lampMaterial);
  Mesh floor(
    {Vertex(
       vec3{-15.0f, 0.0f, 15.0f},
       nullopt,
       glm::vec2{0.0f, 0.0f},
       vec3{0.0f, 1.0f, 0.0f}
     ),
     Vertex(
       vec3{15.0f, 0.0f, 15.0f},
       nullopt,
       glm::vec2{5.0f, 0.0f},
       vec3{0.0f, 1.0f, 0.0f}
     ),
     Vertex(
       vec3{-15.0f, 0.0f, -15.0f},
       nullopt,
       glm::vec2{0.0f, 5.0f},
       vec3{0.0f, 1.0f, 0.0f}
     ),
     Vertex(
       vec3{15.0f, 0.0f, -15.0f},
       nullopt,
       glm::vec2{5.0f, 5.0f},
       vec3{0.0f, 1.0f, 0.0f}
     )},
    {0, 1, 2, 2, 1, 3},
    ground
  );

  Model miku("models/miku/source/miku.gltf");

  float deltaTime = glfwGetTime();

  inputs.listen(ButtonEventEnum::Hold, GLFW_KEY_SPACE, [&]() {
    cam.moveUp(deltaTime);
  });
  inputs.listen(ButtonEventEnum::Hold, GLFW_KEY_LEFT_SHIFT, [&]() {
    cam.moveDown(deltaTime);
  });
  inputs.listen(ButtonEventEnum::Hold, GLFW_KEY_W, [&]() {
    cam.moveForward(deltaTime);
  });
  inputs.listen(ButtonEventEnum::Hold, GLFW_KEY_A, [&]() {
    cam.moveLeft(deltaTime);
  });
  inputs.listen(ButtonEventEnum::Hold, GLFW_KEY_S, [&]() {
    cam.moveBackwards(deltaTime);
  });
  inputs.listen(ButtonEventEnum::Hold, GLFW_KEY_D, [&]() {
    cam.moveRight(deltaTime);
  });
  inputs.listen(ButtonEventEnum::Press, GLFW_KEY_ESCAPE, [&]() {
    mouseInputs.showCursor();
  });
  mouseInputs.setCursorAction([&](float x, float y) {
    static float previousX = 0.0f, previousY = 0.0f;

    cam.rotate(x - previousX, y - previousY);

    previousX = x;
    previousY = y;
  });
  mouseInputs.hideCursor();

  int w_width, w_height;
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    deltaTime = glfwGetTime() - deltaTime;

    glfwPollEvents();
    inputs.handleInputs();
    mouseInputs.handleInputs();
    glfwGetWindowSize(window, &w_width, &w_height);

    shader->use();

    glm::mat4 model(1.0f);
    mat4 view = cam.getView();
    mat4 perspective = cam.getPerspective((float)w_width / (float)w_height);

    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("perspective", perspective);
    shader->setFloat("ambientStrength", 0.25f);
    shader->setVec3("cameraPos", cam.getPosition());
    light.upload(*shader, "light");
    pLight.upload(*shader, "pLight");
    sLight.upload(*shader, "sLight");

    floor.draw(*shader);

    // model = glm::translate(glm::mat4(1.0f), {-1.0f, 0.5f, 0.0f});
    // shader->setMat4("model", model);
    // cube.draw(*shader);

    // model = glm::translate(glm::mat4(1.0f), {1.0f, 0.5f, 0.0f});
    // shader->setMat4("model", model);
    // lamp.draw(*shader);

    // shader->setMat4("model", model);
    model = glm::translate(glm::mat4(1.0f), {1.7f, 0.0f, 0.0f});
    model = glm::scale(model, glm::vec3(0.8));
    miku.draw(*shader, model);

    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}