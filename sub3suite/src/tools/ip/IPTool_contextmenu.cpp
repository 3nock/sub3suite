/*
 Copyright 2020-2022 Enock Nicholaus <3nock@protonmail.com>. All rights reserved.
 Use of this source code is governed by GPL-3.0 LICENSE that can be found in the LICENSE file.

 @brief :
*/

#include "IPTool.h"
#include "ui_IPTool.h"


void IPTool::on_buttonAction_clicked(){
    /* check if there are results available else dont show the context menu */
    if(proxyModel->rowCount() < 1)
        return;

    /* getting the position of the action button to place the context menu and
       showing the context menu right by the side of the action button... */
    QPoint pos = ui->buttonAction->mapToGlobal(QPoint(0,0));
    pos = QPoint(pos.x()+60, pos.y());

    /* save menu */
    QMenu saveMenu(this);
    saveMenu.setTitle(tr("Save"));
    saveMenu.setIcon(QIcon(":/img/res/icons/save.png"));
    if(ui->comboBoxOption->currentIndex() == 0)
        saveMenu.addAction(tr("Subdomain"), this, [=](){this->saveResults(RESULT_TYPE::SUBDOMAIN);});
    saveMenu.addAction(tr("Ip"), this, [=](){this->saveResults(RESULT_TYPE::IP);});

    /* copy menu */
    QMenu copyMenu(this);
    copyMenu.setTitle(tr("Copy"));
    copyMenu.setIcon(QIcon(":/img/res/icons/copy.png"));
    if(ui->comboBoxOption->currentIndex() == 0)
        copyMenu.addAction(tr("Subdomain"), this, [=](){this->copyResults(RESULT_TYPE::SUBDOMAIN);});
    copyMenu.addAction(tr("Ip"), this, [=](){this->copyResults(RESULT_TYPE::IP);});


    /* main menu */
    QMenu menu(this);

    menu.addAction(tr("Clear"), this, [=](){this->clearResults();})->setIcon(QIcon(":/img/res/icons/delete.png"));
    menu.addSeparator();
    menu.addMenu(&saveMenu);
    menu.addMenu(&copyMenu);
    menu.addSeparator();
    if(ui->comboBoxOption->currentIndex() == 1){
        menu.addAction(tr("Send To Project"), this, [=](){this->sendToProject();})->setIcon(QIcon(":/img/res/icons/project.png"));
        menu.addSeparator();
    }
    menu.addAction(tr("Send IpAddress to OSINT"), this, [=](){this->sendToEngine(TOOL::OSINT, RESULT_TYPE::IP);})->setIcon(QIcon(":/img/res/icons/ip.png"));
    menu.addAction(tr("Send IpAddress to RAW"), this, [=](){this->sendToEngine(TOOL::RAW, RESULT_TYPE::IP);})->setIcon(QIcon(":/img/res/icons/ip.png"));
    menu.addAction(tr("Send IpAddress to IP"), this, [=](){this->sendToEngine(TOOL::IP, RESULT_TYPE::IP);})->setIcon(QIcon(":/img/res/icons/ip.png"));
    menu.addSeparator();
    if(ui->comboBoxOption->currentIndex() == 0){
        menu.addAction(tr("Send Hostname to OSINT"), this, [=](){this->sendToEngine(TOOL::OSINT, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to RAW"), this, [=](){this->sendToEngine(TOOL::RAW, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to BRUTE"), this, [=](){this->sendToEngine(TOOL::BRUTE, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to HOST"), this, [=](){this->sendToEngine(TOOL::HOST, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to DNS"), this, [=](){this->sendToEngine(TOOL::DNS, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to SSL"), this, [=](){this->sendToEngine(TOOL::SSL, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to URL"), this, [=](){this->sendToEngine(TOOL::URL, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addSeparator();
    }
    menu.addAction(tr("Send IpAddress to IP-Enum"), this, [=](){this->sendToEnum(ENUMERATOR::IP);})->setIcon(QIcon(":/img/res/icons/ip.png"));

    /* showing the context menu... */
    menu.exec(pos);
}

void IPTool::on_tableViewResults_customContextMenuRequested(const QPoint &pos){
    Q_UNUSED(pos);

    /* check if user right clicked on items else dont show the context menu... */
    if(!ui->tableViewResults->selectionModel()->isSelected(ui->tableViewResults->currentIndex()))
        return;

    /* getting the selected items... */
    selectionModel = ui->tableViewResults->selectionModel();

    /* main menu */
    QMenu menu(this);

    menu.addAction(tr("Remove"), this, [=](){this->removeResults();})->setIcon(QIcon(":/img/res/icons/delete.png"));
    menu.addAction(tr("Open in Browser"), this, [=](){this->openInBrowser();})->setIcon(QIcon(":/img/res/icons/browser.png"));
    menu.addSeparator();
    menu.addAction(tr("Save"), this, [=](){this->saveSelectedResults();})->setIcon(QIcon(":/img/res/icons/save.png"));
    menu.addAction(tr("Copy"), this, [=](){this->copySelectedResults();})->setIcon(QIcon(":/img/res/icons/copy.png"));
    menu.addSeparator();
    menu.addAction(tr("Send To Project"), this, [=](){this->sendSelectedToProject();})->setIcon(QIcon(":/img/res/icons/project.png"));
    menu.addSeparator();
    if(selectionModel->columnIntersectsSelection(0, selectionModel->currentIndex().parent())){
        menu.addAction(tr("Send IpAddress to OSINT"), this, [=](){this->sendSelectedToEngine(TOOL::OSINT, RESULT_TYPE::IP);})->setIcon(QIcon(":/img/res/icons/ip.png"));
        menu.addAction(tr("Send IpAddress to RAW"), this, [=](){this->sendSelectedToEngine(TOOL::RAW, RESULT_TYPE::IP);})->setIcon(QIcon(":/img/res/icons/ip.png"));
        menu.addAction(tr("Send IpAddress to IP"), this, [=](){this->sendSelectedToEngine(TOOL::IP, RESULT_TYPE::IP);})->setIcon(QIcon(":/img/res/icons/ip.png"));
        menu.addSeparator();
    }
    if(selectionModel->columnIntersectsSelection(0, selectionModel->currentIndex().parent()) &&
            ui->comboBoxOption->currentIndex() == 0){
        menu.addAction(tr("Send Hostname to OSINT"), this, [=](){this->sendSelectedToEngine(TOOL::OSINT, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to RAW"), this, [=](){this->sendSelectedToEngine(TOOL::RAW, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to BRUTE"), this, [=](){this->sendSelectedToEngine(TOOL::BRUTE, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to HOST"), this, [=](){this->sendSelectedToEngine(TOOL::HOST, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to DNS"), this, [=](){this->sendSelectedToEngine(TOOL::DNS, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to SSL"), this, [=](){this->sendSelectedToEngine(TOOL::SSL, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to URL"), this, [=](){this->sendSelectedToEngine(TOOL::URL, RESULT_TYPE::SUBDOMAIN);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addSeparator();
    }
    if(selectionModel->columnIntersectsSelection(0, selectionModel->currentIndex().parent()))
        menu.addAction(tr("Send IpAddress to IP-Enum"), this, [=](){this->sendSelectedToEnum(ENUMERATOR::IP);})->setIcon(QIcon(":/img/res/icons/ip.png"));

    /* showing the context menu... */
    menu.exec(QCursor::pos());
}
