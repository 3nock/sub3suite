#ifndef LEAKLOOKUP_H
#define LEAKLOOKUP_H

#include "../AbstractOsintModule.h"


namespace ModuleInfo {
struct LeakLookup{
    QString name = OSINT_MODULE_LEAKLOOKUP;
    QString url = "https://leak-lookup.com/";
    QString url_apiDoc = "https://leak-lookup.com/api";
    QString summary = "Leak-Lookup allows you to search across thousands of data breaches to stay on top of credentials that may have been compromised, allowing you to proactively stay on top of the latest data leaks with ease.";

    QMap<QString, QStringList> flags = {{"IP",
                                         {PLACEHOLDERTEXT_IP, "Search Ip-address"}},
                                        {"EMAIL",
                                         {PLACEHOLDERTEXT_EMAIL, "Search Email address"}},
                                        {"USERNAME",
                                         {PLACEHOLDERTEXT_USERNAME, "Search Username"}},
                                        {"DOMAIN",
                                         {PLACEHOLDERTEXT_DOMAIN, "Search Domain"}}};

    QMap<int, QList<int>> input_output = {};
};
}

class LeakLookup: public AbstractOsintModule{

    public:
        explicit LeakLookup(ScanArgs args);
        ~LeakLookup() override;

    public slots:
        void start() override;

    private:
        QString m_key;
};

#endif // LEAKLOOKUP_H
