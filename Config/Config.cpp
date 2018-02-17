#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>
#include "Config.h"
#include <QDebug>

uint16_t Config::getPort() const {
    return port_;
}

void Config::setPort(uint16_t port) {
    port_ = port;
}

unsigned int Config::getCpuLimit() const {
    return cpuLimit_;
}

void Config::setCpuLimit(unsigned int cpuLimit) {
    cpuLimit_ = cpuLimit;
}

const QString &Config::getDocumentRoot() const {
    return documentRoot_;
}

void Config::setDocumentRoot(const QString &documentRoot) {
    documentRoot_ = documentRoot;
}

bool Config::applyConfigLine(QString configLine) {
    QStringList statements{configLine.split(" ")};
    if (statements.length() != 2) {
        return false;
    }

    auto key = statements.at(0).toStdString();
    bool ok = true;
    if (key == LISTEN_KEY) {
        auto listen = static_cast<uint16_t>(statements.at(1).toUInt(&ok));
        if (!ok) {
            return false;
        } else {
            setPort(listen);
            return true;
        }
    } else if (key == DOCUMENT_ROOT_KEY) {
        QUrl documentRoot{statements.at(1)};
        if (!documentRoot.isValid()) {
            return false;
        }
        setDocumentRoot(statements.at(1));
        return true;
    } else if (key == CPU_LIMIT_KEY) {
        unsigned cpuLimit = statements.at(1).toUInt(&ok);
        if (!ok) {
            return false;
        } else {
            setCpuLimit(cpuLimit);
            return true;
        }
    }
    return false;
}

bool Config::readConfig(const QString &fileName) {
    QFile configFile{fileName};
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in{&configFile};

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!applyConfigLine(line)) {
            isValid_ = false;
            return false;
        }
    }
    fileName_ = fileName;
    isValid_ = true;
    return true;
}

Config::Config(std::string listenKey, std::string cpuLimitKey, std::string documentRootKey)
        : LISTEN_KEY{std::move(listenKey)},
          CPU_LIMIT_KEY{std::move(cpuLimitKey)},
          DOCUMENT_ROOT_KEY{std::move(documentRootKey)} {

}

bool Config::isValid() const {
    return isValid_;
}





