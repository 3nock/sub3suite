#include "SecurityTrails.h"
#include "src/utils/Config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define ACCOUNT 0
#define ASSOCIATED_DOMAINS 1
#define ASSOCIATED_IPS 2
#define COMPANY_ASSOCIATED_IP 3
#define COMPANY_DETAILS 4
#define DNS_HISTORY 5
#define DOMAIN_DETAILS 6
#define DOMAIN_SUBDOMAIN 7
#define IP_NEIGHBOURS 8
#define IP_WHOIS 9
#define PING 10
#define SSL_CERT 11
#define WHOIS 12
#define WHOIS_HISTORY 13

/*
 * Limited to 2000 results, 50 queries a month for the Free plan and to 10000 for subdomainIp paid subscriptions.
 */

/*
 * many not implemented yet, needs more fixing...
 */
SecurityTrails::SecurityTrails(ScanArgs *args): AbstractOsintModule(args)
{
    manager = new MyNetworkAccessManager(this);
    log.moduleName = "SecurityTrails";

    if(args->raw)
        connect(manager, &MyNetworkAccessManager::finished, this, &SecurityTrails::replyFinishedRaw);
    if(args->outputSubdomain)
        connect(manager, &MyNetworkAccessManager::finished, this, &SecurityTrails::replyFinishedSubdomain);
    if(args->outputIp)
        connect(manager, &MyNetworkAccessManager::finished, this, &SecurityTrails::replyFinishedIp);
    ///
    /// get api key....
    ///
    Config::generalConfig().beginGroup("api-keys");
    m_key = Config::generalConfig().value("securitytrails").toString();
    Config::generalConfig().endGroup();
}
SecurityTrails::~SecurityTrails(){
    delete manager;
}

void SecurityTrails::start(){
    QNetworkRequest request;
    request.setRawHeader("APIKEY", m_key.toUtf8());
    request.setRawHeader("Accept", "application/json");

    QUrl url;
    if(args->raw){
        switch (args->rawOption) {
        case DNS_HISTORY:
            url.setUrl("https://api.securitytrails.com/v1/history/"+args->target+"/dns/any");
            break;
        case DOMAIN_SUBDOMAIN:
            url.setUrl("https://api.securitytrails.com/v1/domain/"+args->target+"/subdomains?children_only=false&include_inactive=true");
            break;
        case WHOIS_HISTORY:
            url.setUrl("https://api.securitytrails.com/v1/history/"+args->target+"/whois");
            break;
        case WHOIS:
            url.setUrl("https://api.securitytrails.com/v1/domain/"+args->target+"/whois");
            break;
        case ASSOCIATED_DOMAINS:
            url.setUrl("https://api.securitytrails.com/v1/domain/"+args->target+"/associated");
            break;
        case ASSOCIATED_IPS:
            url.setUrl("https://api.securitytrails.com/v1/company/"+args->target+"/associated-ips");
            break;
        case DOMAIN_DETAILS:
            url.setUrl("https://api.securitytrails.com/v1/domain/"+args->target);
            break;
        case COMPANY_DETAILS:
            url.setUrl("https://api.securitytrails.com/v1/company/"+args->target);
            break;
        case COMPANY_ASSOCIATED_IP:
            url.setUrl("https://api.securitytrails.com/v1/company/"+args->target+"/associated-ips");
            break;
        case SSL_CERT:
            url.setUrl("https://api.securitytrails.com/v1/domain/"+args->target+"/ssl?include_subdomains=false&status=valid");
            break;
        case IP_NEIGHBOURS:
            url.setUrl("https://api.securitytrails.com/v1/ips/nearby/"+args->target);
            break;
        case IP_WHOIS:
            url.setUrl("https://api.securitytrails.com/v1/ips/"+args->target+"/whois");
            break;
        case PING:
            url.setUrl("https://api.securitytrails.com/v1/ping");
            break;
        case ACCOUNT:
            url.setUrl("https://api.securitytrails.com/v1/account/usage");
        }
        request.setUrl(url);
        manager->get(request);
        activeRequests++;
        return;
    }

    if(args->inputDomain){
        if(args->outputSubdomain){
            url.setUrl("https://api.securitytrails.com/v1/domain/"+args->target+"/subdomains?children_only=false&include_inactive=true");
            request.setAttribute(QNetworkRequest::User, DOMAIN_SUBDOMAIN);
            request.setUrl(url);
            manager->get(request);
            activeRequests++;

            url.setUrl("https://api.securitytrails.com/v1/domain/"+args->target);
            request.setAttribute(QNetworkRequest::User, DOMAIN_DETAILS);
            request.setUrl(url);
            manager->get(request);
            activeRequests++;
        }

        if(args->outputIp){
            url.setUrl("https://api.securitytrails.com/v1/company/"+args->target+"/associated-ips");
            request.setAttribute(QNetworkRequest::User, COMPANY_ASSOCIATED_IP);
            request.setUrl(url);
            manager->get(request);
            activeRequests++;

            url.setUrl("https://api.securitytrails.com/v1/domain/"+args->target);
            request.setAttribute(QNetworkRequest::User, DOMAIN_DETAILS);
            request.setUrl(url);
            manager->get(request);
            activeRequests++;
        }
    }

    if(args->inputIp){
        if(args->outputSubdomain){
            url.setUrl("https://api.securitytrails.com/v1/ips/nearby/"+args->target);
            request.setAttribute(QNetworkRequest::User, IP_NEIGHBOURS);
            request.setUrl(url);
            manager->get(request);
            activeRequests++;
        }
    }
}

