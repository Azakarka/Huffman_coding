#ifndef CPP_PILOT_HSE_TRIE_H
#define CPP_PILOT_HSE_TRIE_H

#include "encoder_common_files.h"
#include "../heap/heap.h"
#include <memory>
struct Trie {

    struct TrieNode {
        size_t ch_[2];
        CharType value_;
        bool terminate_;
    };

    void ResetPos();

    Trie() = default;

    bool Go(bool bit);

    void CanonicalView(CanonView& canon_view) const;

    void BuildTreeFromFrequencies(std::unordered_map<CharType, size_t>&& freq);

    void BuildTrieFromSizes(const Vec& sizes, const Vec& abc);

    CharType GetVal() const;

    ~Trie() = default;

private:
    std::vector<TrieNode> trie_;
    size_t cur_index_ = 1;
};

using TriePtr = std::unique_ptr<Trie>;

#endif  // CPP_PILOT_HSE_TRIE_H
