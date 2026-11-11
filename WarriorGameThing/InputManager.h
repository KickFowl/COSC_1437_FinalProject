#pragma once
#include <windows.h>
#include <vector>

#include "EventHandle.h"

enum class MouseEventType {
    NONE, MOVE, WHEEL,
    LEFT_DOWN, LEFT_UP, LEFT_TAP,
    RIGHT_DOWN, RIGHT_UP
};

// Positions are raw console cells. Any viewport offset is the consumer's job.
struct MouseEvent {
    MouseEventType type = MouseEventType::NONE;
    int x = 0;
    int y = 0;
    int wheelDelta = 0;      // >0 up, <0 down (only set for WHEEL)
    bool leftHeld = false;
    bool rightHeld = false;
};

struct KeyEvent {
    WORD virtualKeyCode = 0;
    char asciiChar = '\0';
    bool keyDown = false;
    bool ctrlPressed = false;
    bool shiftPressed = false;
    bool altPressed = false;
};

class InputManager {
private:
    HANDLE hInput;
    int clickStartX = 0;
    int clickStartY = 0;
    int holdFrames = 0;
    bool leftButtonDown = false;
    bool rightButtonDown = false;
    bool isDragging = false;

    MouseEvent currentMouse;
    EventHandle& EHndl;

    // Input events live for one frame: lifeSpan 0 means the next ageEvents() kills them.
    template <typename T>
    void emit(Events type, const T& data)
    {
        eventPtr e = EHndl.addEvent(data);
        e->type = type;
        e->target = ENGINE;
        e->lifeSpan = 0;
    }

    void emit(Events type)
    {
        eventPtr e = EHndl.addEvent();
        e->type = type;
        e->target = ENGINE;
        e->lifeSpan = 0;
    }

    void emitMouse(Events type, MouseEventType mouseType);
    void handleKey(const KEY_EVENT_RECORD& ke);
    void handleMouse(const MOUSE_EVENT_RECORD& me, bool& mouseMoved);

public:
    explicit InputManager(EventHandle& EHndl);

    // Drains the console input buffer and emits events. Call once per frame.
    void poll();
};