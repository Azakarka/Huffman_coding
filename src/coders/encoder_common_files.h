#ifndef CPP_PILOT_HSE_ENCODER_COMMON_FILES_H
#define CPP_PILOT_HSE_ENCODER_COMMON_FILES_H

#include "../bit_stream/bit_stream.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <map>
#include <bitset>
enum Symbols { FILENAME_END = 256, ONE_MORE_FILE = 257, ARCHIVE_END = 258 };
using CharType = uint16_t;
using Char = uint8_t;
using Vec = std::vector<CharType>;
using Path = std::pair<size_t, uint64_t>;
using CodeTable = std::unordered_map<CharType, Path>;
using CanonView = std::pair<Vec, Vec>;
using BitSize = size_t;
const BitSize ENCODED_BIT_LEN = 9;
const BitSize DECODED_BIT_LEN = 8;

#endif