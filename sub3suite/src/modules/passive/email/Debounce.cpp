#include "Debounce.h"
#include "src/utils/Config.h"

#define ACCOUNT_BALANCE 0
#define API_USAGE_HISTORY 1
#define DISPOSABLE_EMAIL 2
#define SINGLE_VALIDATION 3


Debounce::Debounce(ScanArgs args): AbstractOsintModule(args)
{
    manager = new s3sNetworkAccessManager(this, args.config->timeout, args.config->setTimeout);
    log.moduleName = OSINT_MODULE_DEBOUNCE;

    if(args.output_Raw)
        connect(manager, &s3sNetworkAccessManager::finished, this, &Debounce::replyFinishedRawJson);

    /* getting api-key */
    m_key = APIKEY.value(OSINT_MODULE_DEBOUNCE).toString();
}
Debounce::~Debounce(){
    delete manager;
}

void Debounce::start(){
    this->checkAPIKey(m_key);

    QNetworkRequest request;
    QUrl url;

    if(args.output_Raw){
        switch(args.raw_query_id){
        case ACCOUNT_BALANCE:
            url.setUrl("https://api.debounce.io/v1/balance?api="+m_key);
            break;
        case API_USAGE_HISTORY:
            url.setUrl("https://api.debounce.io/v1/usage?api="+m_key);
            break;
        case DISPOSABLE_EMAIL:
            url.setUrl("https://disposable.debounce.io?email="+target);
            break;
        case SINGLE_VALIDATION:
            url.setUrl("https://api.debounce.io/v1?api="+m_key+"&email="+target);
            break;
        }
        request.setUrl(url);
        manager->get(request);
    }
}
