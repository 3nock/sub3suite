/*
 Copyright 2020-2022 Enock Nicholaus <3nock@protonmail.com>. All rights reserved.
 Use of this source code is governed by GPL-3.0 LICENSE that can be found in the LICENSE file.

 @brief :
*/

#include "SSLTool.h"
#include "ui_SSLTool.h"


void SSLTool::on_buttonAction_clicked(){
    /* check if there are results available else dont show the context menu */
    if(proxyModel->rowCount() < 1)
        return;

    /* getting the position of the action button to place the context menu and
       showing the context menu right by the side of the action button... */
    QPoint pos = ui->buttonAction->mapToGlobal(QPoint(0,0));
    pos = QPoint(pos.x()+60, pos.y());

    /* creating the context menu... */
    QMenu menu(this);

    /* adding actions */
    menu.addAction(tr("Clear"), this, [=](){this->clearResults();})->setIcon(QIcon(":/img/res/icons/delete.png"));
    if(ui->comboBoxOutput->currentIndex() == 2){
        menu.addAction(tr("Exapand"), this, [=](){ui->treeViewResults->expandAll();})->setIcon(QIcon(":/img/res/icons/expand.png"));
        menu.addAction(tr("Collapse"), this, [=](){ui->treeViewResults->collapseAll();})->setIcon(QIcon(":/img/res/icons/collapse.png"));
    }
    menu.addSeparator();
    menu.addAction(tr("Save"), this, [=](){this->saveResults();})->setIcon(QIcon(":/img/res/icons/save.png"));
    menu.addAction(tr("Copy"), this, [=](){this->copyResults();})->setIcon(QIcon(":/img/res/icons/copy.png"));
    menu.addSeparator();

    switch (ui->comboBoxOutput->currentIndex()) {
    case 0: // subdomain
        menu.addAction(tr("Send To Project"), this, [=](){this->sendToProject();})->setIcon(QIcon(":/img/res/icons/project.png"));
        menu.addSeparator();
        menu.addAction(tr("Send Hostname to OSINT"), this, [=](){this->sendToEngine(TOOL::OSINT, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to RAW"), this, [=](){this->sendToEngine(TOOL::RAW, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to BRUTE"), this, [=](){this->sendToEngine(TOOL::BRUTE, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to HOST"), this, [=](){this->sendToEngine(TOOL::HOST, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to DNS"), this, [=](){this->sendToEngine(TOOL::DNS, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to SSL"), this, [=](){this->sendToEngine(TOOL::SSL, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to URL"), this, [=](){this->sendToEngine(TOOL::URL, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        break;
    case 1: // cert hash
        menu.addAction(tr("Send To Project"), this, [=](){this->sendToProject();})->setIcon(QIcon(":/img/res/icons/project.png"));
        menu.addSeparator();
        menu.addAction(tr("Send SSL to OSINT"), this, [=](){this->sendToEngine(TOOL::OSINT, RESULT_TYPE::CERT_ID);})->setIcon(QIcon(":/img/res/icons/hash.png"));
        menu.addAction(tr("Send SSL to RAW"), this, [=](){this->sendToEngine(TOOL::RAW, RESULT_TYPE::CERT_ID);})->setIcon(QIcon(":/img/res/icons/hash.png"));
        menu.addSeparator();
        menu.addAction(tr("Send SSL to SSL-Enum"), this, [=](){this->sendToEnum(ENUMERATOR::SSL);})->setIcon(QIcon(":/img/res/icons/hash.png"));
        break;
    case 2: // ssl
        menu.addAction(tr("Send To Project"), this, [=](){this->sendToProject();})->setIcon(QIcon(":/img/res/icons/project.png"));
        break;
    }

    /* showing the context menu... */
    menu.exec(pos);
}

void SSLTool::on_treeViewResults_customContextMenuRequested(const QPoint &pos){
    Q_UNUSED(pos);

    /* check if user right clicked on items else dont show the context menu... */
    if(!ui->treeViewResults->selectionModel()->isSelected(ui->treeViewResults->currentIndex()))
        return;

    /* getting the selected items... */
    selectionModel = ui->treeViewResults->selectionModel();

    /* creating the context menu... */
    QMenu menu(this);

    /* adding actions */
    menu.addAction(tr("Remove"), this, [=](){this->clearResults();})->setIcon(QIcon(":/img/res/icons/delete.png"));
    if(ui->comboBoxOutput->currentIndex() == 0)
        menu.addAction(tr("Open in Browser"), this, [=](){this->openInBrowser();})->setIcon(QIcon(":/img/res/icons/browser.png"));
    menu.addSeparator();
    menu.addAction(tr("Save"), this, [=](){this->saveSelectedResults();})->setIcon(QIcon(":/img/res/icons/save.png"));
    menu.addAction(tr("Copy"), this, [=](){this->copySelectedResults();})->setIcon(QIcon(":/img/res/icons/copy.png"));
    menu.addSeparator();

    switch (ui->comboBoxOutput->currentIndex()) {
    case 0: // subdomain
        menu.addAction(tr("Send To Project"), this, [=](){this->sendSelectedToProject();})->setIcon(QIcon(":/img/res/icons/project.png"));
        menu.addSeparator();
        menu.addAction(tr("Send Hostname to OSINT"), this, [=](){this->sendSelectedToEngine(TOOL::OSINT, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to RAW"), this, [=](){this->sendSelectedToEngine(TOOL::RAW, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to BRUTE"), this, [=](){this->sendSelectedToEngine(TOOL::BRUTE, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to HOST"), this, [=](){this->sendSelectedToEngine(TOOL::HOST, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to DNS"), this, [=](){this->sendSelectedToEngine(TOOL::DNS, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to SSL"), this, [=](){this->sendSelectedToEngine(TOOL::SSL, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to URL"), this, [=](){this->sendSelectedToEngine(TOOL::URL, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        break;
    case 1: // cert hash
        menu.addAction(tr("Send To Project"), this, [=](){this->sendSelectedToProject();})->setIcon(QIcon(":/img/res/icons/project.png"));
        menu.addSeparator();
        menu.addAction(tr("Send SSL to OSINT"), this, [=](){this->sendSelectedToEngine(TOOL::OSINT, RESULT_TYPE::CERT_ID);})->setIcon(QIcon(":/img/res/icons/hash.png"));
        menu.addAction(tr("Send SSL to RAW"), this, [=](){this->sendSelectedToEngine(TOOL::RAW, RESULT_TYPE::CERT_ID);})->setIcon(QIcon(":/img/res/icons/hash.png"));
        menu.addSeparator();
        menu.addAction(tr("Send SSL to RAW"), this, [=](){this->sendSelectedToEnum(ENUMERATOR::SSL);})->setIcon(QIcon(":/img/res/icons/hash.png"));
        break;
    case 2: // ssl
        menu.addAction(tr("Send To Project"), this, [=](){this->sendSelectedToProject();})->setIcon(QIcon(":/img/res/icons/project.png"));
        break;
    }

    /* showing the context menu... */
    menu.exec(QCursor::pos());
}
