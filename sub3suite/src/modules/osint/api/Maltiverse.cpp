#include "Maltiverse.h"
#include "src/utils/Config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define HOSTNAME 0
#define IPV4 1
#define URL 2

/*
 * 100 requests per day...
 *
 * ratelimit-consumed: 1
 * ratelimit-limit: 20
 */
Maltiverse::Maltiverse(ScanArgs args): AbstractOsintModule(args)
{
    manager = new NetworkAccessManager(this);
    log.moduleName = "Maltiverse";

    if(args.outputRaw)
        connect(manager, &NetworkAccessManager::finished, this, &Maltiverse::replyFinishedRawJson);
    if(args.outputEmail)
        connect(manager, &NetworkAccessManager::finished, this, &Maltiverse::replyFinishedEmail);
    if(args.outputAsn)
        connect(manager, &NetworkAccessManager::finished, this, &Maltiverse::replyFinishedAsn);
    if(args.outputIp)
        connect(manager, &NetworkAccessManager::finished, this, &Maltiverse::replyFinishedIp);
}
Maltiverse::~Maltiverse(){
    delete manager;
}

void Maltiverse::start(){
    QNetworkRequest request;

    QUrl url;
    if(args.outputRaw){
        switch (args.rawOption) {
        case HOSTNAME:
            url.setUrl("https://api.maltiverse.com/hostname/"+target);
            break;
        case IPV4:
            url.setUrl("https://api.maltiverse.com/ip/"+target);
            break;
        case URL:
            url.setUrl("https://api.maltiverse.com/url/"+target);
            break;
        }
        request.setUrl(url);
        manager->get(request);
        activeRequests++;
        return;
    }

    if(args.inputDomain){
        url.setUrl("https://api.maltiverse.com/hostname/"+target);
        request.setAttribute(QNetworkRequest::User, HOSTNAME);
        request.setUrl(url);
        manager->get(request);
        activeRequests++;
    }

    if(args.inputIp){
        url.setUrl("https://api.maltiverse.com/urlchecksum/"+target);
        request.setAttribute(QNetworkRequest::User, IPV4);
        request.setUrl(url);
        manager->get(request);
        activeRequests++;
    }
}

void Maltiverse::replyFinishedAsn(QNetworkReply *reply){
    if(reply->error()){
        this->onError(reply);
        return;
    }

    QUERY_TYPE = reply->property(REQUEST_TYPE).toInt();
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    QJsonObject mainObj = document.object();

    if(QUERY_TYPE == HOSTNAME || QUERY_TYPE == IPV4){
        QString asn_name = mainObj["asn_name"].toString();
        //...
        if(!asn_name.isEmpty()){
            QString asnValue = asn_name.split(" ").at(0);
            QString asnName = asn_name.remove(asnValue+" ");
            //...
            emit resultASN(asnValue, asnName);
            log.resultsCount++;
        }
    }


    end(reply);
}

void Maltiverse::replyFinishedIp(QNetworkReply *reply){
    if(reply->error()){
        this->onError(reply);
        return;
    }

    QUERY_TYPE = reply->property(REQUEST_TYPE).toInt();
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    QJsonObject mainObj = document.object();

    if(QUERY_TYPE == HOSTNAME){
        foreach(const QJsonValue &value, mainObj["resolved_ip"].toArray()){
            QString address = value.toObject()["ip_addr"].toString();
            emit resultIp(address);
            log.resultsCount++;
        }
    }

    end(reply);
}

void Maltiverse::replyFinishedEmail(QNetworkReply *reply){
    if(reply->error()){
        this->onError(reply);
        return;
    }

    QUERY_TYPE = reply->property(REQUEST_TYPE).toInt();
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    QJsonArray emails = document.object()["email"].toArray();

    if(QUERY_TYPE == IPV4){
        foreach(const QJsonValue &value, emails){
            emit resultEmail(value.toString());
            log.resultsCount++;
        }
    }

    end(reply);
}
