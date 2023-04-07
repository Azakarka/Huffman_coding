#include <cctype>
#include <cstdio>
#include "../coders/encoder_common_files.h"
#include "../term_parser/parser_common_files.h"
#include "../coders/encoder.h"
#include "../coders/decoder.h"
#include "../term_parser/terminal_parser.h"

const std::string PATH_TO_DATA = "../tasks/archiver/tests/";

bool CompFiles(const std::string &file1, const std::string &file2) {
    std::ifstream in_1(file1);
    std::ifstream in_2(file2);
    while (!in_1.eof() && !in_2.eof()) {
        char byte1 = 0;
        char byte2 = 0;
        in_1.read(&byte1, 1);
        in_2.read(&byte2, 1);
        if (byte1 != byte2) {
            return false;
        }
    }
    if ((!in_1.eof()) ^ (!in_2.eof())) {
        return false;
    }

    return true;
}

void parser_base_test() {
    const std::vector<std::string> vec = {"-h", "-d", "aboba", "-c", "asda", "boba", "burum"};
    char *argv[vec.size()];
    for (size_t i = 0; i < vec.size(); ++i) {
        const auto &string = vec[i];
        argv[i] = new char[string.size()];
        strcpy(argv[i], string.c_str());
    }
    bool v_help = false;
    void (*f_help)(bool &, ParserRVal) = [](bool &b, ParserRVal vals) { b = true; };
    std::string v_d;
    void (*f_d)(std::string &, ParserRVal) = [](std::string &b, ParserRVal vals) { b = vals[0]; };
    std::vector<std::string> v_c;
    void (*f_c)(std::vector<std::string> &, ParserRVal) = [](std::vector<std::string> &vec, ParserRVal vals) {
        for (const auto &filename: vals) {
            vec.emplace_back(filename);
        }
    };
    auto args = std::make_tuple(Arg("-h", ArgType::OPTIONAL, ArgMultiple::ZERO, v_help, *f_help),
                                Arg("-d", ArgType::OPTIONAL, ArgMultiple::ONE, v_d, *f_d),
                                Arg("-c", ArgType::OPTIONAL, ArgMultiple::MULTIPLE, v_c, *f_c));
    Parser<decltype(args)> parser(vec.size(), argv, args);
    try {
        parser.Parse<std::tuple_size_v<decltype(args)>>();
    } catch (...) {
        assert(0);
    }
    assert(v_help == true);
    assert(v_d == "aboba");
    std::vector check = {vec[4], vec[5], vec[6]};
    assert(v_c == check);
    for (size_t i = 0; i < vec.size(); ++i) {
        delete[] argv[i];
    }
}

void parser_test_bad() {
    const std::vector<std::vector<std::string>> vec_global = {
            {"-h", "privet"},
            {"-d"},
            {"-c", "only_one"},
            {"-d", "too", "much", "files"}};
    for (const auto &vec: vec_global) {
        char *argv[vec.size()];
        for (size_t i = 0; i < vec.size(); ++i) {
            const auto &string = vec[i];
            argv[i] = new char[string.size()];
            strcpy(argv[i], string.c_str());
        }
        bool v_help = false;
        void (*f_help)(bool &, ParserRVal) = [](bool &b, ParserRVal vals) { b = true; };
        std::string v_d;
        void (*f_d)(std::string &, ParserRVal) = [](std::string &b, ParserRVal vals) { b = vals[0]; };
        std::vector<std::string> v_c;
        void (*f_c)(std::vector<std::string> &, ParserRVal) = [](std::vector<std::string> &vec, ParserRVal vals) {
            for (const auto &filename: vals) {
                vec.emplace_back(filename);
            }
        };
        auto args = std::make_tuple(Arg("-h", ArgType::OPTIONAL, ArgMultiple::ZERO, v_help, *f_help),
                                    Arg("-d", ArgType::OPTIONAL, ArgMultiple::ONE, v_d, *f_d),
                                    Arg("-c", ArgType::OPTIONAL, ArgMultiple::MULTIPLE, v_c, *f_c));
        Parser<decltype(args)> parser(vec.size(), argv, args);
        try {
            parser.Parse<std::tuple_size_v<decltype(args)>>();
        } catch (std::logic_error &e) {
            assert(1);
            for (size_t i = 0; i < vec.size(); ++i) {
                delete[] argv[i];
            }
            continue;
        } catch (...) {
            for (size_t i = 0; i < vec.size(); ++i) {
                delete[] argv[i];
            }
            assert(0);
        }
        for (size_t i = 0; i < vec.size(); ++i) {
            delete[] argv[i];
        }
        assert(0);
    }
}

