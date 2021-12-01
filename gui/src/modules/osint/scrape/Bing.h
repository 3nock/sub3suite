#ifndef BING_H
#define BING_H

#include "../AbstractOsintModule.h"


class Bing: public AbstractOsintModule{

    public:
        Bing(ScanArgs *args);
        ~Bing() override;

    public slots:
        void start() override;
        void replyFinishedSubdomain(QNetworkReply *) override;

    private:
        int m_page = 0;
        void getLinks(GumboNode *node);
        GumboNode *getBody(GumboNode *node);
};
#endif // BING_H
