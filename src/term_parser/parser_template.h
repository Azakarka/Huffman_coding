//
// Created by Amir on 10/22/22.
//

#ifndef CPP_PILOT_HSE_PARSER_TEMPLATE_H
#define CPP_PILOT_HSE_PARSER_TEMPLATE_H
#include <string>
#include <string_view>
#include <vector>
#include <exception>
#include <tuple>
#include <stdexcept>

using ParserRVal = std::vector<std::string_view>;
enum struct ArgType {
    REQUIRED,
    OPTIONAL,
};

enum struct ArgMultiple { ZERO, ONE, MULTIPLE, ANY };
template <typename T>
struct Arg {
    std::string command_;
    ArgType type_;
    ArgMultiple multiple_;
    T &value_;
    using Operation = void(T &, ParserRVal);
    Operation *func_;

    Arg<T, std::string>(std::string command, ArgType type, ArgMultiple multiple, T &value, Operation *func)
        : command_(std::move(command)), type_(type), multiple_(multiple), value_(value), func_(func) {
    }
};

template <typename Tuple>
class Parser {
    std::vector<std::string> argv_;
    Tuple args_;

public:
    Parser(int argc, char **argv, Tuple args) : args_(std::move(args)) {
        for (size_t i = 0; i < argc; ++i) {
            argv_.emplace_back(std::string(argv[i]));
        }
    }

    template <int size>
    void Parse() {
        auto &arg = std::get<size - 1>(args_);
        auto find_begin = [&arg, this]() {
            for (size_t i = 0; i < argv_.size(); ++i) {
                std::string_view command = arg.command_;
                if (argv_[i].starts_with(command)) {
                    return argv_.begin() + i;
                }
            }
            return argv_.end();
        };
        auto it = find_begin();
        if (it == argv_.end()) {
            if (arg.type_ == ArgType::REQUIRED) {
                throw std::logic_error("required command is not present");
            }
        } else {
            auto beg = ++it;
            for (; it != argv_.end(); ++it) {
                if ((*it)[0] == '-') {
                    break;
                }
            }
            auto cnt = (it - beg);
            switch (arg.multiple_) {
                case ArgMultiple::ZERO:
                    if (cnt != 0) {
                        throw std::logic_error("arg requires zero args, gets " + std::to_string(cnt));
                    }
                    ParseZero<decltype(arg)>(arg);
                    break;
                case ArgMultiple::ONE:
                    if (cnt != 1) {
                        throw std::logic_error("arg requires one gets " + std::to_string(cnt));
                    }
                    ParseOne<decltype(arg)>(arg, beg);
                    break;
                case ArgMultiple::MULTIPLE:
                    if (cnt < 2) {
                        throw std::logic_error("arg requires multiple ( > 1) arguments, gets " + std::to_string(cnt));
                    }
                    ParseMultiple<decltype(arg)>(arg, beg, it);
                    break;
                case ArgMultiple::ANY:
                    ParseAny<decltype(arg)>(arg, beg, it);
                    break;
            }
        }
        Parse<size - 1>();
    }

    template <>
    void Parse<0>() {
    }

private:
    template <typename Arg>
    void ParseZero(Arg &arg) {
        std::string_view loc;
        arg.func_(arg.value_, {loc});
    }

    template <typename Arg>
    void ParseOne(Arg &arg, auto it) {
        std::string_view str = *it;
        arg.func_(arg.value_, {str});
    }

    template <typename Arg>
    void ParseMultiple(Arg &arg, auto it, auto end) {
        ParserRVal rval;
        for (; it != end; ++it) {
            rval.push_back(*it);
        }
        arg.func_(arg.value_, rval);
    }

    template <typename Arg>
    void ParseAny(Arg &arg, auto it, auto end) {
        ParseMultiple(arg, it, end);
    }
};

#endif  // CPP_PILOT_HSE_PARSER_TEMPLATE_H
