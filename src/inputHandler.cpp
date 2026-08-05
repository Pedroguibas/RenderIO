#include "InputHandler.h"

#include <algorithm>

ActionId InputHandler::currentActionId = 0;

bool KeyboardEvent::operator==(const KeyboardEvent &other) const {
  return this->trigger == other.trigger && this->key == other.key;
}

bool EventAction::operator==(int id) const {
  return this->id == id;
}

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

ActionId InputHandler::listen(KeyboardEventEnum event, int key, Action action) {
  KeyboardEvent k_event{.trigger = event, .key = key};

  const ActionId id = currentActionId++;

  inputSets.at(activeSet)[k_event].push_back(
    EventAction{.id = id, .action = std::move(action)}
  );

  return id;
}

void InputHandler::removeListener(
  int inputSet, KeyboardEventEnum event, int key
) {
  KeyboardEvent k_event{.trigger = event, .key = key};

  if (inputSets.contains(inputSet) && inputSets[inputSet].contains(k_event))
    inputSets.at(inputSet).erase(k_event);
}

void InputHandler::removeAction(
  int inputSet, KeyboardEventEnum event, int key, ActionId id
) {
  KeyboardEvent k_event{.trigger = event, .key = key};

  if (inputSets.contains(inputSet) && inputSets[inputSet].contains(k_event)) {
    const auto idx = std::find(
      inputSets[inputSet][k_event].begin(),
      inputSets[inputSet][k_event].end(),
      id
    );

    if (idx != inputSets[inputSet][k_event].end())
      inputSets[inputSet][k_event].erase(idx);
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
    case KeyboardEventEnum::Hold:
      if (glfwGetKey(window, event.key) == GLFW_PRESS)
        shouldExecute = currentState;
      break;

    case KeyboardEventEnum::Press:
      shouldExecute = currentState && !previousState;
      break;

    case KeyboardEventEnum::Release:
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