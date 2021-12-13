#include "Host.h"
#include "src/utils/Config.h"

#define DNS 0
#define FULL 1
#define RELATED 2
#define WEB 3

Host::Host(ScanArgs args): AbstractOsintModule(args)
{
    manager = new NetworkAccessManager(this);
    log.moduleName = "Host";

    if(args.outputRaw)
        connect(manager, &NetworkAccessManager::finished, this, &Host::replyFinishedRawJson);
    ///
    /// getting api key...
    ///
    Config::generalConfig().beginGroup("api-keys");
    m_key = Config::generalConfig().value("host").toString();
    Config::generalConfig().endGroup();
}
Host::~Host(){
    delete manager;
}

void Host::start(){
    QNetworkRequest request;

    QUrl url;
    if(args.outputRaw){
        switch (args.rawOption) {
        case DNS:
            url.setUrl("https://host.io/api/dns/"+target+"?token="+m_key);
            break;
        case FULL:
            url.setUrl("https://host.io/api/full/"+target+"?token="+m_key);
            break;
        case RELATED:
            url.setUrl("https://host.io/api/related/"+target+"?token="+m_key);
            break;
        case WEB:
            url.setUrl("https://host.io/api/web/"+target+"?token="+m_key);
            break;
        }
        request.setUrl(url);
        manager->get(request);
        activeRequests++;
    }
}
