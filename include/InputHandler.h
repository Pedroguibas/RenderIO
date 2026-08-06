#pragma once

#include <unordered_map>
#include <GLFW/glfw3.h>
#include <functional>
#include <vector>
#include <optional>

using Action = std::function<void()>;
using ActionId = uint64_t;

enum class ButtonEventEnum { Hold, Press, Release };

struct EventAction {
    ActionId id;
    Action action;

    bool operator==(int id) const;
};

struct KeyboardEvent {
    ButtonEventEnum trigger;
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

    ActionId listen(ButtonEventEnum event, int key, Action action);
    void removeListener(
      ButtonEventEnum event, int key, std::optional<int> inputSet = std::nullopt
    );
    void removeAction(
      ButtonEventEnum event,
      int key,
      ActionId id,
      std::optional<int> inputSet = std::nullopt
    );

    void handleInputs();
};

using CursorAction = std::function<void(double x, double y)>;
using ScrollAction = std::function<void(double offset)>;

using MouseButtonAction = std::unordered_map<ButtonEventEnum, Action>;

struct MouseActionSettings {
    CursorAction cursor_action;
    MouseButtonAction m1_action;
    MouseButtonAction m2_action;
    MouseButtonAction m3_action;
    ScrollAction scroll_action;
    bool showing = true;
};

class MouseInputHandler {
  private:
    std::unordered_map<int, MouseActionSettings> inputSets;
    std::unordered_map<int, bool> previousButtonStates;
    GLFWwindow *window;
    int activeSet;

    static void
    cursorCallback(GLFWwindow *window, double xPosition, double yPosition);

    static void
    buttonsCallback(GLFWwindow *window, int button, int action, int mods);

    static void
    scrollCallback(GLFWwindow *window, double offsetX, double offsetY);

  public:
    MouseInputHandler(GLFWwindow *window, int defaultSet);

    void setActiveSet(int set);
    const MouseActionSettings &getMouseActions() const;
    void
    processMouseButtons(int glButton, int glAction, MouseButtonAction action);

    void showCursor() noexcept;
    void hideCursor() noexcept;

    void setCursorAction(CursorAction action);
    void setM1Action(ButtonEventEnum event, Action action);
    void setM2Action(ButtonEventEnum event, Action action);
    void setM3Action(ButtonEventEnum event, Action action);
    void setScrollAction(ScrollAction action);
};