void SecurityTrails::replyFinishedSubdomain(QNetworkReply *reply){
    if(reply->error()){
        this->onError(reply);
        return;
    }

    int requestType = reply->property(REQUEST_TYPE).toInt();
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

    if(requestType == DOMAIN_SUBDOMAIN){
        QJsonArray subdomainList = document.object()["subdomains"].toArray();
        foreach(const QJsonValue &value, subdomainList){
            emit subdomain(value.toString().append(".").append(args->target));
            log.resultsCount++;
        }
    }

    if(requestType == DOMAIN_DETAILS){
        QJsonObject txt = document.object()["current_dns"].toObject()["txt"].toObject();
        QJsonObject ns = document.object()["current_dns"].toObject()["ns"].toObject();
        QJsonObject mx = document.object()["current_dns"].toObject()["mx"].toObject();
        //QJsonObject soa = document.object()["current_dns"].toObject()["soa"].toObject();

        if(!txt.isEmpty()){
            foreach(const QJsonValue &value, txt["values"].toArray()){
                emit TXT(value.toObject()["value"].toString());
                log.resultsCount++;
            }
        }
        if(!ns.isEmpty()){
            foreach(const QJsonValue &value, ns["values"].toArray()){
                emit NS(value.toObject()["nameserver"].toString());
                log.resultsCount++;
            }
        }
        if(!mx.isEmpty()){
            foreach(const QJsonValue &value, mx["values"].toArray()){
                emit MX(value.toObject()["hostname"].toString());
                log.resultsCount++;
            }
        }
    }

    /* for getting neighbouring addresses cidr/hostnames... */
    if(requestType == IP_NEIGHBOURS){
        foreach(const QJsonValue &value, document.object()["blocks"].toArray()){
            QString ipCidr = value.toObject()["ip"].toString();
            QJsonArray hostnames = value.toObject()["hostnames"].toArray();

            foreach(const QJsonValue &value, hostnames){
                emit subdomain(value.toString());
                log.resultsCount++;
            }
        }
    }
    end(reply);
}

void SecurityTrails::replyFinishedIp(QNetworkReply *reply){
    if(reply->error()){
        this->onError(reply);
        return;
    }

    int requestType = reply->property(REQUEST_TYPE).toInt();
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

    if(requestType == COMPANY_ASSOCIATED_IP){
        QJsonArray records = document.object()["records"].toArray();
        foreach(const QJsonValue &value, records){
            emit ip(value.toObject()["cidr"].toString());
            //emit ipCDIR(value.toObject()["cidr"].toString());
            log.resultsCount++;
        }
    }

    if(requestType == DOMAIN_DETAILS){
        QJsonObject a = document.object()["current_dns"].toObject()["a"].toObject();
        QJsonObject aaaa = document.object()["current_dns"].toObject()["aaaa"].toObject();

        if(!a.isEmpty()){
            foreach(const QJsonValue &value, a["values"].toArray()){
                emit ipA(value.toObject()["ip"].toString());
                log.resultsCount++;
            }
        }
        if(!aaaa.isEmpty()){
            foreach(const QJsonValue &value, aaaa["values"].toArray()){
                emit ipAAAA(value.toObject()["ip"].toString());
                log.resultsCount++;
            }
        }
    }
    end(reply);
}
