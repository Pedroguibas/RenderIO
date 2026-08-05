#pragma once

#include <unordered_map>
#include <GLFW/glfw3.h>
#include <functional>
#include <vector>

using Action = std::function<void()>;
using ActionId = uint64_t;

enum class KeyboardEventEnum { Hold, Press, Release };

struct EventAction {
    ActionId id;
    Action action;

    bool operator==(int id) const;
};

struct KeyboardEvent {
    KeyboardEventEnum trigger;
    int key;
    std::vector<EventAction> actions;

    bool operator==(const KeyboardEvent &other) const;
};

struct KeyboardEventHash {
    std::size_t operator()(const KeyboardEvent &event) const {
      const auto keyHash = std::hash<int>{}(event.key);
      const auto triggerHash =
        std::hash<int>{}(static_cast<int>(event.trigger));

      return keyHash ^ (triggerHash << 1);
    }
};

using ActionList = std::vector<EventAction>;

using EventSet =
  std::unordered_map<KeyboardEvent, ActionList, KeyboardEventHash>;

using InputSet = std::unordered_map<int, EventSet>;

using KeyStates = std::unordered_map<int, bool>;

class InputHandler {
  private:
    static ActionId currentActionId;
    InputSet inputSets;
    KeyStates keysCurrentState;
    KeyStates keysPreviousState;
    int activeSet;
    GLFWwindow *window;

  public:
    InputHandler(int defaultSetKey, GLFWwindow *window);

    void createInputSet(int key);
    void deleteInputSet(int key);

    void setActiveSet(int key);

    ActionId listen(KeyboardEventEnum event, int key, Action action);
    void removeListener(int inputSet, KeyboardEventEnum event, int key);
    void
    removeAction(int inputSet, KeyboardEventEnum event, int key, ActionId id);

    void handleInputs();
};