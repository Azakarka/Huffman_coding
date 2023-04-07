#ifndef CPP_PILOT_HSE_DECODER_H
#define CPP_PILOT_HSE_DECODER_H

#include "encoder_common_files.h"
#include "../term_parser/parser_common_files.h"
#include "trie.h"

class Decoder {
private:
    std::vector<Char> decoded_name_;
    std::vector<CharType> decoded_data_;
    TriePtr root_;
    IBitStream in_stream_;

    template <typename T>
    std::vector<T> DecodeData(bool& one_more_file) {
        std::vector<T> data;
        while (true) {
            CharType bit = in_stream_.ReadBits(1);
            while (root_->Go(bit)) {
                bit = in_stream_.ReadBits(1);
            }
            const auto& val = root_->GetVal();
            if (val >= Symbols::FILENAME_END) {
                if (val == Symbols::ONE_MORE_FILE) {
                    one_more_file = true;
                }
                return data;
            } else {
                data.push_back(val);
            }
        }
    }
    void VecToString(const std::vector<Char>& filename, std::string& name);
    void GetDecodeTrie();

    void Clear();
    void Print();

public:
    explicit Decoder(const ParsedName& working_file);

    void DecodeFile();
};

#endif