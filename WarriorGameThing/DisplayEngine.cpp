#include "DisplayEngine.h"
#include <cstdlib>

// ============================================================================
// UIBox Point Containment
// ============================================================================
//bool UIBox::containsPoint(int vx, int vy, const DisplayEngine& engine) const {
//    COORD pos = engine.getBoxScreenPos(*this);
//    return (vx >= pos.X && vx < pos.X + static_cast<int>(width) &&
//        vy >= pos.Y && vy < pos.Y + static_cast<int>(height));
//}

// ============================================================================
// UIMenu Implementation
// ============================================================================
UIMenu::UIMenu(const std::string& menuTitle, const std::vector<std::string>& labels) {
    title = menuTitle;
    hasDropShadow = true;
    color = Color::Make(Color::WHITE, Color::DARK_BLUE);
    borderColor = Color::Make(Color::CYAN, Color::DARK_BLUE);
    selectedColor = Color::Make(Color::BLACK, Color::CYAN);
    disabledColor = Color::Make(Color::DARK_GRAY, Color::DARK_BLUE);
    setItems(labels);
}

void UIMenu::setItems(const std::vector<std::string>& labels) {
    items.clear();
    for (const auto& l : labels) {
        items.push_back({ l, true, nullptr });
    }
    selectedIndex = 0;
    scrollOffset = 0;
    rebuild();
}

void UIMenu::addItem(const std::string& label, std::function<void()> callback, bool enabled) {
    items.push_back({ label, enabled, callback });
    rebuild();
}

void UIMenu::setItemEnabled(int index, bool enabled) {
    if (index >= 0 && index < static_cast<int>(items.size())) {
        items[index].enabled = enabled;
        rebuild();
    }
}

void UIMenu::moveUp() {
    if (items.empty()) return;
    selectedIndex = (selectedIndex - 1 + static_cast<int>(items.size())) % static_cast<int>(items.size());
    rebuild();
}

void UIMenu::moveDown() {
    if (items.empty()) return;
    selectedIndex = (selectedIndex + 1) % static_cast<int>(items.size());
    rebuild();
}

void UIMenu::activate() const {
    if (!items.empty() && items[selectedIndex].enabled && items[selectedIndex].onSelect) {
        items[selectedIndex].onSelect();
    }
}

void UIMenu::rebuild() {
    content.clear();
    lineColors.clear();
    if (items.empty()) return;

    if (selectedIndex < scrollOffset) scrollOffset = selectedIndex;
    if (selectedIndex >= scrollOffset + maxVisibleItems) {
        scrollOffset = selectedIndex - maxVisibleItems + 1;
    }

    size_t maxLabelLen = title.length() + 2;
    for (const auto& item : items) {
        size_t len = cursorPrefix.length() + item.label.length() + paddingX;
        if (len > maxLabelLen) maxLabelLen = len;
    }

    int visibleCount = (std::min)(static_cast<int>(items.size()), maxVisibleItems);
    for (int i = 0; i < visibleCount; ++i) {
        int itemIdx = scrollOffset + i;
        const auto& item = items[itemIdx];
        bool isSelected = (itemIdx == selectedIndex);

        std::string row = (isSelected ? cursorPrefix : idlePrefix) + item.label;
        if (row.length() < maxLabelLen) {
            row.append(maxLabelLen - row.length(), ' ');
        }

        if (i == 0 && scrollOffset > 0) row.back() = '^';
        if (i == visibleCount - 1 && scrollOffset + visibleCount < static_cast<int>(items.size())) row.back() = 'v';

        content.push_back(row);

        if (isSelected) lineColors[i] = selectedColor;
        else if (!item.enabled) lineColors[i] = disabledColor;
    }

    if (autoFit) fitToContent(0, 0);
}

