#include "bit_stream/bit_stream.h"
#include "term_parser/terminal_parser.h"
#include "coders/encoder.h"
#include "coders/decoder.h"

void StatusHandler(Status status, ParsedName& working_file, const ParsedNames& filenames) {
    std::string message;
    switch (status) {
        case Status::ENCODE:
            message = "encoding to file named " + working_file.second + "\n" + "files: \n";
            for (const auto& name : filenames) {
                message += name.first + name.second + "\n";
            }
            break;
        case Status::DECODE:
            message = "decoding file named " + working_file.second;
            break;
        case Status::OK:
            message = "OK";
            break;
        case Status::FEW_ARGUMENTS:
            message = "error: few arguments";
            break;
        case Status::HELP:
            message = "";
            break;
        case Status::UNKNOWN_QUERY:
            message = "error: couldn't recognize command";
            break;
        case Status::FILE_NOT_FOUND:
            message = "error: file not found";
            break;
        case Status::TOO_MANY_FILES:
            message = "error: too many files";
            break;
        case Status::INVALID_PATH:
            message = "error: invalid path, couldn't find path";
            break;
        case Status::EXTRA_ARGUMENTS:
            message = "error: extra arguments are present";
    }
    std::cout << message << std::endl;
    if (status == Status::ENCODE) {
        try {
            Encoder encoder(working_file);
            encoder.EncodeFiles(filenames);
            std::cout << "successfully encoded!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            exit(111);
        }
    } else if (status == Status::DECODE) {
        try {
            Decoder decoder(working_file);
            decoder.DecodeFile();
            std::cout << "successfully decoded!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            exit(111);
        }
    } else if (status == Status::OK) {
        exit(0);
    } else {
        exit(111);
    }
}

int main(int argc, char** argv) {
    TerminalParser parser;
    ParsedNames filenames;
    ParsedName working_file;
    auto result = parser.ParseInput(argc, argv, working_file, filenames);
    StatusHandler(result, working_file, filenames);
}
