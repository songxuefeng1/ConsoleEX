#ifndef CONSOLE_EX_EDITOR_HPP
#define CONSOLE_EX_EDITOR_HPP

#include <vector>
#include <string>

namespace cex::io {

/**
 * @class Editor
 * @brief Terminal-based text editor with line editing capabilities
 */
class Editor {
public:
    /**
     * Start the editor with optional initial content
     * @param initial_content Initial content to edit
     * @return Final edited content
     */
    static std::vector<std::string> edit(const std::vector<std::string>& initial_content = {});

    /**
     * Enable raw mode for terminal input
     */
    static void enable_raw_mode();

    /**
     * Disable raw mode (restore terminal to normal)
     */
    static void disable_raw_mode();

private:
    static bool raw_mode_enabled_;

    static void clear_screen_();
    static void hide_cursor_();
    static void show_cursor_();
    static void move_cursor_(int row, int col);
    static char read_key_();
};

} // namespace cex::io

#endif // CONSOLE_EX_EDITOR_HPP
