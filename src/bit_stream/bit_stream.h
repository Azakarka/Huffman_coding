#ifndef CPP_PILOT_HSE_BIT_STREAM_H
#define CPP_PILOT_HSE_BIT_STREAM_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include <string_view>
#include <memory>

using BitSize = size_t;
using CharType = uint16_t;
using Char = uint8_t;
using Vec = std::vector<CharType>;
const static BitSize CHAR_SIZE = 8;

class StreamBuffer {
    const size_t data_size_ = (1 << 12);
    std::unique_ptr<Char[]> data_;
    std::fstream& stream_;
    size_t bytes_used_ = 0;
    size_t processed_cnt_ = 0;
    size_t cur_cnt_ = 0;
    bool started_ = false;

    void ExtractFromOther(StreamBuffer&& other);

public:
    explicit StreamBuffer(std::fstream& stream);
    StreamBuffer(const StreamBuffer& other) = delete;
    StreamBuffer(StreamBuffer&& other);
    StreamBuffer operator=(const StreamBuffer& other) = delete;
    StreamBuffer& operator=(StreamBuffer&& other);
    void Open();
    void Close();
    bool Finished();
    Char ReadByte();
    void ReadData();
    void WriteByte(Char byte);
    void Flush();
};

class BitStream {
public:
    BitStream();
    BitStream(BitStream&& other);
    BitStream operator=(const BitStream& other) = delete;
    BitStream& operator=(BitStream&& other);
    void Open(std::string_view path, std::ios_base::openmode mode, bool little_endian = true);
    void ExceptionHandler();
    Char Read();
    void Write(Char byte);
    bool IsOpen();
    bool Finished();
    void Close();
    virtual ~BitStream();

    static CharType BitSubStr(CharType byte, BitSize l, BitSize r);
    static CharType ChangeEndian(CharType c);

private:
    void ExtractFromOther(BitStream&& other);
    std::fstream stream_;
    StreamBuffer buffer_;
    bool little_endian_;
};

class IBitStream {
    BitStream bit_stream_;

    Char cur_byte_ = 0;
    BitSize bits_taken_ = 0;

public:
    IBitStream(std::string_view working_dir, std::string_view filename);
    IBitStream(const IBitStream& other) = delete;
    IBitStream(IBitStream&& other);
    IBitStream& operator=(const IBitStream& other) = delete;
    IBitStream& operator=(IBitStream&& other);
    void Open(std::string_view working_dir, std::string_view filename);
    void Close();
    CharType ReadBits(BitSize bit_cnt);
    bool Finished();

    ~IBitStream();
};

class OBitStream {
    BitStream bit_stream_;
    Char cur_byte_ = 0;
    BitSize bits_taken_ = 0;

public:
    OBitStream(std::string_view working_dir, std::string_view filename);
    OBitStream(const OBitStream& other) = delete;
    OBitStream(OBitStream&& other);
    OBitStream& operator=(const OBitStream& other) = delete;
    OBitStream& operator=(OBitStream&& other);
    void Open(std::string_view working_dir, std::string_view filename);
    void WriteBits(BitSize bit_cnt, CharType elem);

    void Close();
    ~OBitStream();
};

#endif
