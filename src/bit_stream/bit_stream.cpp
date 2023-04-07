#include "bit_stream.h"

BitStream::BitStream() : stream_(std::fstream()), buffer_(stream_) {
}

void BitStream::ExtractFromOther(BitStream&& other) {
    std::swap(stream_, other.stream_);
    std::swap(buffer_, other.buffer_);
    std::swap(little_endian_, other.little_endian_);
    other.Close();
}

BitStream::BitStream(BitStream&& other) : buffer_(std::move(other.buffer_)) {
    ExtractFromOther(std::move(other));
}

BitStream& BitStream::operator=(BitStream&& other) {
    ExtractFromOther(std::move(other));
    return *this;
}

void BitStream::ExceptionHandler() {
    if (!stream_.is_open()) {
        throw std::ios_base::failure("operations over closed file");
    }
    if (stream_.bad()) {
        throw std::ios_base::failure("operations over bad file");
    }
}
void BitStream::Open(std::string_view path, std::ios_base::openmode mode, bool little_endian) {
    little_endian_ = little_endian;
    stream_.open(&path[0], mode);
    if (!stream_.good()) {
        throw std::ios_base::failure("couldn't open file");
    }
    buffer_.Open();
    if (mode & std::ios_base::in) {
        buffer_.ReadData();
    }
}

Char BitStream::Read() {
    if (!stream_.good()) {
        ExceptionHandler();
    }
    Char byte = buffer_.ReadByte();
    if (!little_endian_) {
        byte = ChangeEndian(byte);
    }
    return byte;
}

void BitStream::Write(Char byte) {
    if (!stream_.good()) {
        ExceptionHandler();
    }
    if (!little_endian_) {
        byte = ChangeEndian(byte);
    }
    buffer_.WriteByte(byte);
}

CharType BitStream::BitSubStr(CharType byte, BitSize l, BitSize r) {
    CharType substr = byte & ((1 << r) - (1 << l));
    return substr >> l;
}

CharType BitStream::ChangeEndian(CharType c) {
    CharType b = 0;
    for (BitSize i = 0; i < CHAR_SIZE; ++i) {
        bool bit = c & (1 << (CHAR_SIZE - i - 1));
        b |= (bit << i);
    }
    return b;
}

bool BitStream::IsOpen() {
    return stream_.is_open();
}

bool BitStream::Finished() {
    return buffer_.Finished();
}

void BitStream::Close() {
    if (!stream_.is_open()) {
        return;
    }
    buffer_.Close();
    stream_.close();
}

BitStream::~BitStream() {
    BitStream::Close();
}
