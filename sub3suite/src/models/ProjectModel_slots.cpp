#include "ProjectModel.h"

#include "src/items/SSLItem.h"


///
/// active slots...
///
void ProjectModel::addActiveHost(const s3s_struct::HOST &host){
    if(map_activeHost.contains(host.host))
    {
        s3s_item::HOST *item = map_activeHost.value(host.host);
        if(!host.ipv4.isEmpty())
            item->setValue_ipv4(host.ipv4);
        if(!host.ipv6.isEmpty())
            item->setValue_ipv6(host.ipv6);
        return;
    }

    s3s_item::HOST *item = new s3s_item::HOST;
    item->setValues(host);
    activeHost->appendRow({item, item->ipv4, item->ipv6});
    map_activeHost.insert(host.host, item);
    modified = true;
}

void ProjectModel::addActiveIP(const s3s_struct::IPTool &ip){
    if(map_activeIP.contains(ip.ip))
    {
        s3s_item::IPTool *item = map_activeIP.value(ip.ip);
        item->setValues(ip);
        return;
    }

    s3s_item::IPTool *item = new s3s_item::IPTool;
    item->setValues(ip);
    activeIP->appendRow({item, item->ports});
    map_activeIP.insert(ip.ip, item);
    modified = true;
}

void ProjectModel::addActiveIP(const QString &ip, unsigned short port){
    if(map_activeIP.contains(ip))
    {
        s3s_item::IPTool *item = map_activeIP.value(ip);
        item->addPort(QString::number(port));
        return;
    }

    s3s_item::IPTool *item = new s3s_item::IPTool;
    item->setValues(ip, QString::number(port));
    activeIP->appendRow({item, item->ports});
    map_activeIP.insert(ip, item);
    modified = true;
}

void ProjectModel::addActiveWildcard(const s3s_struct::Wildcard &wildcard){
    if(map_activeWildcard.contains(wildcard.wildcard))
    {
        s3s_item::Wildcard *item = map_activeWildcard.value(wildcard.wildcard);
        item->setValues(wildcard);
        return;
    }

    s3s_item::Wildcard *item = new s3s_item::Wildcard;
    item->setValues(wildcard);
    activeWildcard->appendRow({item, item->ipv4, item->ipv6});
    map_activeWildcard.insert(wildcard.wildcard, item);
    modified = true;
}

void ProjectModel::addActiveSSL(const QString &target, const QSslCertificate &ssl){
    if(map_activeSSL.contains(target)){
        s3s_item::SSL *item = map_activeSSL.value(target);
        item->setValues(target, ssl);
        return;
    }

    s3s_item::SSL *item = new s3s_item::SSL;
    item->setValues(target, ssl);
    activeSSL->appendRow(item);
    map_activeSSL.insert(target, item);
    modified = true;
}

void ProjectModel::addActiveSSL_hash(const QString &hash){
    if(hash.length() == 40)
        activeSSL_sha1->appendRow(new QStandardItem(hash));

    if(hash.length() == 64)
        activeSSL_sha256->appendRow(new QStandardItem(hash));

    modified = true;
}

void ProjectModel::addActiveSSL_sha1(const QString &sha1){
    activeSSL_sha1->appendRow(new QStandardItem(sha1));
    modified = true;
}

void ProjectModel::addActiveSSL_sha256(const QString &sha256){
    activeSSL_sha256->appendRow(new QStandardItem(sha256));
    modified = true;
}

void ProjectModel::addActiveSSL_altNames(const QString &alternative_names){
    activeSSL_altNames->appendRow(new QStandardItem(alternative_names));
    modified = true;
}

void ProjectModel::addActiveDNS(const s3s_struct::DNS &dns){
    if(map_activeDNS.contains(dns.dns)){
        s3s_item::DNS *item = map_activeDNS.value(dns.dns);
        item->setValues(dns);
        this->addActiveRecord(dns);
        return;
    }

    s3s_item::DNS *item = new s3s_item::DNS;
    item->setValues(dns);
    activeDNS->appendRow(item);
    map_activeDNS.insert(dns.dns, item);
    modified = true;
    this->addActiveRecord(dns);
}

