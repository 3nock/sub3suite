#include "SiteDossier.h"

/*
 * get different info for different things...
 *
 * uses captcha...
 *
 */
SiteDossier::SiteDossier(ScanArgs *args):
    AbstractOsintModule(args)
{
    manager = new MyNetworkAccessManager(this);
    connect(manager, &MyNetworkAccessManager::finished, this, &SiteDossier::replyFinished);
}
SiteDossier::~SiteDossier(){
    delete manager;
}

void SiteDossier::start(){
    QNetworkRequest request;
    QUrl url("http://www.sitedossier.com/parentdomain/"+args->target+"/"+QString::number(m_page));
    request.setUrl(url);
    manager->get(request);
}

void SiteDossier::replyFinished(QNetworkReply *reply){
    if(reply->error() == QNetworkReply::NoError)
    {
        GumboOutput *output = gumbo_parse(reply->readAll());
        GumboNode *body = getBody(output->root);
        getSubdomains(body);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
    }
    else
    {
        emit errorLog(reply->errorString());
    }
    reply->deleteLater();
    emit quitThread();
}

///
/// next page implementation not yet...
///
void SiteDossier::getSubdomains(GumboNode *node){
    if(node->type != GUMBO_NODE_ELEMENT)
        return;

    if(node->v.element.tag == GUMBO_TAG_LI && node->v.element.children.length > 0)
    {
        for(unsigned int i = 0; i < node->v.element.children.length; i++){
            GumboNode *tiChild = static_cast<GumboNode*>(node->v.element.children.data[i]);
            if(tiChild->type == GUMBO_NODE_ELEMENT && tiChild->v.element.tag == GUMBO_TAG_A){
                GumboNode *link = static_cast<GumboNode*>(tiChild->v.element.children.data[0]);
                if(link->type == GUMBO_NODE_TEXT){
                    QString item = QString::fromUtf8(link->v.text.text);
                    item = item.remove(0, 7).remove("/");
                    emit subdomain(item);
                }
            }
        }
    }

    GumboVector *children = &node->v.element.children;
    for(unsigned int i = 0; i < children->length; i++)
        getSubdomains(static_cast<GumboNode*>(children->data[i]));
    return;
}

GumboNode *SiteDossier::getBody(GumboNode *node){
    for(unsigned int i = 0; i < node->v.element.children.length; i++){
        GumboNode *child = static_cast<GumboNode*>(node->v.element.children.data[i]);
        if(child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == GUMBO_TAG_BODY)
            return child;
    }
    return nullptr;
}