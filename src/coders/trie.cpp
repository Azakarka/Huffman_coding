#include "trie.h"

bool Trie::Go(bool bit) {
    if (trie_[cur_index_].terminate_) {
        ResetPos();
    }
    cur_index_ = trie_[cur_index_].ch_[bit];
    if (cur_index_ == 0) {
        throw std::out_of_range("accessing trie null element");
    }
    if (trie_[cur_index_].terminate_) {
        return false;
    }
    return true;
}
void Trie::ResetPos() {
    cur_index_ = 1;
}

void Trie::CanonicalView(CanonView& canon_view) const {
    auto& [sizes, vals] = canon_view;
    std::map<size_t, std::vector<size_t>> size_map;
    std::queue<std::pair<size_t, size_t>> deq;
    deq.push({1, 0});
    while (!deq.empty()) {
        auto [index, len] = deq.front();
        deq.pop();
        if (trie_[index].terminate_) {
            size_map[len].emplace_back(index);
            continue;
        }
        deq.push({trie_[index].ch_[0], len + 1});
        deq.push({trie_[index].ch_[1], len + 1});
    }
    sizes.resize(size_map.rbegin()->first);
    vals.reserve(trie_.size() / 2);
    size_t size_sum = 0;
    for (const auto& [len, indexes] : size_map) {
        for (const auto& index : indexes) {
            vals.emplace_back(trie_[index].value_);
        }
        std::sort(vals.begin() + size_sum, vals.begin() + size_sum + indexes.size());
        size_sum += indexes.size();
        sizes[len - 1] = indexes.size();
    }
}

void Trie::BuildTreeFromFrequencies(std::unordered_map<CharType, size_t>&& freq) {
    if (freq.empty()) {
        throw std::logic_error("Trie: giving empty frequency data");
    }
    trie_.resize(freq.size() * 2);
    using Trinity = std::tuple<size_t, CharType, size_t>;
    auto cmp = [](const Trinity& t1, const Trinity& t2) {
        return std::tie(std::get<0>(t1), std::get<1>(t1)) < std::tie(std::get<0>(t2), std::get<1>(t2));
    };
    Heap<Trinity> heap(cmp);
    size_t index = 2 * freq.size() - 1;
    for (auto& [key, val] : freq) {
        trie_[index] = {.ch_ = {0, 0}, .value_ = key, .terminate_ = true};
        heap.Push({val, key, index});
        --index;
    }
    while (heap.Size() != 1) {
        auto [cnt1, val1, index1] = heap.Top();
        heap.Pop();
        auto [cnt2, val2, index2] = heap.Top();
        heap.Pop();
        trie_[index] = {.ch_ = {index1, index2}, .value_ = std::min(val1, val2), .terminate_ = false};
        heap.Push({cnt1 + cnt2, std::min(val1, val2), index});
        --index;
    }
}

CharType Trie::GetVal() const {
    return trie_[cur_index_].value_;
}

void Trie::BuildTrieFromSizes(const Vec& sizes, const Vec& abc) {
    trie_.resize(2 * abc.size());
    size_t len = 1;
    size_t taken_cnt = 0;
    size_t index = 1;
    std::vector<std::pair<size_t, bool>> deq;
    deq.push_back({1, 0});
    ++index;
    for (const auto& val : abc) {
        while (sizes[len - 1] == taken_cnt) {
            ++len;
            taken_cnt = 0;
        }
        while (deq.size() != len) {
            auto [ind, status] = deq.back();
            trie_[ind].ch_[status] = index;
            deq.push_back({index, false});
            ++index;
        }
        trie_[index] = {
            .ch_ = {0, 0},
            .value_ = val,
            .terminate_ = true,
        };
        auto& [par, status] = deq.back();
        trie_[par].ch_[status] = index;
        while (!deq.empty() && deq.back().second) {
            deq.pop_back();
        }
        if (!deq.empty()) {
            deq.back().second = true;
        }
        ++taken_cnt;
        ++index;
    }
}
