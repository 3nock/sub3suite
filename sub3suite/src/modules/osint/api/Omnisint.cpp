#include "Omnisint.h"
#include <QJsonDocument>
#include <QJsonArray>

#define ALL 0
#define REVERSE_IP 1
#define SUBDOMAIN 2
#define TLD 3

Omnisint::Omnisint(ScanArgs args): AbstractOsintModule(args)
{
    manager = new s3sNetworkAccessManager(this);
    log.moduleName = "Omnisint";

    if(args.outputRaw)
        connect(manager, &s3sNetworkAccessManager::finished, this, &Omnisint::replyFinishedRawJson);
    if(args.outputSubdomain)
        connect(manager, &s3sNetworkAccessManager::finished, this, &Omnisint::replyFinishedSubdomain);
}
Omnisint::~Omnisint(){
    delete manager;
}

void Omnisint::start(){
    QNetworkRequest request;

    QUrl url;
    if(args.outputRaw){
        switch(args.rawOption){
        case ALL:
            url.setUrl("https://sonar.omnisint.io/all/"+target);
            break;
        case REVERSE_IP:
            url.setUrl("https://sonar.omnisint.io/reverse/"+target);
            break;
        case SUBDOMAIN:
            url.setUrl("https://sonar.omnisint.io/subdomains/"+target);
            break;
        case TLD:
            url.setUrl("https://sonar.omnisint.io/tlds/"+target);
            break;
        }
        request.setUrl(url);
        manager->get(request);
        activeRequests++;
    }

    if(args.inputIp){
        url.setUrl("https://sonar.omnisint.io/reverse/"+target);
        request.setAttribute(QNetworkRequest::User, REVERSE_IP);
        request.setUrl(url);
        manager->get(request);
        activeRequests++;
    }

    if(args.outputSubdomain){
        url.setUrl("https://sonar.omnisint.io/all/"+target);
        request.setAttribute(QNetworkRequest::User, ALL);
        request.setUrl(url);
        manager->get(request);
        activeRequests++;
    }
}

void Omnisint::replyFinishedSubdomain(QNetworkReply *reply){
    if(reply->error()){
        this->onError(reply);
        return;
    }

    QUERY_TYPE = reply->property(REQUEST_TYPE).toInt();
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    QJsonArray subdomains = document.array();

    if(QUERY_TYPE == ALL){
        foreach(const QJsonValue &value, subdomains){
            emit resultSubdomain(value.toString());
            log.resultsCount++;
        }
    }

    if(QUERY_TYPE == REVERSE_IP){
        foreach(const QJsonValue &value, subdomains){
            emit resultSubdomain(value.toString());
            log.resultsCount++;
        }
    }
    end(reply);
}