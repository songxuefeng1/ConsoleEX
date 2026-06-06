#include "io/editor.hpp"
#include "core/exception_manager.hpp"
#include <iostream>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
#endif

namespace cex::io {

bool Editor::raw_mode_enabled_ = false;

#ifdef _WIN32
static HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
static DWORD mode_original = 0;
#else
static termios original_termios;
#endif

void Editor::enable_raw_mode() {
#ifdef _WIN32
    GetConsoleMode(hstdin, &mode_original);
    SetConsoleMode(hstdin, mode_original & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
#else
    tcgetattr(STDIN_FILENO, &original_termios);
    termios raw = original_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
    raw_mode_enabled_ = true;
}

void Editor::disable_raw_mode() {
#ifdef _WIN32
    SetConsoleMode(hstdin, mode_original);
#else
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
#endif
    raw_mode_enabled_ = false;
}

void Editor::clear_screen_() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void Editor::hide_cursor_() {
    std::cout << "\033[?25l" << std::flush;
}

void Editor::show_cursor_() {
    std::cout << "\033[?25h" << std::flush;
}

void Editor::move_cursor_(int row, int col) {
    std::cout << "\033[" << (row + 1) << ";" << (col + 1) << "H" << std::flush;
}

char Editor::read_key_() {
#ifdef _WIN32
    return _getch();
#else
    char c = 0;
    read(STDIN_FILENO, &c, 1);
    return c;
#endif
}

std::vector<std::string> Editor::edit(const std::vector<std::string>& initial_content) {
    std::vector<std::string> lines = initial_content;
    if (lines.empty()) {
        lines.push_back("");
    }

    int cursor_x = 0;
    int cursor_y = 0;

    enable_raw_mode();
    hide_cursor_();

    auto cleanup = [](){ disable_raw_mode(); show_cursor_(); };
    atexit(cleanup);

    bool editing = true;
    while (editing) {
        clear_screen_();

        // Display content
        for (size_t i = 0; i < lines.size(); ++i) {
            move_cursor_(i, 0);
            std::cout << lines[i];
            std::cout << "\033[K"; // Clear to end of line
        }

        // Display status bar
        move_cursor_(lines.size(), 0);
        std::cout << "\033[7m Cursor: (" << cursor_y << "," << cursor_x << ") | "
                  << "Lines: " << lines.size() << " | Ctrl+S: Save, Ctrl+C: Exit \033[0m"
                  << std::flush;

        move_cursor_(cursor_y, cursor_x);
        std::cout << std::flush;

        char c = read_key_();

        if (c == 3) { // Ctrl+C
            editing = false;
        } else if (c == 19) { // Ctrl+S
            editing = false;
        } else if (c == '\r' || c == '\n') { // Enter
            std::string current = lines[cursor_y];
            std::string right = current.substr(cursor_x);
            lines[cursor_y] = current.substr(0, cursor_x);
            lines.insert(lines.begin() + cursor_y + 1, right);
            cursor_y++;
            cursor_x = 0;
        } else if (c == 127 || c == '\b') { // Backspace
            if (cursor_x > 0) {
                lines[cursor_y].erase(cursor_x - 1, 1);
                cursor_x--;
            } else if (cursor_y > 0) {
                cursor_x = lines[cursor_y - 1].size();
                lines[cursor_y - 1] += lines[cursor_y];
                lines.erase(lines.begin() + cursor_y);
                cursor_y--;
            }
        } else if (c >= 32 && c <= 126) { // Printable ASCII
            lines[cursor_y].insert(cursor_x, 1, c);
            cursor_x++;
        }

        // Boundary checks
        if (cursor_y >= (int)lines.size()) {
            cursor_y = lines.size() - 1;
        }
        if (cursor_x > (int)lines[cursor_y].size()) {
            cursor_x = lines[cursor_y].size();
        }
    }

    disable_raw_mode();
    show_cursor_();
    clear_screen_();

    ExceptionManager::log("Editor: " + std::to_string(lines.size()) + " lines saved.",
                         core::LogLevel::Success);

    return lines;
}

} // namespace cex::io
