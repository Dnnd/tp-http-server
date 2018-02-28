#include "IOBuffer.h"
#include <QDebug>
#include <iostream>

const char *IOBuffer::readPos() const {
    return buffer_.data() + readPos_;
}

char *IOBuffer::writePos() {
    return buffer_.data() + writePos_;
}

std::pair<std::size_t, std::size_t> IOBuffer::totalBytesAvailableToRead() const {
    if (isEmpty) {
        return {0, 0};
    }

    if (readPos_ <= writePos_) {
        return {writePos_ - readPos_, 0};
    } else {
        return {buffer_.size() - readPos_, writePos_};
    }
}

std::size_t IOBuffer::bytesAvailableToReadAtOnce() const {
    if (isEmpty) {
        return 0;
    }
    if (readPos_ <= writePos_) {
        return writePos_ - readPos_;
    } else {
        return buffer_.size() - readPos_;
    }
}

void IOBuffer::advanceReadPos(qint64 bytes) {
    readPos_ = (readPos_ + bytes) % buffer_.size();
    isEmpty = writePos_ == readPos_;
    if (isEmpty) {
        readPos_ = 0;
        writePos_ = 0;
    }
}

std::size_t IOBuffer::bytesAvailableToWriteAtOnce() const {
    if (isEmpty) {
        return buffer_.size();
    }
    if (writePos_ < readPos_) {
        return readPos_ - writePos_;
    } else {
        return buffer_.size() - writePos_;
    }
}

std::pair<std::size_t, std::size_t> IOBuffer::totalBytesAvailableToWrite() const {
    if (isEmpty) {
        return {buffer_.size(), 0};
    }

    if (writePos_ + 1 <= readPos_) {
        return {readPos_ - (writePos_ + 1), 0};
    } else {
        return {buffer_.size() - writePos_, (readPos_ == 0) ? 0 : readPos_ - 1};
    }
}

void IOBuffer::advanceWritePos(qint64 bytes) {
    writePos_ = (writePos_ + bytes) % buffer_.size();
    isEmpty = writePos_ == readPos_;
}

IOBuffer::IOBuffer(std::size_t bufferSize)
  : buffer_{} {
        buffer_.resize(bufferSize);
}

qint64 IOBuffer::writeToBuffer(QIODevice *source, qint64 bytes) {
    if (bytes > buffer_.size()) {
        return -1;
    }

    auto [bytesToEnd, bytesFromBegin] = totalBytesAvailableToWrite();

    if (bytes <= bytesToEnd) {
        auto readBytes = source->read(writePos(), bytes);
        advanceWritePos(readBytes);
        return readBytes;
    } else if (bytes <= bytesToEnd + bytesFromBegin) {
        source->read(writePos(), bytesToEnd);
        advanceWritePos(bytesToEnd);
        source->read(writePos(), bytes - bytesToEnd);
        advanceWritePos(bytes - bytesToEnd);
        return bytes;
    } else {
        source->read(writePos(), bytesToEnd);
        advanceWritePos(bytesToEnd);
        source->read(writePos(), bytesFromBegin);
        advanceWritePos(bytesFromBegin);
        return bytesToEnd + bytesFromBegin;
    }
}

qint64 IOBuffer::readFromBuffer(QIODevice *dest, qint64 bytes) {

    auto [bytesToEnd, bytesFromBegin] = totalBytesAvailableToRead();

    if (bytes <= bytesToEnd) {
        auto written = dest->write(readPos(), bytes);
        advanceReadPos(written);
        return written;
    } else if (bytes <= bytesToEnd + bytesFromBegin) {
        auto writtenToEnd = dest->write(readPos(), bytesToEnd);
        advanceReadPos(writtenToEnd);
        if (writtenToEnd < bytesToEnd) {
            return writtenToEnd;
        }
        auto writtenFromBegin = dest->write(readPos(), bytes - bytesToEnd);
        advanceReadPos(writtenFromBegin);
        return writtenFromBegin + writtenToEnd;
    } else {
        auto writtenToEnd = dest->write(readPos(), bytesToEnd);

        advanceReadPos(writtenToEnd);
        if (writtenToEnd < bytesToEnd) {
            return writtenToEnd;
        }
        auto writtenFromBegin = dest->write(readPos(), bytesFromBegin);
        advanceReadPos(writtenFromBegin);
        return writtenFromBegin + writtenToEnd;
    }
}

std::size_t IOBuffer::size() const {
    return buffer_.size();
}