void ProjectModel::addActiveRecord(const s3s_struct::DNS &dns){
    foreach(const QString &a, dns.A){
        if(!set_activeA.contains(a)){
            set_activeA.insert(a);
            activeA->appendRow(new QStandardItem(a));
        }
    }
    foreach(const QString &aaaa, dns.AAAA){
        if(!set_activeAAAA.contains(aaaa)){
            set_activeAAAA.insert(aaaa);
            activeAAAA->appendRow(new QStandardItem(aaaa));
        }
    }
    foreach(const QString &ns, dns.NS){
        if(!set_activeNS.contains(ns)){
            set_activeNS.insert(ns);
            activeNS->appendRow(new QStandardItem(ns));
        }
    }
    foreach(const QString &mx, dns.MX){
        if(!set_activeMX.contains(mx)){
            set_activeMX.insert(mx);
            activeMX->appendRow(new QStandardItem(mx));
        }
    }
    foreach(const QString &txt, dns.TXT){
        if(!set_activeTXT.contains(txt)){
            set_activeTXT.insert(txt);
            activeTXT->appendRow(new QStandardItem(txt));
        }
    }
    foreach(const QString &srv, dns.SRV){
        if(!set_activeSRV.contains(srv)){
            set_activeSRV.insert(srv);
            activeSRV->appendRow(new QStandardItem(srv));
        }
    }
    foreach(const QString &cname, dns.CNAME){
        if(!set_activeCNAME.contains(cname)){
            set_activeCNAME.insert(cname);
            activeCNAME->appendRow(new QStandardItem(cname));
        }
    }
}

void ProjectModel::addActiveURL(const s3s_struct::URL &url){
    if(map_activeURL.contains(url.url)){
        s3s_item::URL *item = map_activeURL.value(url.url);
        item->setValues(url);
        return;
    }

    s3s_item::URL *item = new s3s_item::URL;
    item->setValues(url);
    activeURL->appendRow({item, item->status_code, item->title, item->banner, item->content_type});
    map_activeURL.insert(url.url, item);
    modified = true;
}

///
/// passive slots...
///
void ProjectModel::addPassiveSubdomainIp(const QString &subdomain, const QString &ip){
    passiveSubdomainIp->appendRow({new QStandardItem(subdomain), new QStandardItem(ip)});
    modified = true;
}

void ProjectModel::addPassiveSubdomain(const QString &subdomain){
    passiveSubdomain->appendRow(new QStandardItem(subdomain));
    modified = true;
}

void ProjectModel::addPassiveIp(const QString &ip){
    if(ip.contains(":"))
        passiveAAAA->appendRow(new QStandardItem(ip));
    else
        passiveA->appendRow(new QStandardItem(ip));

    modified = true;
}

void ProjectModel::addPassiveA(const QString &ipv4){
    passiveA->appendRow(new QStandardItem(ipv4));
    modified = true;
}

void ProjectModel::addPassiveAAAA(const QString &ipv6){
    passiveAAAA->appendRow(new QStandardItem(ipv6));
    modified = true;
}

void ProjectModel::addPassiveCidr(const QString &cidr){
    passiveCIDR->appendRow(new QStandardItem(cidr));
    modified = true;
}

void ProjectModel::addPassiveNS(const QString &ns){
    passiveNS->appendRow(new QStandardItem(ns));
    modified = true;
}

void ProjectModel::addPassiveMX(const QString &mx){
    passiveMX->appendRow(new QStandardItem(mx));
    modified = true;
}

void ProjectModel::addPassiveTXT(const QString &txt){
    passiveTXT->appendRow(new QStandardItem(txt));
    modified = true;
}

void ProjectModel::addPassiveCNAME(const QString &cname){
    passiveCNAME->appendRow(new QStandardItem(cname));
    modified = true;
}

void ProjectModel::addPassiveEMail(const QString &email){
    passiveEmail->appendRow(new QStandardItem(email));
    modified = true;
}

void ProjectModel::addPassiveUrl(const QString &url){
    passiveURL->appendRow(new QStandardItem(url));
    modified = true;
}

