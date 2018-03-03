#ifndef TP_HTTP_SERVER_IOBUFFER_H
#define TP_HTTP_SERVER_IOBUFFER_H


#include <cstddef>
#include <vector>
#include <utility>
#include <QtCore/QIODevice>

class IOBuffer {

public:

    explicit IOBuffer(std::size_t bufferSize);

    const char *readPos() const;

    char *writePos();

    std::pair<std::size_t, std::size_t> totalBytesAvailableToWrite() const;

    std::pair<std::size_t, std::size_t> totalBytesAvailableToRead() const;

    void advanceReadPos(qint64 bytes);

    void advanceWritePos(qint64 bytes);

    qint64 writeToBuffer(QIODevice *source, qint64 bytes);

    qint64 readFromBuffer(QIODevice *dest, qint64 bytes);

    std::size_t size() const;

private:
    std::vector<char> buffer_;
    bool isEmpty{true};
    bool isFull{false};

    std::size_t readPos_{0};
    std::size_t writePos_{0};
};


#endif //TP_HTTP_SERVER_IOBUFFER_H
