#include "bit_stream.h"

IBitStream::IBitStream(std::string_view working_dir, std::string_view filename) {
    Open(working_dir, filename);
}

IBitStream::IBitStream(IBitStream &&other) {
    bit_stream_ = std::move(other.bit_stream_);
    cur_byte_ = other.cur_byte_;
    bits_taken_ = other.bits_taken_;
}

IBitStream &IBitStream::operator=(IBitStream &&other) {
    std::swap(bit_stream_, other.bit_stream_);
    std::swap(cur_byte_, other.cur_byte_);
    std::swap(bits_taken_, other.bits_taken_);
    other.bit_stream_.Close();
    return *this;
}

void IBitStream::Open(std::string_view working_dir, std::string_view filename) {
    Close();
    std::string dir = std::string(working_dir);
    dir += filename;
    bit_stream_.Open(dir, std::ios::in | std::ios::binary);
    bits_taken_ = CHAR_SIZE;
    cur_byte_ = 0;
}

void IBitStream::Close() {
    if (!bit_stream_.IsOpen()) {
        return;
    }
    bit_stream_.Close();
}

CharType IBitStream::ReadBits(BitSize bit_cnt) {
    CharType result = 0;
    BitSize suffix = bit_cnt;
    while (suffix != 0) {
        if (bits_taken_ == CHAR_SIZE) {
            bits_taken_ = 0;
            cur_byte_ = bit_stream_.Read();
        }
        BitSize read_cnt = std::min(CHAR_SIZE - bits_taken_, suffix);
        CharType bits = BitStream::BitSubStr(cur_byte_, CHAR_SIZE - bits_taken_ - read_cnt, CHAR_SIZE - bits_taken_);
        suffix -= read_cnt;
        result |= bits << (suffix);
        bits_taken_ += read_cnt;
    }
    return result;
}

bool IBitStream::Finished() {
    return bit_stream_.Finished() && (bits_taken_ == 0 || bits_taken_ == CHAR_SIZE);
}
IBitStream::~IBitStream() {
    IBitStream::Close();
}
