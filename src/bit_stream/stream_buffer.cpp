#include "bit_stream.h"

StreamBuffer::StreamBuffer(std::fstream &stream) : stream_(stream) {
}

void StreamBuffer::ExtractFromOther(StreamBuffer &&other) {
    std::swap(bytes_used_, other.bytes_used_);
    std::swap(processed_cnt_, other.processed_cnt_);
    std::swap(cur_cnt_, other.cur_cnt_);
    std::swap(started_, other.started_);
    std::swap(data_, other.data_);
    other.Close();
}

StreamBuffer::StreamBuffer(StreamBuffer &&other) : stream_(other.stream_) {
    ExtractFromOther(std::move(other));
}

StreamBuffer &StreamBuffer::operator=(StreamBuffer &&other) {
    std::swap(stream_, other.stream_);
    ExtractFromOther(std::move(other));
    return *this;
}

void StreamBuffer::Open() {
    data_ = std::unique_ptr<Char[]>(new Char[data_size_]);
}
void StreamBuffer::Close() {
    Flush();
    bytes_used_ = 0;
    processed_cnt_ = 0;
    cur_cnt_ = 0;
    started_ = false;
    data_.reset();
}

Char StreamBuffer::ReadByte() {
    if (cur_cnt_ >= processed_cnt_ && stream_.eof()) {
        throw std::ios::failure("reading after end of file");
    }
    Char byte = data_.get()[bytes_used_++];
    ++cur_cnt_;
    if (cur_cnt_ == processed_cnt_) {
        ReadData();
    }
    return byte;
}

void StreamBuffer::ReadData() {
    started_ = true;
    stream_.read(reinterpret_cast<char *>(data_.get()), data_size_);
    processed_cnt_ += stream_.gcount();
    bytes_used_ = 0;
}

void StreamBuffer::Flush() {
    stream_.write(reinterpret_cast<const char *>(data_.get()), bytes_used_);
    bytes_used_ = 0;
}

bool StreamBuffer::Finished() {
    return cur_cnt_ >= processed_cnt_ && stream_.eof();
}

void StreamBuffer::WriteByte(Char byte) {
    if (bytes_used_ == data_size_) {
        Flush();
    }
    ++cur_cnt_;
    data_.get()[bytes_used_] = byte;
    ++bytes_used_;
}
