#include "encoder.h"
Encoder::Encoder(const ParsedName& encoding_file) : out_stream_(encoding_file.first, encoding_file.second) {
}

void Encoder::EncodeData() {
    encoded_data_.reserve(data_.size());
    for (auto& elem : data_) {
        encoded_data_.emplace_back(table_[elem]);
    }
}

void Encoder::CreateEncodeTable() {
    const auto& [sizes, vals] = canon_view_;
    size_t len = 1;
    size_t taken_cnt = 0;
    Path code = {0, 0};
    for (const auto& val : vals) {
        while (sizes[len - 1] == taken_cnt) {
            ++len;
            taken_cnt = 0;
        }
        while (code.first != len) {
            code.second <<= 1;
            ++code.first;
        }
        table_[val] = code;
        ++code.second;
        ++taken_cnt;
    }
}
void Encoder::CreateTrie(std::unordered_map<CharType, size_t>&& freq) {
    root_ = std::unique_ptr<Trie>(new Trie());
    root_->BuildTreeFromFrequencies(std::move(freq));
}

std::unordered_map<CharType, size_t> Encoder::CountFrequencies(const Vec& data) {
    std::unordered_map<CharType, size_t> map;
    for (const auto& elem : data) {
        ++map[elem];
    }
    map[Symbols::ONE_MORE_FILE] = 1;
    map[Symbols::ARCHIVE_END] = 1;
    map[Symbols::FILENAME_END] = 1;
    return map;
}

void Encoder::Encode() {
    CreateTrie(CountFrequencies(data_));
    root_->CanonicalView(canon_view_);
    CreateEncodeTable();
    EncodeData();
}

void Encoder::Clear() {
    data_.clear();
    encoded_data_.clear();
    canon_view_.first.clear();
    canon_view_.second.clear();
    table_.clear();
    root_.reset();
}

void Encoder::Print() {
    auto& [sizes, vals] = canon_view_;
    out_stream_.WriteBits(ENCODED_BIT_LEN, vals.size());
    for (auto c : vals) {
        out_stream_.WriteBits(ENCODED_BIT_LEN, c);
    }
    for (auto c : sizes) {
        out_stream_.WriteBits(ENCODED_BIT_LEN, c);
    }
    for (const auto& path : encoded_data_) {
        for (BitSize bit_cnt = path.first; bit_cnt > 0; --bit_cnt) {
            out_stream_.WriteBits(1, 0 != (path.second & (1 << (bit_cnt - 1))));
        }
    }
}

void Encoder::EncodeFiles(const ParsedNames& filenames) {
    size_t iter = 0;
    for (const auto& [dir, filename] : filenames) {
        Clear();
        IBitStream bts(dir, filename);
        // emplace filename
        for (size_t index = 0; index < filename.size(); ++index) {
            data_.emplace_back(filename[index]);
        }
        data_.push_back(Symbols::FILENAME_END);
        // emplace file data
        while (!bts.Finished()) {
            data_.push_back(bts.ReadBits(DECODED_BIT_LEN));
        }
        if (iter + 1 == filenames.size()) {
            data_.push_back(Symbols::ARCHIVE_END);
        } else {
            data_.push_back(Symbols::ONE_MORE_FILE);
        }
        ++iter;
        bts.Close();
        Encode();
        Print();
    }
    out_stream_.Close();
}