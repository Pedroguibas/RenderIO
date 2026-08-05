#pragma once

#include <unordered_map>
#include <GLFW/glfw3.h>
#include <functional>

using KeySet = std::unordered_map<int, std::function<void()>>;
using EventSet = std::unordered_map<int, KeySet>;
using InputSet = std::unordered_map<int, EventSet>;

class InputHandler {
  private:
    InputSet inputSets;
    int activeSet;
    GLFWwindow *window;

  public:
    InputHandler(int defaultSetKey, GLFWwindow *window);

    void createInputSet(int key);
    void deleteInputSet(int key);

    void setActiveSet(int key);

    void listen(int event, int key, std::function<void()> func);

    void handleInputs();
};