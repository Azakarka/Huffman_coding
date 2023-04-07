#include "decoder.h"

Decoder::Decoder(const ParsedName& working_file) : in_stream_(working_file.first, working_file.second) {
}

void Decoder::GetDecodeTrie() {
    CharType symbols_count = in_stream_.ReadBits(ENCODED_BIT_LEN);
    Vec abc(symbols_count);
    for (size_t i = 0; i < symbols_count; ++i) {
        abc[i] = in_stream_.ReadBits(ENCODED_BIT_LEN);
    }
    Vec sizes;
    while (symbols_count != 0) {
        auto size = in_stream_.ReadBits(ENCODED_BIT_LEN);
        sizes.emplace_back(size);
        symbols_count -= size;
    }
    root_ = std::unique_ptr<Trie>(new Trie());
    root_->BuildTrieFromSizes(sizes, abc);
}

void Decoder::Clear() {
    decoded_data_.clear();
    decoded_name_.clear();
    root_.reset();
}

void Decoder::Print() {
    std::string new_name;
    VecToString(decoded_name_, new_name);
    std::string dir;
    OBitStream bts(dir, new_name);
    for (const auto& c : decoded_data_) {
        bts.WriteBits(DECODED_BIT_LEN, c);
    }
}

void Decoder::DecodeFile() {
    bool one_more_file = true;
    while (one_more_file) {
        one_more_file = false;
        Clear();
        GetDecodeTrie();
        decoded_name_ = DecodeData<Char>(one_more_file);
        decoded_data_ = DecodeData<CharType>(one_more_file);
        Print();
    }
    in_stream_.Close();
}
void Decoder::VecToString(const std::vector<Char>& filename, std::string& name) {
    for (size_t i = 0; i < filename.size(); ++i) {
        name += filename[i];
    }
}
