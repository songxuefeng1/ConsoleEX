#include "io/export_import.hpp"
#include <fstream>
#include <cstring>
#include <vector>

// File format identifier
static constexpr const char* FILE_SIGNATURE = "CEX_EISS_v1";
static constexpr int SIGNATURE_SIZE = 12;

/**
 * Get current time as TimeInfo
 */
static TimeInfo get_current_time() {
    std::time_t now = std::time(nullptr);
    std::tm* time_info = std::localtime(&now);

    TimeInfo ti;
    ti.sec = time_info->tm_sec;
    ti.min = time_info->tm_min;
    ti.hour = time_info->tm_hour;
    ti.day = time_info->tm_mday;
    ti.month = time_info->tm_mon + 1;
    ti.week = time_info->tm_wday;
    ti.year = time_info->tm_year + 1900;

    return ti;
}

extern "C" {

EISS_API FileStruct EISS_Import(const char* filepath) {
    FileStruct result = {nullptr};

    if (filepath == nullptr) {
        result.name = _strdup("ERROR: Invalid filepath");
        return result;
    }

    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        result.name = _strdup("ERROR: File not found");
        return result;
    }

    // Read signature
    char signature[SIGNATURE_SIZE + 1] = {0};
    file.read(signature, SIGNATURE_SIZE);

    if (std::string(signature) != FILE_SIGNATURE) {
        result.name = _strdup("ERROR: Invalid file format");
        file.close();
        return result;
    }

    // Read header
    uint16_t version = 0;
    uint8_t use_timebar = 0;
    uint32_t name_length = 0;
    uint32_t text_count = 0;
    uint32_t timebar_count = 0;

    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    file.read(reinterpret_cast<char*>(&use_timebar), sizeof(use_timebar));
    file.read(reinterpret_cast<char*>(&name_length), sizeof(name_length));
    file.read(reinterpret_cast<char*>(&text_count), sizeof(text_count));
    file.read(reinterpret_cast<char*>(&timebar_count), sizeof(timebar_count));

    // Read filename
    char* filename = new char[name_length + 1];
    file.read(filename, name_length);
    filename[name_length] = '\0';
    result.name = filename;

    // Read creation and last modified times
    file.read(reinterpret_cast<char*>(&result.created_time), sizeof(TimeInfo));
    file.read(reinterpret_cast<char*>(&result.last_modified_time), sizeof(TimeInfo));

    // Read text content
    if (text_count > 0) {
        result.text = new const char*[text_count];
        for (uint32_t i = 0; i < text_count; ++i) {
            uint32_t line_length = 0;
            file.read(reinterpret_cast<char*>(&line_length), sizeof(line_length));
            
            char* line = new char[line_length + 1];
            file.read(line, line_length);
            line[line_length] = '\0';
            result.text[i] = line;
        }
    }

    // Read timebar if present
    if (use_timebar && timebar_count > 0) {
        result.timebar = new TimeInfo[timebar_count];
        file.read(reinterpret_cast<char*>(result.timebar),
                 timebar_count * sizeof(TimeInfo));
    }

    result.use_timebar = (use_timebar != 0);
    result.text_length = text_count;
    result.timebar_length = timebar_count;

    file.close();
    return result;
}

EISS_API bool EISS_Export(const FileStruct* file, const char* filepath) {
    if (file == nullptr || filepath == nullptr) {
        return false;
    }

    std::ofstream out(filepath, std::ios::binary);
    if (!out) {
        return false;
    }

    // Write signature
    out.write(FILE_SIGNATURE, SIGNATURE_SIZE);

    // Write header
    uint16_t version = 1;
    uint8_t use_timebar = file->use_timebar ? 1 : 0;
    uint32_t name_length = std::strlen(file->name);
    uint32_t text_count = file->text_length;
    uint32_t timebar_count = file->timebar_length;

    out.write(reinterpret_cast<const char*>(&version), sizeof(version));
    out.write(reinterpret_cast<const char*>(&use_timebar), sizeof(use_timebar));
    out.write(reinterpret_cast<const char*>(&name_length), sizeof(name_length));
    out.write(reinterpret_cast<const char*>(&text_count), sizeof(text_count));
    out.write(reinterpret_cast<const char*>(&timebar_count), sizeof(timebar_count));

    // Write filename
    out.write(file->name, name_length);

    // Write times
    out.write(reinterpret_cast<const char*>(&file->created_time), sizeof(TimeInfo));
    out.write(reinterpret_cast<const char*>(&file->last_modified_time), sizeof(TimeInfo));

    // Write text content
    for (int i = 0; i < file->text_length; ++i) {
        if (file->text[i] != nullptr) {
            uint32_t line_length = std::strlen(file->text[i]);
            out.write(reinterpret_cast<const char*>(&line_length), sizeof(line_length));
            out.write(file->text[i], line_length);
        }
    }

    // Write timebar if present
    if (use_timebar && file->timebar != nullptr) {
        out.write(reinterpret_cast<const char*>(file->timebar),
                 file->timebar_length * sizeof(TimeInfo));
    }

    out.close();
    return true;
}

EISS_API void EISS_FreeFileStruct(FileStruct* file) {
    if (file == nullptr) return;

    if (file->name != nullptr) {
        delete[] file->name;
        file->name = nullptr;
    }

    if (file->text != nullptr) {
        for (int i = 0; i < file->text_length; ++i) {
            if (file->text[i] != nullptr) {
                delete[] file->text[i];
            }
        }
        delete[] file->text;
        file->text = nullptr;
    }

    if (file->timebar != nullptr) {
        delete[] file->timebar;
        file->timebar = nullptr;
    }

    file->text_length = 0;
    file->timebar_length = 0;
}

} // extern "C"
