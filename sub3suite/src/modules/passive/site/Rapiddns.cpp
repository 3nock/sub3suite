#include "Rapiddns.h"
#include <QStack>


/*
 * it is one of my fav...
 */
Rapiddns::Rapiddns(ScanArgs args): AbstractOsintModule(args)
{
    manager = new s3sNetworkAccessManager(this, args.config->timeout, args.config->setTimeout);
    log.moduleName = OSINT_MODULE_RAPIDDNS;

    if(args.output_Hostname)
        connect(manager, &s3sNetworkAccessManager::finished, this, &Rapiddns::replyFinishedSubdomain);
    if(args.output_HostnameIP)
        connect(manager, &s3sNetworkAccessManager::finished, this, &Rapiddns::replyFinishedSubdomainIp);
    if(args.output_IP)
        connect(manager, &s3sNetworkAccessManager::finished, this, &Rapiddns::replyFinishedIp);
}
Rapiddns::~Rapiddns(){
    delete manager;
}

void Rapiddns::start(){
    QNetworkRequest request;
    QUrl url;

    if(args.input_Domain){
        url.setUrl("https://rapiddns.io/subdomain/"+target+"?full=1#result");
        request.setUrl(url);
        manager->get(request);
        return;
    }

    if(args.input_IP){
        url.setUrl("https://rapiddns.io/sameip/"+target+"?full=1#result");
        request.setUrl(url);
        manager->get(request);
        return;
    }

    if(args.input_CIDR){
        url.setUrl("https://rapiddns.io/s/"+target+"?full=1#result");
        request.setUrl(url);
        manager->get(request);
        return;
    }
}

void Rapiddns::replyFinishedSubdomain(QNetworkReply *reply){
    if(reply->error()){
        this->onError(reply);
        return;
    }

    QStack<GumboNode*> nodes;
    GumboOutput *output = gumbo_parse(reply->readAll());
    nodes.push(GumboMethods::getBody(output->root));

    GumboNode *node;
    while(!nodes.isEmpty())
    {
        node = nodes.pop();
        if(node->type != GUMBO_NODE_ELEMENT || node->v.element.tag == GUMBO_TAG_SCRIPT)
            continue;

        if(node->v.element.tag == GUMBO_TAG_TBODY && node->v.element.children.length > 0){
            GumboVector tbodyChildren = node->v.element.children;
            for(unsigned int i = 0; i < tbodyChildren.length; i++){
                GumboNode *tr = static_cast<GumboNode*>(tbodyChildren.data[i]);
                if(tr->type == GUMBO_NODE_ELEMENT && tr->v.element.tag == GUMBO_TAG_TR && tr->v.element.children.length > 1)
                {
                    GumboNode *td = static_cast<GumboNode*>(tr->v.element.children.data[3]);
                    GumboNode *hostname = static_cast<GumboNode*>(td->v.element.children.data[0]);
                    emit resultSubdomain(hostname->v.text.text);
                    log.resultsCount++;
                }
            }
            continue;
        }

        GumboVector *children = &node->v.element.children;
        for(unsigned int i = 0; i < children->length; i++)
            nodes.push(static_cast<GumboNode*>(children->data[i]));
    }

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    this->end(reply);
}

void Rapiddns::replyFinishedSubdomainIp(QNetworkReply *reply){
    if(reply->error()){
        this->onError(reply);
        return;
    }

    QStack<GumboNode*> nodes;
    GumboOutput *output = gumbo_parse(reply->readAll());
    nodes.push(GumboMethods::getBody(output->root));

    GumboNode *node;
    while(!nodes.isEmpty())
    {
        node = nodes.pop();
        if(node->type != GUMBO_NODE_ELEMENT || node->v.element.tag == GUMBO_TAG_SCRIPT)
            continue;

        if(node->v.element.tag == GUMBO_TAG_TBODY && node->v.element.children.length > 0){
            GumboVector tbodyChildren = node->v.element.children;
            for(unsigned int i = 0; i < tbodyChildren.length; i++){
                GumboNode *tr = static_cast<GumboNode*>(tbodyChildren.data[i]);
                if(tr->type == GUMBO_NODE_ELEMENT && tr->v.element.tag == GUMBO_TAG_TR && tr->v.element.children.length > 1)
                {
                    GumboNode *td_type = static_cast<GumboNode*>(tr->v.element.children.data[7]);
                    GumboNode *type = static_cast<GumboNode*>(td_type->v.element.children.data[0]);

                    if(QString::fromUtf8(type->v.text.text) == "A" || QString::fromUtf8(type->v.text.text) == "AAAA"){
                        GumboNode *td_domain = static_cast<GumboNode*>(tr->v.element.children.data[3]);
                        GumboNode *domain = static_cast<GumboNode*>(td_domain->v.element.children.data[0]);

                        GumboNode *td_address = static_cast<GumboNode*>(tr->v.element.children.data[5]);
                        GumboNode *a = static_cast<GumboNode*>(td_address->v.element.children.data[0]);
                        GumboNode *address = static_cast<GumboNode*>(a->v.element.children.data[0]);

                        emit resultSubdomainIp(domain->v.text.text, address->v.text.text);
                        log.resultsCount++;
                    }
                }
            }
            continue;
        }

        GumboVector *children = &node->v.element.children;
        for(unsigned int i = 0; i < children->length; i++)
            nodes.push(static_cast<GumboNode*>(children->data[i]));
    }

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    this->end(reply);
}

void Rapiddns::replyFinishedIp(QNetworkReply *reply){
    if(reply->error()){
        this->onError(reply);
        return;
    }

    QStack<GumboNode*> nodes;
    GumboOutput *output = gumbo_parse(reply->readAll());
    nodes.push(GumboMethods::getBody(output->root));

    GumboNode *node;
    while(!nodes.isEmpty())
    {
        node = nodes.pop();
        if(node->type != GUMBO_NODE_ELEMENT || node->v.element.tag == GUMBO_TAG_SCRIPT)
            continue;

        if(node->v.element.tag == GUMBO_TAG_TBODY && node->v.element.children.length > 0){
            GumboVector tbodyChildren = node->v.element.children;
            for(unsigned int i = 0; i < tbodyChildren.length; i++){
                GumboNode *tr = static_cast<GumboNode*>(tbodyChildren.data[i]);
                if(tr->type == GUMBO_NODE_ELEMENT && tr->v.element.tag == GUMBO_TAG_TR && tr->v.element.children.length > 1)
                {
                    GumboNode *td_type = static_cast<GumboNode*>(tr->v.element.children.data[7]);
                    GumboNode *type = static_cast<GumboNode*>(td_type->v.element.children.data[0]);

                    if(QString::fromUtf8(type->v.text.text) == "A" || QString::fromUtf8(type->v.text.text) == "AAAA"){

                        GumboNode *td_address = static_cast<GumboNode*>(tr->v.element.children.data[5]);
                        GumboNode *a = static_cast<GumboNode*>(td_address->v.element.children.data[0]);
                        GumboNode *address = static_cast<GumboNode*>(a->v.element.children.data[0]);

                        emit resultIP(address->v.text.text);
                        log.resultsCount++;
                    }
                }
            }
            continue;
        }

        GumboVector *children = &node->v.element.children;
        for(unsigned int i = 0; i < children->length; i++)
            nodes.push(static_cast<GumboNode*>(children->data[i]));
    }

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    this->end(reply);
}
