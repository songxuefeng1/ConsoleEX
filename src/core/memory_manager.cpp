#include "core/memory_manager.hpp"
#include "core/exception_manager.hpp"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace cex::core {

std::vector<std::unique_ptr<FileNode>> MemoryManager::memory_;

FileNode::FileNode(const std::string& filename, bool track_history)
    : name(filename), enable_history(track_history) {
    created_time = get_current_time();
    last_modified_time = created_time;
}

void FileNode::set_content(const std::vector<std::string>& new_content) {
    content = new_content;
    last_modified_time = get_current_time();

    if (enable_history) {
        modification_history.push_back(last_modified_time);
    }
}

void FileNode::append_line(const std::string& line) {
    content.push_back(line);
    last_modified_time = get_current_time();

    if (enable_history) {
        modification_history.push_back(last_modified_time);
    }
}

std::string FileNode::get_current_time() const {
    std::time_t now = std::time(nullptr);
    std::tm* time_info = std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(time_info, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// MemoryManager implementation

bool MemoryManager::add_file(const std::string& filename, const std::vector<std::string>& content,
                             bool track_history) {
    try {
        // Check for duplicate filename
        if (find_file_(filename) != nullptr) {
            ExceptionManager::log("File '" + filename + "' already exists.", LogLevel::Warning);
            return false;
        }

        auto new_file = std::make_unique<FileNode>(filename, track_history);
        new_file->set_content(content);
        memory_.push_back(std::move(new_file));

        ExceptionManager::log("File '" + filename + "' created successfully.", LogLevel::Success);
        return true;
    } catch (const std::exception& e) {
        ExceptionManager::log("Error creating file: " + std::string(e.what()), LogLevel::Error);
        return false;
    }
}

std::vector<std::string> MemoryManager::get_file_content(const std::string& filename) {
    FileNode* file = find_file_(filename);
    if (file == nullptr) {
        ExceptionManager::log("File '" + filename + "' not found.", LogLevel::Warning);
        return {};
    }
    return file->content;
}

bool MemoryManager::file_exists(const std::string& filename) {
    return find_file_(filename) != nullptr;
}

bool MemoryManager::delete_file(const std::string& filename) {
    try {
        auto it = std::find_if(memory_.begin(), memory_.end(),
                              [&filename](const std::unique_ptr<FileNode>& file) {
                                  return file->name == filename;
                              });

        if (it == memory_.end()) {
            ExceptionManager::log("File '" + filename + "' not found.", LogLevel::Warning);
            return false;
        }

        memory_.erase(it);
        ExceptionManager::log("File '" + filename + "' deleted.", LogLevel::Success);
        return true;
    } catch (const std::exception& e) {
        ExceptionManager::log("Error deleting file: " + std::string(e.what()), LogLevel::Error);
        return false;
    }
}

bool MemoryManager::update_file(const std::string& filename, const std::vector<std::string>& new_content) {
    FileNode* file = find_file_(filename);
    if (file == nullptr) {
        ExceptionManager::log("File '" + filename + "' not found.", LogLevel::Warning);
        return false;
    }

    file->set_content(new_content);
    ExceptionManager::log("File '" + filename + "' updated.", LogLevel::Success);
    return true;
}

std::vector<std::string> MemoryManager::list_files() {
    std::vector<std::string> filenames;
    for (const auto& file : memory_) {
        filenames.push_back(file->name);
    }
    return filenames;
}

size_t MemoryManager::file_count() {
    return memory_.size();
}

void MemoryManager::clear_all() {
    memory_.clear();
    ExceptionManager::log("All files cleared from memory.", LogLevel::Success);
}

FileNode* MemoryManager::find_file_(const std::string& filename) {
    for (auto& file : memory_) {
        if (file->name == filename) {
            return file.get();
        }
    }
    return nullptr;
}

} // namespace cex::core
