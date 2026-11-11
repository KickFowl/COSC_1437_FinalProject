#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <cmath>
#include "InputManager.h"

enum class Anchor {
    TOP_LEFT, TOP_CENTER, TOP_RIGHT,
    MID_LEFT, CENTER, MID_RIGHT,
    BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT
};

enum class TextAlign {
    LEFT, CENTER, RIGHT
};

enum class DimensionType {
    LITERAL, PERCENT, NORMAL
};

namespace Color {
    enum Code : WORD {
        BLACK = 0, DARK_BLUE = 1, DARK_GREEN = 2, DARK_CYAN = 3,
        DARK_RED = 4, DARK_MAGENTA = 5, DARK_YELLOW = 6, GRAY = 7,
        DARK_GRAY = 8, BLUE = 9, GREEN = 10, CYAN = 11,
        RED = 12, MAGENTA = 13, YELLOW = 14, WHITE = 15,
        TRANSPARENT_COLOR = 0xFF
    };

    constexpr WORD FG_TRANSPARENT_BIT = 0x0100;
    constexpr WORD BG_TRANSPARENT_BIT = 0x0200;

    constexpr WORD Make(Code fg, Code bg = BLACK) {
        WORD attr = 0;
        if (fg == TRANSPARENT_COLOR) attr |= FG_TRANSPARENT_BIT;
        else attr |= static_cast<WORD>(fg & 0x0F);

        if (bg == TRANSPARENT_COLOR) attr |= BG_TRANSPARENT_BIT;
        else attr |= (static_cast<WORD>(bg & 0x0F) << 4);

        return attr;
    }
}

// ============================================================================
// 1. CORE UI BOX WITH EVENT DISPATCH INTERFACES
// ============================================================================
struct UIBox {
    float width = 20;
    float height = 5;
    int offsetX = 0;
    int offsetY = 0;
    int zIndex = 0;
    Anchor anchor = Anchor::TOP_LEFT;
    TextAlign textAlign = TextAlign::LEFT;
    DimensionType dimensions = DimensionType::LITERAL;

    WORD color = Color::Make(Color::WHITE, Color::BLACK);
    WORD borderColor = Color::Make(Color::GRAY, Color::BLACK);

    bool visible = true;
    bool isModal = false; // When true, absorbs all input within its bounds
    bool hasBorder = true;
    bool fillBackground = true;
    bool transparentSpaces = false;
    char transparentChar = '\0';
    bool hasDropShadow = false;

    std::string title = "";
    std::vector<std::string> content;
    std::unordered_map<int, WORD> lineColors;

    virtual ~UIBox() = default;

    // Hit-testing in viewport coordinates
    //virtual bool containsPoint(int vx, int vy, const DisplayEngine& engine) const;

    void fitToContent(int paddingX = 2, int paddingY = 1) {
        int maxLen = static_cast<int>(title.length());
        for (const auto& line : content) {
            maxLen = (std::max)(maxLen, static_cast<int>(line.length()));
        }
        width = static_cast<float>(maxLen + (paddingX * 2) + (hasBorder ? 2 : 0));
        height = static_cast<float>(content.size() + (paddingY * 2) + (hasBorder ? 2 : 0));
    }
};

// ============================================================================
// 2. DYNAMIC MENU BUILDER (Inherits UIBox)
// ============================================================================
struct MenuItem {
    std::string label;
    bool enabled = true;
    std::function<void()> onSelect = nullptr;
};

class UIMenu : public UIBox {
private:
    std::vector<MenuItem> items;
    int selectedIndex = 0;
    int scrollOffset = 0;

public:
    int maxVisibleItems = 6;
    int paddingX = 1;
    bool autoFit = true;

    std::string cursorPrefix = " > ";
    std::string idlePrefix = "   ";

    WORD selectedColor = Color::Make(Color::BLACK, Color::CYAN);
    WORD disabledColor = Color::Make(Color::DARK_GRAY, Color::DARK_BLUE);

    UIMenu(const std::string& menuTitle = "", const std::vector<std::string>& labels = {});

    void setItems(const std::vector<std::string>& labels);
    void addItem(const std::string& label, std::function<void()> callback = nullptr, bool enabled = true);
    void setItemEnabled(int index, bool enabled);
    void moveUp();
    void moveDown();
    void activate() const;

    int getSelectedIndex() const { return selectedIndex; }
    std::string getSelectedLabel() const { return items.empty() ? "" : items[selectedIndex].label; }
    void rebuild();
};

// ============================================================================
// 3. SPRITE FILE LOADER (.txt -> UIBox)
// ============================================================================
class SpriteLoader {
public:
    static UIBox fromFile(
        const std::string& filepath,
        Color::Code fgColor = Color::WHITE,
        Anchor anchor = Anchor::CENTER,
        char transparentKey = ' '
    );
};

// ============================================================================
// 6. DISPLAY ENGINE (Renderer & Viewport Scaler)
// ============================================================================
class DisplayEngine {
private:
    HANDLE hConsole;
    HWND hwndConsole;

    int bufferWidth = 0;
    int bufferHeight = 0;

    int targetColumns;
    int targetRows;
    int viewportOffsetX = 0;
    int viewportOffsetY = 0;

    WORD defaultBackground;
    WORD marginColor = Color::Make(Color::DARK_GRAY, Color::BLACK);
    WORD marginFrameColor = Color::Make(Color::CYAN, Color::BLACK);
    char marginPatternChar = '\xB0';

    std::vector<CHAR_INFO> backBuffer;

    void ensureLegacyConhost();
    void configureFullscreenAndFont();
    void drawViewportBorder();

    void blendCell(int x, int y, char ch, WORD attr, bool preserveChar = false);
    void shadowCell(int x, int y);
    void setPhysicalCell(int physX, int physY, char ch, WORD attr);

public:
    DisplayEngine(int targetCols = 120, int targetRows = 40);
    ~DisplayEngine() = default;

    void updateDimensions();

    void setMarginStyle(char patternChar, WORD patternColor, WORD borderColor) {
        marginPatternChar = patternChar;
        marginColor = patternColor;
        marginFrameColor = borderColor;
    }

    COORD resolveAnchor(const UIBox& box, int parentX, int parentY, int parentW, int parentH) const;
    COORD getBoxScreenPos(const UIBox& box) const;

    void clearBuffer(WORD bgColor = Color::Make(Color::WHITE, Color::BLACK));
    void drawBox(const UIBox& box);
    void drawChildBox(const UIBox& parent, const UIBox& child);
    void drawLayers(std::vector<const UIBox*> boxes);

    void present();

    int getWidth() const { return targetColumns; }
    int getHeight() const { return targetRows; }
    int getPhysicalWidth() const { return bufferWidth; }
    int getPhysicalHeight() const { return bufferHeight; }
    int getViewportOffsetX() const { return viewportOffsetX; }
    int getViewportOffsetY() const { return viewportOffsetY; }
};