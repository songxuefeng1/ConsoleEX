#ifndef CONSOLE_EX_EXPORT_IMPORT_HPP
#define CONSOLE_EX_EXPORT_IMPORT_HPP

#include <cstdint>
#include <ctime>

#ifdef _WIN32
    #ifdef BUILD_DLL
        #define EISS_API __declspec(dllexport)
    #else
        #define EISS_API __declspec(dllimport)
    #endif
#else
    #define EISS_API
#endif

/**
 * @struct TimeInfo
 * @brief Timestamp structure for cross-platform compatibility
 */
struct EISS_API TimeInfo {
    uint8_t  sec;
    uint8_t  min;
    uint8_t  hour;
    uint8_t  day;
    uint8_t  month;
    uint8_t  week;
    uint16_t year;
};

/**
 * @struct FileStruct
 * @brief File data structure for C interface
 */
struct EISS_API FileStruct {
    const char*  name;             // Filename (heap allocated, caller must free)
    const char** text;             // Text lines (heap allocated, caller must free)
    TimeInfo*    timebar;          // Modification timestamps (heap allocated, caller must free)
    TimeInfo     created_time;     // File creation time
    TimeInfo     last_modified_time; // Last modification time
    bool         use_timebar;      // Whether timebar is enabled
    int          text_length;      // Number of text lines
    int          timebar_length;   // Number of timestamps
};

/**
 * @namespace EISS (Export/Import System Support)
 * @brief C interface for file serialization
 */
extern "C" {
    /**
     * Import file from disk
     * @param filepath Path to the file to import
     * @return FileStruct containing loaded data
     */
    EISS_API FileStruct EISS_Import(const char* filepath);

    /**
     * Export file to disk
     * @param file FileStruct to export
     * @param filepath Path where to save the file
     * @return true if successful
     */
    EISS_API bool EISS_Export(const FileStruct* file, const char* filepath);

    /**
     * Free memory allocated for FileStruct
     * @param file FileStruct to free
     */
    EISS_API void EISS_FreeFileStruct(FileStruct* file);
}

#endif // CONSOLE_EX_EXPORT_IMPORT_HPP
