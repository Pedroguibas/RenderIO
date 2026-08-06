#include "InputHandler.h"

#include <algorithm>

ActionId InputHandler::currentActionId = 0;

bool KeyboardEvent::operator==(const KeyboardEvent &other) const {
  return this->trigger == other.trigger && this->key == other.key;
}

bool EventAction::operator==(int id) const {
  return this->id == id;
}

InputHandler::InputHandler(GLFWwindow *window, int defaultSetKey)
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

ActionId InputHandler::listen(ButtonEventEnum event, int key, Action action) {
  KeyboardEvent k_event{.trigger = event, .key = key};

  const ActionId id = currentActionId++;

  inputSets.at(activeSet)[k_event].push_back(
    EventAction{.id = id, .action = std::move(action)}
  );

  return id;
}

void InputHandler::removeListener(
  ButtonEventEnum event, int key, std::optional<int> inputSet
) {
  const int set = inputSet.has_value() ? inputSet.value() : activeSet;

  KeyboardEvent k_event{.trigger = event, .key = key};

  if (inputSets.contains(set) && inputSets[set].contains(k_event))
    inputSets.at(set).erase(k_event);
}

void InputHandler::removeAction(
  ButtonEventEnum event, int key, ActionId id, std::optional<int> inputSet
) {
  const int set = inputSet.has_value() ? inputSet.value() : activeSet;
  KeyboardEvent k_event{.trigger = event, .key = key};

  if (inputSets.contains(set) && inputSets[set].contains(k_event)) {
    const auto idx = std::find(
      inputSets[set][k_event].begin(), inputSets[set][k_event].end(), id
    );

    if (idx != inputSets[set][k_event].end())
      inputSets[set][k_event].erase(idx);
  }
}

void InputHandler::handleInputs() {
  const auto setIterator = inputSets.find(activeSet);

  if (setIterator == inputSets.end())
    return;

  const EventSet &eventSet = setIterator->second;

  for (const auto &[event, actions] : eventSet) {
    keysCurrentState[event.key] = glfwGetKey(window, event.key);
  }

  for (const auto &[event, actions] : eventSet) {
    const bool currentState = keysCurrentState[event.key];

    const auto previousIterator = keysPreviousState.find(event.key);

    const bool previousState = previousIterator != keysPreviousState.end()
                                 ? previousIterator->second
                                 : false;

    bool shouldExecute = false;

    switch (event.trigger) {
    case ButtonEventEnum::Hold:
      if (glfwGetKey(window, event.key) == GLFW_PRESS)
        shouldExecute = currentState;
      break;

    case ButtonEventEnum::Press:
      shouldExecute = currentState && !previousState;
      break;

    case ButtonEventEnum::Release:
      shouldExecute = currentState && !previousState;
      break;
    }

    if (shouldExecute) {
      for (const EventAction &eventAction : actions) {
        if (eventAction.action)
          eventAction.action();
      }
    }
  }
  for (const auto &[key, state] : keysCurrentState) {
    keysPreviousState[key] = state;
  }
}

MouseInputHandler::MouseInputHandler(GLFWwindow *window, int defaultSet)
: window(window) {
  setActiveSet(defaultSet);
  glfwSetWindowUserPointer(window, this);

  glfwSetCursorPosCallback(window, cursorCallback);
  glfwSetMouseButtonCallback(window, buttonsCallback);
  glfwSetScrollCallback(window, scrollCallback);
}

void MouseInputHandler::cursorCallback(
  GLFWwindow *window, double xPosition, double yPosition
) {
  auto *input =
    static_cast<MouseInputHandler *>(glfwGetWindowUserPointer(window));

  if (input->getMouseActions().cursor_action)
    input->getMouseActions().cursor_action(xPosition, yPosition);
}

void MouseInputHandler::processButtonEvent(
  int glButton, int glAction, int glMods, MouseButtonAction actions
) {
  ButtonEventEnum event;

  if (glAction == GLFW_PRESS)
    event = ButtonEventEnum::Press;
  else if (glAction == GLFW_RELEASE)
    event = ButtonEventEnum::Release;
  else
    return;

  const auto iterator = actions.find(event);

  if (iterator != actions.end() && iterator->second)
    iterator->second(glMods);

  previousButtonStates[glButton] = glAction == GLFW_PRESS;
}

void MouseInputHandler::buttonsCallback(
  GLFWwindow *window, int button, int action, int mods
) {
  auto *input =
    static_cast<MouseInputHandler *>(glfwGetWindowUserPointer(window));

  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    input->processButtonEvent(
      GLFW_MOUSE_BUTTON_LEFT, action, mods, input->getMouseActions().m1_action
    );
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    input->processButtonEvent(
      GLFW_MOUSE_BUTTON_RIGHT, action, mods, input->getMouseActions().m2_action
    );
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    input->processButtonEvent(
      GLFW_MOUSE_BUTTON_MIDDLE, action, mods, input->getMouseActions().m3_action
    );
  }
}
void MouseInputHandler::processHeldButton(
  int button, const MouseButtonAction &actions
) {
  if (glfwGetMouseButton(window, button) != GLFW_PRESS)
    return;

  const auto iterator = actions.find(ButtonEventEnum::Hold);

  if (iterator != actions.end() && iterator->second) {
    iterator->second(0);
  }
}

void MouseInputHandler::scrollCallback(
  GLFWwindow *window, double offsetX, double offsetY
) {
  auto *input =
    static_cast<MouseInputHandler *>(glfwGetWindowUserPointer(window));

  if (input->getMouseActions().scroll_action)
    input->getMouseActions().scroll_action(offsetX, offsetY);
}

void MouseInputHandler::showCursor() noexcept {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  inputSets[activeSet].showing = true;
}
void MouseInputHandler::hideCursor() noexcept {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  inputSets[activeSet].showing = false;
}

void MouseInputHandler::setCursorAction(CursorAction action) {
  inputSets[activeSet].cursor_action = std::move(action);
}
void MouseInputHandler::setM1Action(ButtonEventEnum event, ClickAction action) {
  inputSets[activeSet].m1_action.try_emplace(event, std::move(action));
}
void MouseInputHandler::setM2Action(ButtonEventEnum event, ClickAction action) {
  inputSets[activeSet].m2_action.try_emplace(event, std::move(action));
}
void MouseInputHandler::setM3Action(ButtonEventEnum event, ClickAction action) {
  inputSets[activeSet].m3_action.try_emplace(event, std::move(action));
}
void MouseInputHandler::setScrollAction(ScrollAction action) {
  inputSets[activeSet].scroll_action = std::move(action);
}

void MouseInputHandler::setActiveSet(int set) {
  activeSet = set;

  if (!inputSets.contains(activeSet))
    inputSets.insert({activeSet, MouseActionSettings{}});

  if (inputSets[activeSet].showing)
    showCursor();
  else
    hideCursor();
}

const MouseActionSettings &MouseInputHandler::getMouseActions() const {
  return inputSets.at(activeSet);
}

void MouseInputHandler::handleInputs() {
  processHeldButton(GLFW_MOUSE_BUTTON_LEFT, inputSets[activeSet].m1_action);
  processHeldButton(GLFW_MOUSE_BUTTON_RIGHT, inputSets[activeSet].m2_action);
  processHeldButton(GLFW_MOUSE_BUTTON_MIDDLE, inputSets[activeSet].m3_action);
}
