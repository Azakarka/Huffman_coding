#include "bit_stream.h"

OBitStream::OBitStream(std::string_view working_dir, std::string_view filename) {
    Open(working_dir, filename);
}

void OBitStream::Open(std::string_view working_dir, std::string_view filename) {
    Close();
    std::string dir = std::string(working_dir);
    dir += filename;
    bit_stream_.Open(dir, std::ios::out | std::ios::binary);
    bits_taken_ = 0;
}

OBitStream::OBitStream(OBitStream&& other) {
    bit_stream_ = std::move(other.bit_stream_);
    std::swap(cur_byte_, other.cur_byte_);
    std::swap(bits_taken_, other.bits_taken_);
}

OBitStream& OBitStream::operator=(OBitStream&& other) {
    bit_stream_ = std::move(other.bit_stream_);
    std::swap(cur_byte_, other.cur_byte_);
    std::swap(bits_taken_, other.bits_taken_);
    other.bit_stream_.Close();
    return *this;
}

void OBitStream::WriteBits(BitSize bit_cnt, CharType elem) {
    BitSize suffix = bit_cnt;
    while (suffix != 0) {
        if (bits_taken_ == CHAR_SIZE) {
            bit_stream_.Write(cur_byte_);
            cur_byte_ = 0;
            bits_taken_ = 0;
        }
        BitSize write_cnt = std::min(CHAR_SIZE - bits_taken_, suffix);
        CharType bits = BitStream::BitSubStr(elem, suffix - write_cnt, suffix);
        cur_byte_ |= (bits << (CHAR_SIZE - bits_taken_ - write_cnt));
        bits_taken_ += write_cnt;
        suffix -= write_cnt;
    }
}

void OBitStream::Close() {
    if (!bit_stream_.IsOpen()) {
        return;
    }
    if (bits_taken_ != 0) {
        Char mask = std::numeric_limits<Char>::max();
        mask -= (1 << (CHAR_SIZE - bits_taken_)) - 1;
        bit_stream_.Write(cur_byte_ & mask);
    }
    bit_stream_.Close();
}

OBitStream::~OBitStream() {
    OBitStream::Close();
}