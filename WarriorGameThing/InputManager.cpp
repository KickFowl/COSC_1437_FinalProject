#include "InputManager.h"
#include <cmath>

InputManager::InputManager(EventHandle& EHndl) : EHndl(EHndl) {
    hInput = GetStdHandle(STD_INPUT_HANDLE);
}

void InputManager::emitMouse(Events type, MouseEventType mouseType)
{
    currentMouse.type = mouseType;
    currentMouse.leftHeld = leftButtonDown;
    currentMouse.rightHeld = rightButtonDown;
    emit(type, currentMouse); // copied into the event
}

void InputManager::handleKey(const KEY_EVENT_RECORD& ke)
{
    if (!ke.bKeyDown) return;

    KeyEvent evt;
    evt.virtualKeyCode = ke.wVirtualKeyCode;
    evt.asciiChar = ke.uChar.AsciiChar;
    evt.keyDown = true;
    evt.ctrlPressed = (ke.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
    evt.shiftPressed = (ke.dwControlKeyState & SHIFT_PRESSED) != 0;
    evt.altPressed = (ke.dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;

    emit(Events::KEY_DOWN, evt);
    if (evt.virtualKeyCode == VK_ESCAPE) emit(Events::ESCAPE);
}

void InputManager::handleMouse(const MOUSE_EVENT_RECORD& me, bool& mouseMoved)
{
    currentMouse.x = me.dwMousePosition.X;
    currentMouse.y = me.dwMousePosition.Y;

    if (me.dwEventFlags & MOUSE_WHEELED) {
        currentMouse.wheelDelta = static_cast<short>(HIWORD(me.dwButtonState));
        emitMouse(Events::MOUSE_WHEEL, MouseEventType::WHEEL);
        currentMouse.wheelDelta = 0;
        return;
    }

    if (me.dwEventFlags & MOUSE_MOVED) {
        mouseMoved = true;
        if (leftButtonDown &&
            std::abs(currentMouse.x - clickStartX) + std::abs(currentMouse.y - clickStartY) > 1)
            isDragging = true;
    }

    // Handle button transitions per record so a fast press+release in one poll isn't lost.
    const bool newLeft = (me.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;
    const bool newRight = (me.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0;

    if (newLeft && !leftButtonDown) {
        leftButtonDown = true;
        clickStartX = currentMouse.x;
        clickStartY = currentMouse.y;
        holdFrames = 0;
        isDragging = false;
        emitMouse(Events::MOUSE_LEFT_DOWN, MouseEventType::LEFT_DOWN);
    }
    else if (!newLeft && leftButtonDown) {
        leftButtonDown = false;
        if (isDragging) emitMouse(Events::MOUSE_LEFT_UP, MouseEventType::LEFT_UP);
        else            emitMouse(Events::MOUSE_LEFT_TAP, MouseEventType::LEFT_TAP);
        isDragging = false;
        holdFrames = 0;
    }

    if (newRight && !rightButtonDown) {
        rightButtonDown = true;
        emitMouse(Events::MOUSE_RIGHT_DOWN, MouseEventType::RIGHT_DOWN);
    }
    else if (!newRight && rightButtonDown) {
        rightButtonDown = false;
        emitMouse(Events::MOUSE_RIGHT_UP, MouseEventType::RIGHT_UP);
    }
}

void InputManager::poll()
{
    bool mouseMoved = false;

    DWORD numEvents = 0;
    if (GetNumberOfConsoleInputEvents(hInput, &numEvents) && numEvents > 0) {
        std::vector<INPUT_RECORD> records(numEvents);
        DWORD numRead = 0;
        if (ReadConsoleInputA(hInput, records.data(), numEvents, &numRead)) {
            for (DWORD i = 0; i < numRead; ++i) {
                if (records[i].EventType == MOUSE_EVENT)
                    handleMouse(records[i].Event.MouseEvent, mouseMoved);
                else if (records[i].EventType == KEY_EVENT)
                    handleKey(records[i].Event.KeyEvent);
            }
        }
    }

    // Frame-based hold detection: a held button becomes a drag after ~9 polls.
    if (leftButtonDown && !isDragging && ++holdFrames >= 9) isDragging = true;

    // One move event per poll (final position only), which keeps the 8-bit ID pool from flooding.
    if (mouseMoved) emitMouse(Events::MOUSE_MOVE, MouseEventType::MOVE);
}