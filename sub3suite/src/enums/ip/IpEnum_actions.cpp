#include "IpEnum.h"
#include "ui_IpEnum.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QClipboard>


/* TODO:
 *     validate if selected index is ip...
 */

void IpEnum::m_clearResults(){
    /* clear the results... */
    m_model->clear();
    ui->labelResultsCount->clear();
    m_model->setHorizontalHeaderLabels({"    IP", "    Value"});
    m_resultsSet.clear();

    /* clear the progressbar... */
    ui->progressBar->clearMask();
    ui->progressBar->reset();
    ui->progressBar->hide();
}

void IpEnum::m_removeResults(QItemSelectionModel *selectionModel){
    /* loop to delete all selected items */
    foreach(const QModelIndex &proxyIndex, selectionModel->selectedIndexes())
    {
        QModelIndex index = proxyModel->mapToSource(proxyIndex);

        /* remove entire ip */
        if(index.parent() == m_model->invisibleRootItem()->index()){
            m_resultsSet.remove(index.data().toString());
            m_model->removeRow(index.row());
        }
        /* remove a certain row in the ip item */
        else{
            m_model->removeRow(index.row());
        }
    }
    ui->labelResultsCount->setNum(proxyModel->rowCount());
}

void IpEnum::m_saveResults(){
    /* checks... */
    QString filename = QFileDialog::getSaveFileName(this, "Save To File", "./");
    if(filename.isEmpty())
        return;

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    if(!file.isOpen())
        return;

    QJsonArray ip_array;
    for(int i = 0; i != proxyModel->rowCount(); ++i)
    {
        QModelIndex index = proxyModel->mapToSource(proxyModel->index(i ,0));
        s3s_item::IP *ip = static_cast<s3s_item::IP*>(m_model->item(index.row(), index.column()));

        ip_array.append(ip_to_json(ip));
    }

    QJsonDocument document;
    document.setArray(ip_array);

    qDebug() << "Saving IP results to File: " << file.fileName();
    file.write(document.toJson());
    file.close();
}

void IpEnum::m_saveResults(QItemSelectionModel *selection){
    /* checks... */
    QString filename = QFileDialog::getSaveFileName(this, "Save To File", "./");
    if(filename.isEmpty())
        return;

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    if(!file.isOpen())
        return;

    QJsonArray ip_array;
    foreach(const QModelIndex &index, selection->selectedIndexes()){
        QModelIndex model_index = proxyModel->mapToSource(index);
        s3s_item::IP *ip = static_cast<s3s_item::IP*>(m_model->item(model_index.row(), model_index.column()));

        ip_array.append(ip_to_json(ip));
    }

    QJsonDocument document;
    document.setArray(ip_array);

    qDebug() << "Saving IP results to File: " << file.fileName();
    file.write(document.toJson());
    file.close();
}

void IpEnum::m_copyResults(){
    QClipboard *clipboard = QGuiApplication::clipboard();

    QJsonArray ip_array;
    for(int i = 0; i != proxyModel->rowCount(); ++i)
    {
        QModelIndex index = proxyModel->mapToSource(proxyModel->index(i ,0));
        s3s_item::IP *ip = static_cast<s3s_item::IP*>(m_model->item(index.row(), index.column()));

        ip_array.append(ip_to_json(ip));
    }

    QJsonDocument document;
    document.setArray(ip_array);

    qDebug() << "Copying IP results to clipboard...";
    clipboard->setText(document.toJson());
}

void IpEnum::m_copyResults(QItemSelectionModel *selection){
    QClipboard *clipboard = QGuiApplication::clipboard();

    QJsonArray ip_array;
    foreach(const QModelIndex &index, selection->selectedIndexes()){
        QModelIndex model_index = proxyModel->mapToSource(index);
        s3s_item::IP *ip = static_cast<s3s_item::IP*>(m_model->item(model_index.row(), model_index.column()));

        ip_array.append(ip_to_json(ip));
    }

    QJsonDocument document;
    document.setArray(ip_array);

    qDebug() << "Copying IP results to clipboard...";
    clipboard->setText(document.toJson());
}

///
/// send all...
///

void IpEnum::m_sendToProject(){
    for(int i = 0; i != proxyModel->rowCount(); ++i)
    {
        QModelIndex index = proxyModel->mapToSource(proxyModel->index(i ,0));
        s3s_item::IP *item = static_cast<s3s_item::IP*>(m_model->item(index.row(), index.column()));
        project->addEnumIP(ip_to_struct(item));
    }
}

///
/// send selected...
///

void IpEnum::m_sendToProject(QItemSelectionModel *selection){
    foreach(const QModelIndex &index, selection->selectedIndexes()){
        QModelIndex model_index = proxyModel->mapToSource(index);
        s3s_item::IP *ip = static_cast<s3s_item::IP*>(m_model->item(model_index.row(), model_index.column()));

        project->addEnumIP(ip_to_struct(ip));
    }
}

void IpEnum::onReceiveTargets(QString target, RESULT_TYPE resultType){
    if(resultType == RESULT_TYPE::IP)
        ui->targets->add(target);

    /* set multiple targets checkbox checked */
    ui->checkBoxMultipleTargets->setChecked(true);
}
