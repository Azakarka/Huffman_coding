#ifndef CPP_PILOT_HSE_TERMINAL_PARSER_H
#define CPP_PILOT_HSE_TERMINAL_PARSER_H
#include "parser_template.h"
#include <string>
#include <string.h>  // for strcmp
#include <vector>
#include <optional>
#include <fstream>
#include <iostream>
#include "parser_common_files.h"
#include <sys/stat.h>

enum class Status {
    OK,
    FEW_ARGUMENTS,
    HELP,
    ENCODE,
    DECODE,
    UNKNOWN_QUERY,
    FILE_NOT_FOUND,
    TOO_MANY_FILES,
    INVALID_PATH,
    EXTRA_ARGUMENTS,
};

struct ArgValues {
    bool is_help_ = false;
    std::string main_filename_;
    std::vector<std::string> filenames_;
};

class TerminalParser {

    void Help();
    Status IsPathExists(std::string_view s);

    Status IsFileExists(std::string_view filename);

    ParsedNames ParseFiles(const Filenames& filenames, ParsedName& working_file);

    ParsedName ParseName(std::string_view filename);

public:
    TerminalParser() = default;

    Status ParseInput(int argc, char** argv, ParsedName& working_file, ParsedNames& output);
};

#endif  // CPP_PILOT_HSE_TERMINAL_PARSER_H
