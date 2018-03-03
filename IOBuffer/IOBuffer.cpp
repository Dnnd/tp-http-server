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

    if (isFull) {
        return {buffer_.size() - readPos_, writePos_};
    }

    if (readPos_ < writePos_) {
        return {writePos_ - readPos_, 0};
    } else {
        return {buffer_.size() - readPos_, writePos_};
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

std::pair<std::size_t, std::size_t> IOBuffer::totalBytesAvailableToWrite() const {
    if (isEmpty) {
        return {buffer_.size(), 0};
    }
    if (isFull) {
        return {0, 0};
    }
    if (writePos_ < readPos_) {
        return {readPos_ - writePos_, 0};
    } else {
        return {buffer_.size() - writePos_, readPos_};
    }
}

void IOBuffer::advanceWritePos(qint64 bytes) {
    writePos_ = (writePos_ + bytes) % buffer_.size();
    isFull = writePos_ == readPos_;
}

IOBuffer::IOBuffer(std::size_t bufferSize)
        : buffer_(bufferSize) {
}

qint64 IOBuffer::writeToBuffer(QIODevice *source, qint64 bytes) {
    if (bytes > buffer_.size()) {
        return -1;
    }
    if (isFull) {
        return 0;
    }
    auto[bytesToEnd, bytesFromBegin] = totalBytesAvailableToWrite();
    qint64 writtenUntilEnd{0};
    qint64 writtenFromBegin{0};

    if (bytes <= bytesToEnd) {
        writtenUntilEnd = source->read(writePos(), bytes);
        advanceWritePos(writtenUntilEnd);
    } else if (bytes <= bytesToEnd + bytesFromBegin) {
        writtenUntilEnd = source->read(writePos(), bytesToEnd);
        advanceWritePos(writtenUntilEnd);
        if (writtenUntilEnd == bytesToEnd) {
            writtenFromBegin = source->read(writePos(), bytes - bytesToEnd);
            advanceWritePos(writtenFromBegin);
        }
    } else {
        writtenUntilEnd = source->read(writePos(), bytesToEnd);
        advanceWritePos(writtenUntilEnd);
        if (writtenUntilEnd == bytesToEnd) {
            writtenFromBegin = source->read(writePos(), bytesFromBegin);
            advanceWritePos(writtenFromBegin);
        }
    }
    if (isEmpty) {
        isEmpty = (writtenFromBegin + writtenUntilEnd) == 0;
    }
    return writtenFromBegin + writtenUntilEnd;
}

qint64 IOBuffer::readFromBuffer(QIODevice *dest, qint64 bytes) {
    if (isEmpty) {
        return 0;
    }
    auto[bytesToEnd, bytesFromBegin] = totalBytesAvailableToRead();
    qint64 readUntilEnd{0};
    qint64 readFromBegin{0};

    if (bytes <= bytesToEnd) {
        readUntilEnd = dest->write(readPos(), bytes);
        advanceReadPos(readUntilEnd);
    } else if (bytes <= bytesToEnd + bytesFromBegin) {
        readUntilEnd = dest->write(readPos(), bytesToEnd);
        advanceReadPos(readUntilEnd);
        if (readUntilEnd == bytesToEnd) {
            readFromBegin = dest->write(readPos(), bytes - bytesToEnd);
            advanceReadPos(readFromBegin);
        }
    } else {
        readUntilEnd = dest->write(readPos(), bytesToEnd);
        advanceReadPos(readUntilEnd);
        if (readUntilEnd == bytesToEnd) {
            readFromBegin = dest->write(readPos(), bytesFromBegin);
            advanceReadPos(readFromBegin);
        }
    }
    if (isFull) {
        isFull = (readFromBegin + readUntilEnd) == 0;
    }
    return readFromBegin + readUntilEnd;
}

std::size_t IOBuffer::size() const {
    return buffer_.size();
}
