#include "InputHandler.h"

InputHandler::InputHandler(int defaultSetKey, GLFWwindow *window)
: window(window) {
  setActiveSet(defaultSetKey);
}

void InputHandler::createInputSet(int key) {
  inputSets.try_emplace(key);
}
void InputHandler::deleteInputSet(int key) {
  inputSets.erase(key);
}

void InputHandler::setActiveSet(int key) {
  if (!inputSets.contains(key))
    createInputSet(key);

  activeSet = key;
}

void InputHandler::listen(int event, int key, std::function<void()> func) {
  if (!inputSets.at(activeSet).contains(event))
    inputSets.at(activeSet).try_emplace(event);

  inputSets.at(activeSet).at(event).insert_or_assign(key, func);
}

void InputHandler::handleInputs() {
  for (const auto &eventSet : inputSets.at(activeSet)) {
    for (const auto &keySet : eventSet.second) {
      if (glfwGetKey(window, keySet.first) == eventSet.first) {
        keySet.second();
      }
    }
  }
}