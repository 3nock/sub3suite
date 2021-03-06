/*
 Copyright 2020-2022 Enock Nicholaus <3nock@protonmail.com>. All rights reserved.
 Use of this source code is governed by GPL-3.0 LICENSE that can be found in the LICENSE file.

 @brief :
*/

#include "NSEnum.h"
#include "ui_NSEnum.h"

#include <QMenu>


void NSEnum::on_buttonAction_clicked(){
    /* check if there are results available else dont show the context menu */
    if(proxyModel->rowCount() < 1)
        return;

    /* getting the position of the action button to place the context menu and
       showing the context menu right by the side of the action button... */
    QPoint pos = ui->buttonAction->mapToGlobal(QPoint(0,0));
    pos = QPoint(pos.x()+60, pos.y());

    /* creating the context menu... */
    QMenu menu(this);
    menu.addAction(tr("Clear"), this, [=](){this->clearResults();})->setIcon(QIcon(":/img/res/icons/delete.png"));
    menu.addAction(tr("Expand"), this, [=](){ui->treeViewResults->expandAll();})->setIcon(QIcon(":/img/res/icons/expand.png"));
    menu.addAction(tr("Collapse"), this, [=](){ui->treeViewResults->collapseAll();})->setIcon(QIcon(":/img/res/icons/collapse.png"));
    menu.addSeparator();
    menu.addAction(tr("Save"), this, [=](){this->saveResults();})->setIcon(QIcon(":/img/res/icons/save.png"));
    menu.addAction(tr("Copy"), this, [=](){this->copyResults();})->setIcon(QIcon(":/img/res/icons/copy.png"));
    menu.addSeparator();
    menu.addAction(tr("Send To Project"), this, [=](){this->sendToProject();})->setIcon(QIcon(":/img/res/icons/project.png"));
    menu.addSeparator();
    menu.addAction(tr("Send Hostname to OSINT"), this, [=](){this->sendToEngine(TOOL::OSINT);})->setIcon(QIcon(":/img/res/icons/domain.png"));
    menu.addAction(tr("Send Hostname to RAW"), this, [=](){this->sendToEngine(TOOL::RAW);})->setIcon(QIcon(":/img/res/icons/domain.png"));
    menu.addAction(tr("Send Hostname to BRUTE"), this, [=](){this->sendToEngine(TOOL::BRUTE);})->setIcon(QIcon(":/img/res/icons/domain.png"));
    menu.addAction(tr("Send Hostname to HOST"), this, [=](){this->sendToEngine(TOOL::HOST);})->setIcon(QIcon(":/img/res/icons/domain.png"));
    menu.addAction(tr("Send Hostname to DNS"), this, [=](){this->sendToEngine(TOOL::DNS);})->setIcon(QIcon(":/img/res/icons/domain.png"));
    menu.addAction(tr("Send Hostname to SSL"), this, [=](){this->sendToEngine(TOOL::SSL);})->setIcon(QIcon(":/img/res/icons/domain.png"));

    /* showing the context menu... */
    menu.exec(pos);
}

void NSEnum::on_treeViewResults_customContextMenuRequested(const QPoint &pos){
    Q_UNUSED(pos);

    /* check if user right clicked on items else dont show the context menu... */
    if(!ui->treeViewResults->selectionModel()->isSelected(ui->treeViewResults->currentIndex()))
        return;

    /* getting the selected items... */
    selectionModel = ui->treeViewResults->selectionModel();

    /* creating the context menu... */
    QMenu menu(this);
    menu.addAction(tr("Remove"), this, [=](){this->clearResults();})->setIcon(QIcon(":/img/res/icons/delete.png"));
    menu.addSeparator();
    menu.addAction(tr("Save"), this, [=](){this->saveSelectedResults();})->setIcon(QIcon(":/img/res/icons/save.png"));
    menu.addAction(tr("Copy"), this, [=](){this->copySelectedResults();})->setIcon(QIcon(":/img/res/icons/copy.png"));
    menu.addSeparator();
    if(selectionModel->columnIntersectsSelection(0, selectionModel->currentIndex().parent())){
        menu.addAction(tr("Send To Project"), this, [=](){this->sendSelectedToProject();})->setIcon(QIcon(":/img/res/icons/project.png"));
        menu.addSeparator();
        menu.addAction(tr("Send Hostname to OSINT"), this, [=](){this->sendSelectedToEngine(TOOL::OSINT);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to RAW"), this, [=](){this->sendSelectedToEngine(TOOL::RAW);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to BRUTE"), this, [=](){this->sendSelectedToEngine(TOOL::BRUTE);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to HOST"), this, [=](){this->sendSelectedToEngine(TOOL::HOST);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to DNS"), this, [=](){this->sendSelectedToEngine(TOOL::DNS);})->setIcon(QIcon(":/img/res/icons/domain.png"));
        menu.addAction(tr("Send Hostname to SSL"), this, [=](){this->sendSelectedToEngine(TOOL::SSL);})->setIcon(QIcon(":/img/res/icons/domain.png"));
    }

    /* showing the context menu... */
    menu.exec(QCursor::pos());
}