void ProjectModel::addPassiveAsn(const QString &asn, const QString &name){
    passiveASN->appendRow({new QStandardItem(asn), new QStandardItem(name)});
    modified = true;
}

void ProjectModel::addPassiveSSL(const QString &ssl){
    passiveSSL->appendRow(new QStandardItem(ssl));
    modified = true;
}

///
/// enum slots...
///
void ProjectModel::addEnumASN(const s3s_struct::ASN &asn){
    if(map_enumASN.contains(asn.asn)){
        s3s_item::ASN *item = map_enumASN.value(asn.asn);
        item->setValues(asn);
        return;
    }

    s3s_item::ASN *item = new s3s_item::ASN;
    item->setValues(asn);
    enumASN->appendRow(item);
    map_enumASN.insert(asn.asn, item);
    modified = true;
}

void ProjectModel::addEnumCIDR(const s3s_struct::CIDR &cidr){
    if(map_enumASN.contains(cidr.cidr)){
        s3s_item::CIDR *item = map_enumCIDR.value(cidr.cidr);
        item->setValues(cidr);
        return;
    }

    s3s_item::CIDR *item = new s3s_item::CIDR;
    item->setValues(cidr);
    enumCIDR->appendRow(item);
    map_enumCIDR.insert(cidr.cidr, item);
    modified = true;
}

void ProjectModel::addEnumIP(const s3s_struct::IP &ip){
    if(map_enumASN.contains(ip.ip)){
        s3s_item::IP *item = map_enumIP.value(ip.ip);
        item->addValues(ip);
        return;
    }

    s3s_item::IP *item = new s3s_item::IP;
    item->setValues(ip);
    enumIP->appendRow(item);
    map_enumIP.insert(ip.ip, item);
    modified = true;
}

void ProjectModel::addEnumIP_domains(const s3s_struct::IP &ip){
    if(map_enumASN.contains(ip.ip)){
        s3s_item::IP *item = map_enumIP.value(ip.ip);
        item->addDomains(ip);
        return;
    }

    s3s_item::IP *item = new s3s_item::IP;
    item->addDomains(ip);
    enumIP->appendRow(item);
    map_enumIP.insert(ip.ip, item);
    modified = true;
}

void ProjectModel::addEnumNS(const s3s_struct::NS &ns){
    if(map_enumNS.contains(ns.ns)){
        s3s_item::NS *item = map_enumNS.value(ns.ns);
        item->setValues(ns);
        return;
    }

    s3s_item::NS *item = new s3s_item::NS;
    item->setValues(ns);
    enumNS->appendRow(item);
    map_enumNS.insert(ns.ns, item);
    modified = true;
}

void ProjectModel::addEnumMX(const s3s_struct::MX &mx){
    if(map_enumMX.contains(mx.mx)){
        s3s_item::MX *item = map_enumMX.value(mx.mx);
        item->setValues(mx);
        return;
    }

    s3s_item::MX *item = new s3s_item::MX;
    item->setValues(mx);
    enumMX->appendRow(item);
    map_enumMX.insert(mx.mx, item);
    modified = true;
}

void ProjectModel::addEnumSSL(const QString &target, const QSslCertificate &ssl){
    if(map_enumSSL.contains(target)){
        s3s_item::SSL *item = map_enumSSL.value(target);
        item->setValues(target, ssl);
        return;
    }

    s3s_item::SSL *item = new s3s_item::SSL;
    item->setValues(target, ssl);
    enumSSL->appendRow(item);
    map_enumSSL.insert(target, item);
    modified = true;
}

void ProjectModel::addEnumEmail(const s3s_struct::Email &email){
    if(map_enumEmail.contains(email.email)){
        s3s_item::Email *item = map_enumEmail.value(email.email);
        item->setValues(email);
        return;
    }

    s3s_item::Email *item = new s3s_item::Email;
    item->setValues(email);
    enumEmail->appendRow(item);
    map_enumEmail.insert(email.email, item);
    modified = true;
}

void ProjectModel::addRaw(const s3s_struct::RAW &raw_results){
    s3s_item::RAW *item = new s3s_item::RAW;
    item->setValues(raw_results);
    raw->appendRow(item);
    modified = true;
}