// ============================================================================
// SpriteLoader Implementation
// ============================================================================
UIBox SpriteLoader::fromFile(const std::string& filepath, Color::Code fgColor, Anchor anchor, char transparentKey) {
    UIBox sprite;
    sprite.anchor = anchor;
    sprite.hasBorder = false;
    sprite.fillBackground = false;
    sprite.color = Color::Make(fgColor, Color::TRANSPARENT_COLOR);

    if (transparentKey == ' ') {
        sprite.transparentSpaces = true;
    }
    else {
        sprite.transparentSpaces = false;
        sprite.transparentChar = transparentKey;
    }

    std::ifstream file(filepath);
    if (!file.is_open()) {
        sprite.hasBorder = true;
        sprite.fillBackground = true;
        sprite.color = Color::Make(Color::WHITE, Color::DARK_RED);
        sprite.borderColor = Color::Make(Color::YELLOW, Color::DARK_RED);
        sprite.title = "MISSING SPRITE";
        sprite.content = { "Could not open:", filepath };
        sprite.fitToContent(1, 0);
        return sprite;
    }

    std::string line;
    size_t maxLineWidth = 0;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (sprite.content.empty() && !line.empty() && line[0] == '#') {
            if (line.rfind("#transparent=", 0) == 0 && line.length() >= 14) {
                char key = line[13];
                sprite.transparentSpaces = (key == ' ');
                sprite.transparentChar = (key == ' ') ? '\0' : key;
            }
            continue;
        }

        maxLineWidth = (std::max)(maxLineWidth, line.length());
        sprite.content.push_back(line);
    }

    char padChar = sprite.transparentSpaces ? ' ' : (sprite.transparentChar != '\0' ? sprite.transparentChar : ' ');
    for (auto& row : sprite.content) {
        if (row.length() < maxLineWidth) {
            row.append(maxLineWidth - row.length(), padChar);
        }
    }

    sprite.fitToContent(0, 0);
    return sprite;
}