void input_output_positive() {
    const std::string path = PATH_TO_DATA + "test_IO/";
    const std::string path_to_result = path + "results/";
    std::vector<std::string> files = {"a", "forest.mp4", "master_i_margarita.txt", "my_photo_0.jpg", "exists"};
    for (const auto &filename: files) {
        IBitStream ibts(path, filename);
        Vec data;
        while (!ibts.Finished()) {
            data.emplace_back(ibts.ReadBits(CHAR_SIZE));
        }
        ibts.Close();
        OBitStream obts(path_to_result, filename);
        for (const auto &byte: data) {
            obts.WriteBits(CHAR_SIZE, byte);
        }
        obts.Close();
        assert(CompFiles(path + filename, path_to_result + filename));
    }
}

void input_output_positive_more_bits() {
    const std::string path = PATH_TO_DATA + "test_IO/";
    const std::string path_to_result = path + "results/";
    std::vector<std::string> files = {"a", "forest.mp4", "master_i_margarita.txt", "my_photo_0.jpg", "exists"};
    for (BitSize bits_cnt: {1, 2, 4, 8}) {
        for (const auto &filename: files) {
            IBitStream ibts(path, filename);
            Vec data;
            while (!ibts.Finished()) {
                data.emplace_back(ibts.ReadBits(bits_cnt));
            }
            ibts.Close();
            OBitStream obts(path_to_result, filename);
            for (const auto &byte: data) {
                obts.WriteBits(bits_cnt, byte);
            }
            obts.Close();
            assert(CompFiles(path + filename, path_to_result + filename));
        }
    }
}

void encoder_decoder_test() {
    const std::string path = PATH_TO_DATA + "test_IO/";
    const std::string path_to_result = path + "results/";
    std::vector<std::string> files = {"a", "forest.mp4", "master_i_margarita.txt", "my_photo_0.jpg", "exists"};
    std::vector<std::string> files2 = {"__a_", "__forest_.mp4", "__master_i_margarita_.txt", "__my_photo_0_.jpg",
                                       "__exists_"};
    for (size_t i = 0; i < files.size(); ++i) {
        Encoder encoder({path_to_result, files2[i]});
        encoder.EncodeFiles({{path, files[i]}});
        Decoder decoder({path_to_result, files2[i]});
        decoder.DecodeFile();
        assert(CompFiles(path + files[i], path_to_result + files[i]));
    }
}

void encoder_decoder_test_multiple() {
    const std::string path = PATH_TO_DATA + "test_IO/";
    const std::string path_to_result = path + "results/";
    std::vector<std::string> files = {"a", "forest.mp4", "master_i_margarita.txt", "my_photo_0.jpg", "exists"};
    Encoder encoder({path_to_result, "__MULTIPLE_FILES"});
    ParsedNames names;
    for (const auto &filename: files) {
        names.push_back({path, filename});
    }
    encoder.EncodeFiles(names);
    Decoder decoder({path_to_result, "__MULTIPLE_FILES"});
    decoder.DecodeFile();
    for (size_t i = 0; i < files.size(); ++i) {
        assert(CompFiles(path + files[i], path_to_result + files[i]));
    }
}

