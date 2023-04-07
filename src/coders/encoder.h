
#ifndef CPP_PILOT_HSE_ENCODER_H
#define CPP_PILOT_HSE_ENCODER_H
#include "encoder_common_files.h"
#include "../term_parser/parser_common_files.h"
#include "trie.h"
class Encoder {
private:
    Vec data_;
    std::vector<Path> encoded_data_;
    CanonView canon_view_;
    CodeTable table_;
    TriePtr root_;
    OBitStream out_stream_;

    void EncodeData();

    void CreateEncodeTable();

    void CreateTrie(std::unordered_map<CharType, size_t>&& freq);

    std::unordered_map<CharType, size_t> CountFrequencies(const Vec& data);

    void Encode();

    void Clear();

    void Print();

public:
    explicit Encoder(const ParsedName& encoding_file);

    void EncodeFiles(const ParsedNames& filenames);
};

#endif
