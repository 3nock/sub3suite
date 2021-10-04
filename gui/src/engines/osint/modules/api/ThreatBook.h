#ifndef THREATBOOK_H
#define THREATBOOK_H

#include "src/engines/osint/modules/AbstractOsintModule.h"

class ThreatBook: public AbstractOsintModule{

    public:
        ThreatBook(QString target = nullptr, QObject *parent = nullptr);
        ~ThreatBook() override;

    public slots:
        void start() override;
        void replyFinished(QNetworkReply *) override;

    private:
        QString m_key = nullptr;
};

#endif // THREATBOOK_H