#include "terminal_parser.h"

Status TerminalParser::IsPathExists(std::string_view s) {
    struct stat buffer;
    if (s.empty() || stat(&s[0], &buffer) == 0) {
        return Status::OK;
    }
    return Status::INVALID_PATH;
}
Status TerminalParser::IsFileExists(std::string_view filename) {
    std::ifstream ifs;
    ifs.open(&filename[0]);
    if (!ifs.is_open()) {
        return Status::FILE_NOT_FOUND;
    }
    return Status::OK;
}

ParsedName TerminalParser::ParseName(std::string_view filename) {
    size_t index = 0;
    for (size_t i = 0; i < filename.size(); ++i) {
        if (filename[i] == '/') {
            index = i + 1;
        }
    }
    ParsedName result;
    for (size_t i = 0; i < index; ++i) {
        result.first += filename[i];
    }
    for (size_t i = index; i < filename.size(); ++i) {
        result.second += filename[i];
    }
    return result;
}

ParsedNames TerminalParser::ParseFiles(const Filenames& filenames, ParsedName& working_file) {
    working_file = ParseName(filenames[0]);
    ParsedNames result;
    for (size_t i = 1; i < filenames.size(); ++i) {
        result.emplace_back(ParseName(filenames[i]));
    }
    return result;
}

Status TerminalParser::ParseInput(int argc, char** argv, ParsedName& working_file, ParsedNames& output) {
    ArgValues values;
    void (*help)(bool&, ParserRVal) = [](bool& res, ParserRVal vals) { res = true; };
    void (*d_func)(std::string&, ParserRVal) = [](std::string& name, ParserRVal vals) { name = vals[0]; };
    void (*c_func)(std::vector<std::string>&, ParserRVal) = [](std::vector<std::string>& names, ParserRVal vals) {
        for (const auto& str : vals) {
            names.emplace_back(str);
        }
    };
    auto args = std::make_tuple(Arg("-h", ArgType::OPTIONAL, ArgMultiple::ZERO, values.is_help_, *help),
                                Arg("-d", ArgType::OPTIONAL, ArgMultiple::ONE, values.main_filename_, *d_func),
                                Arg("-c", ArgType::OPTIONAL, ArgMultiple::MULTIPLE, values.filenames_, *c_func));

    Parser<decltype(args)> parser(argc, argv, args);
    try {
        parser.Parse<std::tuple_size_v<decltype(args)>>();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        exit(111);
    }
    if (values.is_help_) {
        Help();
        return Status::HELP;
    } else if (!values.filenames_.empty()) {
        for (size_t i = 1; i < values.filenames_.size(); ++i) {
            if (auto status = IsFileExists(values.filenames_[i]); status != Status::OK) {
                return status;
            }
        }
        output = ParseFiles(values.filenames_, working_file);
        if (auto status = IsPathExists(working_file.first); status != Status::OK) {
            return status;
        }
        return Status::ENCODE;
    } else if (!values.main_filename_.empty()) {
        if (auto status = IsPathExists(values.main_filename_); status != Status::OK) {
            return status;
        }
        output = {{"", ""}};
        working_file = ParseName(values.main_filename_);
        return Status::DECODE;
    }
    return Status::OK;
}
void TerminalParser::Help() {
    const std::string info =
        "archiver -c archive_name file1 [file2 ...] - заархивировать файлы file1, file2, ... и сохранить результат в "
        "файл archive_name.\n"
        "archiver -d archive_name - разархивировать файлы из архива archive_name и положить в текущую директорию.\n"
        "archiver -h - вывести справку по использованию программы.";
    std::cout << info << std::endl;
}
