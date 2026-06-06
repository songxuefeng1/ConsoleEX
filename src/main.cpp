#include <iostream>
#include "core/exception_manager.hpp"
#include "core/action_manager.hpp"
#include "core/memory_manager.hpp"
#include "io/editor.hpp"

using namespace cex::core;
using namespace cex::io;

void display_menu() {
    std::cout << "\n========== ConsoleEX v1.0 ==========" << std::endl;
    std::cout << "1. Create new file" << std::endl;
    std::cout << "2. Edit file" << std::endl;
    std::cout << "3. View file" << std::endl;
    std::cout << "4. Delete file" << std::endl;
    std::cout << "5. List all files" << std::endl;
    std::cout << "6. Clear all files" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Select option: ";
}

void create_file_menu() {
    std::vector<std::string> content = ActionManager::get_file_creation_input();
    if (content.empty()) {
        ExceptionManager::log("No content provided.", LogLevel::Warning);
        return;
    }

    std::string filename;
    std::cout << "Enter filename: ";
    std::getline(std::cin, filename);

    if (MemoryManager::add_file(filename, content)) {
        ExceptionManager::log("File created and saved.", LogLevel::Success);
    }
}

void edit_file_menu() {
    if (MemoryManager::file_count() == 0) {
        ExceptionManager::log("No files available.", LogLevel::Notice);
        return;
    }

    std::cout << "Available files:" << std::endl;
    auto files = MemoryManager::list_files();
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << (i + 1) << ". " << files[i] << std::endl;
    }

    std::cout << "Select file number: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(); // Clear newline

    if (choice < 1 || choice > (int)files.size()) {
        ExceptionManager::log("Invalid selection.", LogLevel::Error);
        return;
    }

    std::string filename = files[choice - 1];

    if (ActionManager::confirm_file_edit(filename)) {
        auto current_content = MemoryManager::get_file_content(filename);
        auto edited_content = Editor::edit(current_content);
        MemoryManager::update_file(filename, edited_content);
    }
}

void view_file_menu() {
    if (MemoryManager::file_count() == 0) {
        ExceptionManager::log("No files available.", LogLevel::Notice);
        return;
    }

    std::cout << "Available files:" << std::endl;
    auto files = MemoryManager::list_files();
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << (i + 1) << ". " << files[i] << std::endl;
    }

    std::cout << "Select file number: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    if (choice < 1 || choice > (int)files.size()) {
        ExceptionManager::log("Invalid selection.", LogLevel::Error);
        return;
    }

    std::string filename = files[choice - 1];
    auto content = MemoryManager::get_file_content(filename);

    std::cout << "\n=== File: " << filename << " ===" << std::endl;
    for (size_t i = 0; i < content.size(); ++i) {
        std::cout << (i + 1) << ": " << content[i] << std::endl;
    }
    std::cout << "==========================\n" << std::endl;
}

void delete_file_menu() {
    if (MemoryManager::file_count() == 0) {
        ExceptionManager::log("No files available.", LogLevel::Notice);
        return;
    }

    std::cout << "Available files:" << std::endl;
    auto files = MemoryManager::list_files();
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << (i + 1) << ". " << files[i] << std::endl;
    }

    std::cout << "Select file number: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    if (choice < 1 || choice > (int)files.size()) {
        ExceptionManager::log("Invalid selection.", LogLevel::Error);
        return;
    }

    std::string filename = files[choice - 1];

    if (ActionManager::confirm_file_deletion(filename)) {
        MemoryManager::delete_file(filename);
    }
}

void list_files_menu() {
    size_t count = MemoryManager::file_count();

    if (count == 0) {
        ExceptionManager::log("No files in memory.", LogLevel::Notice);
        return;
    }

    std::cout << "\n=== Files in Memory ===" << std::endl;
    auto files = MemoryManager::list_files();
    for (size_t i = 0; i < files.size(); ++i) {
        auto content = MemoryManager::get_file_content(files[i]);
        std::cout << (i + 1) << ". " << files[i] << " (" << content.size() << " lines)"
                  << std::endl;
    }
    std::cout << "======================" << std::endl;
}

int main() {
    ExceptionManager::log("ConsoleEX v1.0 - File Management System", LogLevel::Success);

    int choice = -1;

    while (choice != 0) {
        display_menu();
        std::cin >> choice;
        std::cin.ignore(); // Clear newline from input buffer

        ActionManager::clear_screen();

        switch (choice) {
            case 1:
                create_file_menu();
                break;
            case 2:
                edit_file_menu();
                break;
            case 3:
                view_file_menu();
                break;
            case 4:
                delete_file_menu();
                break;
            case 5:
                list_files_menu();
                break;
            case 6:
                MemoryManager::clear_all();
                break;
            case 0:
                ExceptionManager::log("Goodbye!", LogLevel::Success);
                break;
            default:
                ExceptionManager::log("Invalid option. Please try again.", LogLevel::Error);
        }

        if (choice != 0 && choice != 5 && choice != 6) {
            ActionManager::pause();
        }
    }

    return 0;
}
