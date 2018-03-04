#ifndef TP_HTTP_SERVER_CONFIG_H
#define TP_HTTP_SERVER_CONFIG_H


#include <QtCore/QString>
#include <QtCore/QVariant>

class Config {
public:
    explicit Config(std::string listenKey = "listen",
                    std::string cpuLimitKey = "cpu_limit",
                    std::string documentRootKey = "document_root");

    Config(const Config &other) = default;

    Config(Config &&other) noexcept;

    bool readConfig(const QString &fileName);

    uint16_t getPort() const;

    void setPort(uint16_t port);

    unsigned int getCpuLimit() const;

    void setCpuLimit(unsigned int cpuLimit);

    const QString &getDocumentRoot() const;

    void setDocumentRoot(const QString &documentRoot);

    bool isValid() const;

    const std::string LISTEN_KEY;
    const std::string CPU_LIMIT_KEY;
    const std::string DOCUMENT_ROOT_KEY;

private:

    bool applyConfigLine(QString string);

    bool isValid_{false};
    uint16_t port_;
    unsigned cpuLimit_;
    QString documentRoot_;
};

#endif //TP_HTTP_SERVER_CONFIG_H
