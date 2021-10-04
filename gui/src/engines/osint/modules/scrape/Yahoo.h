#ifndef YAHOO_H
#define YAHOO_H

#include "src/engines/osint/modules/AbstractOsintModule.h"
#include "gumbo-parser/src/gumbo.h"

class Yahoo: public AbstractOsintModule{

    public:
        Yahoo(QString target = nullptr, QObject *parent = nullptr);
        ~Yahoo() override;

    public slots:
        void start() override;
        void replyFinished(QNetworkReply *) override;

    private:
        int m_page = 0;
        int activeRequests = 0;
        void getLinks(GumboNode *node);
        GumboNode *getBody(GumboNode *node);
};
#endif // YAHOO_H