void input_not_exists_parser() {
    std::string dir = PATH_TO_DATA + "test_IO/results/exists";
    const char *file1 = dir.c_str();
    const char file2[] = "love_memes";
    const char file3[] = "very_much";
    const char param[] = "-c";
    char *files[5];
    files[1] = new char[strlen(param)];
    memcpy(files[1], param, strlen(param));
    files[2] = new char[strlen(file1)];
    memcpy(files[2], file1, strlen(file1));
    files[3] = new char[strlen(file2)];
    memcpy(files[3], file2, strlen(file2));
    files[4] = new char[strlen(file3)];
    memcpy(files[4], file3, strlen(file3));
    TerminalParser parser;
    ParsedName w_file;
    ParsedNames out;
    auto result = parser.ParseInput(5, files, w_file, out);
    for (size_t i = 1; i < 4; ++i) {
        delete[] files[i];
    }
    assert(result == Status::FILE_NOT_FOUND);
}

void delete_encoded_file() {
    const std::string path1 = PATH_TO_DATA + "test_IO/results/";
    const std::string path2 = PATH_TO_DATA + "test_IO/";
    const std::string name1 = "a_";
    const std::string name2 = "a";
    Encoder encoder({path1, name1});
    encoder.EncodeFiles({{path2, name2}});
    remove(std::string(path1 + name1).c_str());
    try {
        Decoder decoder({path1, name1});
        decoder.DecodeFile();
    } catch (std::ios_base::failure &e) {
        assert(1);
    } catch (...) {
        assert(0);
    }
}

void input_no_exists_encoder() {
    std::string filename = "this_file_should_not_exist||tmp_1kmoim24r4";
    std::string filename2 = "this_file_should_not_exist||tmp_398uj89n24";
    try {
        Encoder encoder({"", filename});
        encoder.EncodeFiles({{"", filename2}});
    } catch (const std::ios_base::failure &e) {
        assert(1);
    } catch (...) {
        assert(0);
    }
}

void input_no_exists_decoder() {
    std::string filename = "this_file_should_not_exist||tmp_98jfn248n42r";
    try {
        Decoder decoder({"", filename});
        decoder.DecodeFile();
    } catch (const std::ios_base::failure &e) {
        assert(1);
    } catch (...) {
        assert(0);
    }
}

void heap_base() {
    std::priority_queue<int> pq;
    std::mt19937 rand(time(0));
    Heap<int> heap;
    const size_t size = 100000;
    for (size_t i = 0; i < size; ++i) {
        int x = rand();
        pq.emplace(x);
        heap.Push(x);
    }
    while (!pq.empty()) {
        assert(heap.Size() == pq.size());
        assert(heap.Top() == pq.top());
        heap.Pop();
        pq.pop();
    }
}

void heap_bad_pop() {
    Heap<int> heap;
    heap.Push(0);
    heap.Pop();
    try {
        heap.Pop();
    } catch (std::out_of_range &e) {
        assert(1);
    } catch (...) {
        assert(0);
    }
}

void heap_pair() {
    using T = std::pair<int, int>;
    std::priority_queue<T> pq;
    std::mt19937 rand(time(0));
    Heap<T> heap;
    const size_t size = 100000;
    for (size_t i = 0; i < size; ++i) {
        T x = std::make_pair(rand(), rand());
        pq.emplace(x);
        heap.Push(x);
    }
    while (!pq.empty()) {
        assert(heap.Size() == pq.size());
        assert(heap.Top() == pq.top());
        heap.Pop();
        pq.pop();
    }
}

void heap_greater() {
    using T = std::pair<int, int>;
    auto cmp = [](T a, T b) { return a < b; };
    auto cmp_pq = [](T a, T b) { return a > b; };  // pq works this way
    std::priority_queue<T, std::vector<T>, decltype(cmp_pq)> pq(cmp_pq);
    std::mt19937 rand(time(0));
    Heap<T> heap(cmp);
    const size_t size = 100000;
    for (size_t i = 0; i < size; ++i) {
        T x = std::make_pair(rand(), rand());
        pq.emplace(x);
        heap.Push(x);
    }
    while (!pq.empty()) {
        assert(heap.Size() == pq.size());
        assert(heap.Top() == pq.top());
        heap.Pop();
        pq.pop();
    }
}