// ============================================================================
// DisplayEngine Implementation
// ============================================================================
DisplayEngine::DisplayEngine(int targetCols, int targetRows)
    : targetColumns(targetCols), targetRows(targetRows),
    defaultBackground(Color::Make(Color::WHITE, Color::BLACK))
{
    ensureLegacyConhost();

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    hwndConsole = GetConsoleWindow();

    configureFullscreenAndFont();
    updateDimensions();

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void DisplayEngine::ensureLegacyConhost() {
    char envBuf[16];
    if (GetEnvironmentVariableA("RPG_CONHOST_ACTIVE", envBuf, sizeof(envBuf)) > 0) {
        return;
    }

    SetEnvironmentVariableA("RPG_CONHOST_ACTIVE", "1");

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string cmdLine = "conhost.exe \"" + std::string(exePath) + "\"";

    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi = {};

    if (CreateProcessA(NULL, cmdLine.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        std::exit(0);
    }
}

void DisplayEngine::configureFullscreenAndFont() {
    int monitorPxWidth = GetSystemMetrics(SM_CXSCREEN);
    int monitorPxHeight = GetSystemMetrics(SM_CYSCREEN);

    int desiredCols = targetColumns + 2;
    int desiredRows = targetRows + 2;

    int maxFontH_ByRows = monitorPxHeight / desiredRows;
    int maxFontW_ByCols = monitorPxWidth / desiredCols;
    int maxFontH_ByCols = maxFontW_ByCols * 2;

    SHORT fontHeight = static_cast<SHORT>((std::max)(8, (std::min)(maxFontH_ByRows, maxFontH_ByCols)));
    SHORT fontWidth = static_cast<SHORT>((std::max)(4, fontHeight / 2));

    LONG_PTR style = GetWindowLongPtr(hwndConsole, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_VSCROLL | WS_HSCROLL);
    style |= WS_POPUP;
    SetWindowLongPtr(hwndConsole, GWL_STYLE, style);

    LONG_PTR exStyle = GetWindowLongPtr(hwndConsole, GWL_EXSTYLE);
    exStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);
    SetWindowLongPtr(hwndConsole, GWL_EXSTYLE, exStyle);

    SetWindowPos(hwndConsole, HWND_TOP, 0, 0, monitorPxWidth, monitorPxHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    SMALL_RECT minimalRect = { 0, 0, 1, 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &minimalRect);

    CONSOLE_FONT_INFOEX cfi = {};
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    cfi.nFont = 0;
    cfi.dwFontSize.X = fontWidth;
    cfi.dwFontSize.Y = fontHeight;
    cfi.FontFamily = FF_MODERN | TMPF_VECTOR | TMPF_TRUETYPE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

    CONSOLE_FONT_INFOEX actualFont = {};
    actualFont.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    GetCurrentConsoleFontEx(hConsole, FALSE, &actualFont);
    int actualFontW = (actualFont.dwFontSize.X > 0) ? actualFont.dwFontSize.X : fontWidth;
    int actualFontH = (actualFont.dwFontSize.Y > 0) ? actualFont.dwFontSize.Y : fontHeight;

    COORD largestPossible = GetLargestConsoleWindowSize(hConsole);
    SHORT physCols = static_cast<SHORT>((std::min)(static_cast<int>(largestPossible.X), monitorPxWidth / actualFontW));
    SHORT physRows = static_cast<SHORT>((std::min)(static_cast<int>(largestPossible.Y), monitorPxHeight / actualFontH));

    physCols = (std::max)(physCols, static_cast<SHORT>(targetColumns));
    physRows = (std::max)(physRows, static_cast<SHORT>(targetRows));

    COORD exactBufferSize = { physCols, physRows };
    SetConsoleScreenBufferSize(hConsole, exactBufferSize);

    SMALL_RECT exactWindowRect = { 0, 0, static_cast<SHORT>(physCols - 1), static_cast<SHORT>(physRows - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &exactWindowRect);
    SetConsoleScreenBufferSize(hConsole, exactBufferSize);

    int exactPixelW = physCols * actualFontW;
    int exactPixelH = physRows * actualFontH;
    int winOffsetX = (monitorPxWidth - exactPixelW) / 2;
    int winOffsetY = (monitorPxHeight - exactPixelH) / 2;
    SetWindowPos(hwndConsole, HWND_TOP, winOffsetX, winOffsetY, exactPixelW, exactPixelH, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    ShowScrollBar(hwndConsole, SB_BOTH, FALSE);

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    SetConsoleMode(hIn, mode);
}

void DisplayEngine::updateDimensions() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        int newPhysW = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int newPhysH = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        if (newPhysW != bufferWidth || newPhysH != bufferHeight) {
            bufferWidth = newPhysW;
            bufferHeight = newPhysH;
            backBuffer.resize(bufferWidth * bufferHeight);

            COORD bufCoord = { static_cast<SHORT>(bufferWidth), static_cast<SHORT>(bufferHeight) };
            SetConsoleScreenBufferSize(hConsole, bufCoord);
            ShowScrollBar(hwndConsole, SB_BOTH, FALSE);

            viewportOffsetX = (std::max)(0, (bufferWidth - targetColumns) / 2);
            viewportOffsetY = (std::max)(0, (bufferHeight - targetRows) / 2);

            CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE };
            SetConsoleCursorInfo(hConsole, &cursorInfo);
        }
    }
}

void DisplayEngine::setPhysicalCell(int physX, int physY, char ch, WORD attr) {
    if (physX >= 0 && physX < bufferWidth && physY >= 0 && physY < bufferHeight) {
        int idx = physY * bufferWidth + physX;
        backBuffer[idx].Char.AsciiChar = ch;
        backBuffer[idx].Attributes = attr;
    }
}

void DisplayEngine::drawViewportBorder() {
    int left = viewportOffsetX - 1;
    int right = viewportOffsetX + targetColumns;
    int top = viewportOffsetY - 1;
    int bottom = viewportOffsetY + targetRows;

    const char TL = '\xC9', TR = '\xBB', BL = '\xC8', BR = '\xBC';
    const char H = '\xCD', V = '\xBA';

    setPhysicalCell(left, top, TL, marginFrameColor);
    setPhysicalCell(right, top, TR, marginFrameColor);
    setPhysicalCell(left, bottom, BL, marginFrameColor);
    setPhysicalCell(right, bottom, BR, marginFrameColor);

    for (int x = viewportOffsetX; x < viewportOffsetX + targetColumns; ++x) {
        setPhysicalCell(x, top, H, marginFrameColor);
        setPhysicalCell(x, bottom, H, marginFrameColor);
    }
    for (int y = viewportOffsetY; y < viewportOffsetY + targetRows; ++y) {
        setPhysicalCell(left, y, V, marginFrameColor);
        setPhysicalCell(right, y, V, marginFrameColor);
    }
}

void DisplayEngine::clearBuffer(WORD bgColor) {
    defaultBackground = bgColor & 0x00FF;

    for (int i = 0; i < bufferWidth * bufferHeight; ++i) {
        backBuffer[i].Char.AsciiChar = marginPatternChar;
        backBuffer[i].Attributes = marginColor;
    }

    drawViewportBorder();

    for (int y = 0; y < targetRows; ++y) {
        for (int x = 0; x < targetColumns; ++x) {
            setPhysicalCell(viewportOffsetX + x, viewportOffsetY + y, ' ', defaultBackground);
        }
    }
}

void DisplayEngine::blendCell(int x, int y, char ch, WORD attr, bool preserveChar) {
    if (x < 0 || x >= targetColumns || y < 0 || y >= targetRows) return;

    int physX = viewportOffsetX + x;
    int physY = viewportOffsetY + y;
    if (physX < 0 || physX >= bufferWidth || physY < 0 || physY >= bufferHeight) return;

    int index = physY * bufferWidth + physX;
    CHAR_INFO& dst = backBuffer[index];

    if (!preserveChar) {
        dst.Char.AsciiChar = ch;
    }

    WORD existingFG = dst.Attributes & 0x000F;
    WORD existingBG = dst.Attributes & 0x00F0;

    WORD newFG = (attr & Color::FG_TRANSPARENT_BIT) ? existingFG : (attr & 0x000F);
    WORD newBG = (attr & Color::BG_TRANSPARENT_BIT) ? existingBG : (attr & 0x00F0);

    dst.Attributes = newFG | newBG;
}

void DisplayEngine::shadowCell(int x, int y) {
    if (x < 0 || x >= targetColumns || y < 0 || y >= targetRows) return;

    int physX = viewportOffsetX + x;
    int physY = viewportOffsetY + y;
    if (physX < 0 || physX >= bufferWidth || physY < 0 || physY >= bufferHeight) return;

    int index = physY * bufferWidth + physX;
    CHAR_INFO& dst = backBuffer[index];

    WORD fg = dst.Attributes & 0x0007;
    if (fg == 0) fg = Color::DARK_GRAY;
    dst.Attributes = fg | (Color::BLACK << 4);
}

COORD DisplayEngine::resolveAnchor(const UIBox& box, int parentX, int parentY, int parentW, int parentH) const {
    int x = parentX;
    int y = parentY;
    int bW = static_cast<int>(box.width);
    int bH = static_cast<int>(box.height);

    switch (box.anchor) {
    case Anchor::TOP_LEFT:      x += 0;                 y += 0; break;
    case Anchor::TOP_CENTER:    x += (parentW - bW) / 2; y += 0; break;
    case Anchor::TOP_RIGHT:     x += parentW - bW;       y += 0; break;
    case Anchor::MID_LEFT:      x += 0;                 y += (parentH - bH) / 2; break;
    case Anchor::CENTER:        x += (parentW - bW) / 2; y += (parentH - bH) / 2; break;
    case Anchor::MID_RIGHT:     x += parentW - bW;       y += (parentH - bH) / 2; break;
    case Anchor::BOTTOM_LEFT:   x += 0;                 y += parentH - bH; break;
    case Anchor::BOTTOM_CENTER: x += (parentW - bW) / 2; y += parentH - bH; break;
    case Anchor::BOTTOM_RIGHT:  x += parentW - bW;       y += parentH - bH; break;
    }

    return { static_cast<SHORT>(x + box.offsetX), static_cast<SHORT>(y + box.offsetY) };
}

COORD DisplayEngine::getBoxScreenPos(const UIBox& box) const {
    return resolveAnchor(box, 0, 0, targetColumns, targetRows);
}

void DisplayEngine::drawBox(const UIBox& box) {
    if (!box.visible) return;
    UIBox viewportRoot;
    viewportRoot.width = static_cast<float>(targetColumns);
    viewportRoot.height = static_cast<float>(targetRows);
    drawChildBox(viewportRoot, box);
}

void DisplayEngine::drawChildBox(const UIBox& parent, const UIBox& child) {
    if (!child.visible) return;
    UIBox target = child;

    if (target.dimensions == DimensionType::PERCENT) {
        target.width = std::round(parent.width * target.width);
        target.height = std::round(parent.height * target.height);
    }

    COORD parentPos = { 0, 0 };
    if (static_cast<int>(parent.width) != targetColumns || static_cast<int>(parent.height) != targetRows) {
        parentPos = resolveAnchor(parent, 0, 0, targetColumns, targetRows);
    }

    COORD pos = resolveAnchor(target, parentPos.X, parentPos.Y, static_cast<int>(parent.width), static_cast<int>(parent.height));
    int startX = pos.X;
    int startY = pos.Y;
    int tW = static_cast<int>(target.width);
    int tH = static_cast<int>(target.height);

    if (target.hasDropShadow) {
        for (int y = 1; y <= tH; ++y) {
            for (int x = 2; x <= tW + 1; ++x) {
                shadowCell(startX + x, startY + y);
            }
        }
    }

    if (target.fillBackground) {
        bool keepUnderlyingChars = (target.color & Color::FG_TRANSPARENT_BIT) != 0;
        for (int y = 0; y < tH; ++y) {
            for (int x = 0; x < tW; ++x) {
                blendCell(startX + x, startY + y, ' ', target.color, keepUnderlyingChars);
            }
        }
    }

    if (target.hasBorder && tW >= 2 && tH >= 2) {
        const char TL = '\xDA', TR = '\xBF', BL = '\xC0', BR = '\xD9';
        const char H = '\xC4', V = '\xB3';

        blendCell(startX, startY, TL, target.borderColor);
        blendCell(startX + tW - 1, startY, TR, target.borderColor);
        blendCell(startX, startY + tH - 1, BL, target.borderColor);
        blendCell(startX + tW - 1, startY + tH - 1, BR, target.borderColor);

        for (int x = 1; x < tW - 1; ++x) {
            blendCell(startX + x, startY, H, target.borderColor);
            blendCell(startX + x, startY + tH - 1, H, target.borderColor);
        }
        for (int y = 1; y < tH - 1; ++y) {
            blendCell(startX, startY + y, V, target.borderColor);
            blendCell(startX + tW - 1, startY + y, V, target.borderColor);
        }

        if (!target.title.empty()) {
            std::string formattedTitle = " " + target.title + " ";
            int titleX = startX + 2;
            for (size_t i = 0; i < formattedTitle.size() && (titleX + static_cast<int>(i)) < (startX + tW - 1); ++i) {
                blendCell(titleX + static_cast<int>(i), startY, formattedTitle[i], target.borderColor);
            }
        }
    }

    int innerX = startX + (target.hasBorder ? 1 : 0);
    int innerY = startY + (target.hasBorder ? 1 : 0);
    int innerW = tW - (target.hasBorder ? 2 : 0);
    int innerH = tH - (target.hasBorder ? 2 : 0);

    for (size_t lineIdx = 0; lineIdx < target.content.size() && static_cast<int>(lineIdx) < innerH; ++lineIdx) {
        const std::string& line = target.content[lineIdx];
        int textX = innerX;

        if (target.textAlign == TextAlign::CENTER) {
            textX = innerX + (innerW - static_cast<int>(line.length())) / 2;
        }
        else if (target.textAlign == TextAlign::RIGHT) {
            textX = innerX + innerW - static_cast<int>(line.length());
        }

        WORD activeLineColor = target.color;
        auto colorIt = target.lineColors.find(static_cast<int>(lineIdx));
        if (colorIt != target.lineColors.end()) {
            activeLineColor = colorIt->second;
        }

        for (size_t charIdx = 0; charIdx < line.length(); ++charIdx) {
            char ch = line[charIdx];
            if (target.transparentSpaces && ch == ' ') continue;
            if (target.transparentChar != '\0' && ch == target.transparentChar) continue;

            int drawX = textX + static_cast<int>(charIdx);
            if (drawX >= innerX && drawX < innerX + innerW) {
                blendCell(drawX, innerY + static_cast<int>(lineIdx), ch, activeLineColor);
            }
        }
    }
}

void DisplayEngine::drawLayers(std::vector<const UIBox*> boxes) {
    std::stable_sort(boxes.begin(), boxes.end(), [](const UIBox* a, const UIBox* b) {
        return a->zIndex < b->zIndex;
        });
    for (const auto* box : boxes) {
        if (box) drawBox(*box);
    }
}

void DisplayEngine::present() {
    COORD bufferSize = { static_cast<SHORT>(bufferWidth), static_cast<SHORT>(bufferHeight) };
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT writeRegion = { 0, 0, static_cast<SHORT>(bufferWidth - 1), static_cast<SHORT>(bufferHeight - 1) };

    WriteConsoleOutputA(hConsole, backBuffer.data(), bufferSize, bufferCoord, &writeRegion);
}