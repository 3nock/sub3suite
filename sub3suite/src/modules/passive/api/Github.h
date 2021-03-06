#ifndef GITHUB_H
#define GITHUB_H

#include "../AbstractOsintModule.h"


namespace ModuleInfo {
struct Github{
    QString name = OSINT_MODULE_GITHUB;
    QString url = "https://github.com/";
    QString url_apiDoc = "";
    QString summary = "Github API";
    QMap<QString, QStringList> flags = {{"code",
                                         {PLACEHOLDERTEXT_DOMAIN, ""}}};

    QMap<int, QList<int>> input_output = {{IN_DOMAIN,
                                           {OUT_SUBDOMAIN}}};
};
}

class Github: public AbstractOsintModule{

    public:
        explicit Github(ScanArgs args);
        ~Github() override;

    public slots:
        void start() override;
        void replyFinishedSubdomain(QNetworkReply *) override;

    private:
        QString m_key;
};
#endif // GITHUB_H