void canonic_view() {
    std::unordered_map<CharType, size_t> freq;
    size_t size = 1000;
    for (size_t i = 0; i < size; ++i) {
        int x = std::abs(rand());
        CharType y = rand();
        freq[y] += x;
    }
    CanonView correct;
    {
        struct Elem {
            CharType c_;
            size_t freq_;
            CharType len_ = 0;

            Elem(CharType c, size_t freq, size_t len = 0) : c_(c), freq_(freq), len_(len) {
            }
        };
        using Arr = std::vector<Elem>;
        auto cmp = [](const Arr &t1, const Arr &t2) {
            auto c1 = t1[0].c_;
            auto c2 = t2[0].c_;
            size_t sum1 = 0;
            size_t sum2 = 0;
            for (const auto &elem: t1) {
                sum1 += elem.freq_;
                c1 = std::min(c1, elem.c_);
            }
            for (const auto &elem: t2) {
                sum2 += elem.freq_;
                c2 = std::min(c2, elem.c_);
            }
            return std::tie(sum1, c1) < std::tie(sum2, c2);
        };
        Heap<Arr> heap(cmp);
        for (const auto &[c, cnt]: freq) {
            heap.Push({Elem(c, cnt)});
        }
        while (heap.Size() != 1) {
            auto vec1 = heap.Top();
            heap.Pop();
            auto vec2 = heap.Top();
            heap.Pop();
            std::vector<Elem> vec3;
            for (const auto &elem: vec1) {
                vec3.emplace_back(Elem(elem.c_, elem.freq_, elem.len_ + 1));
            }
            for (const auto &elem: vec2) {
                vec3.emplace_back(Elem(elem.c_, elem.freq_, elem.len_ + 1));
            }
            heap.Push(vec3);
        }
        std::map<size_t, std::vector<CharType>> sizes;
        for (const auto &elem: heap.Top()) {
            sizes[elem.len_].emplace_back(elem.c_);
        }
        std::vector<CharType> lens(sizes.rbegin()->first);
        std::vector<CharType> chars;
        for (auto &elem: sizes) {
            std::sort(elem.second.begin(), elem.second.end());
            for (const auto c: elem.second) {
                chars.emplace_back(c);
            }
            lens[elem.first - 1] = elem.second.size();
        }
        correct = std::make_pair(lens, chars);
    }
    CanonView mine;
    {
        TriePtr root = std::unique_ptr<Trie>(new Trie());
        root->BuildTreeFromFrequencies(std::move(freq));
        root->CanonicalView(mine);
    }
    assert(correct.first == mine.first);
    assert(correct.second == mine.second);
}

void bad_encode_file() {
    const std::string path = PATH_TO_DATA + "test_IO/";
    const std::string filename = "garbage_file";
    std::ofstream out(path + filename, std::ios::binary);
    const char *garbage = "1111";
    out.write(garbage, 4);
    out.close();
    Decoder decoder({path, filename});
    try {
        decoder.DecodeFile();
    } catch (std::ios_base::failure &e) {
        assert(1);
    } catch (...) {
        assert(0);
    }
}

int main() {
    parser_base_test();
    parser_test_bad();
    input_output_positive();
    input_output_positive_more_bits();
    encoder_decoder_test();
    encoder_decoder_test_multiple();
    input_not_exists_parser();
    delete_encoded_file();
    input_no_exists_encoder();
    input_no_exists_decoder();
    heap_base();
    heap_bad_pop();
    heap_pair();
    heap_greater();
    canonic_view();
    bad_encode_file();
}