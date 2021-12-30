#include "Ip.h"
#include "ui_Ip.h"

#include <QDateTime>
#include <QClipboard>
#include "src/dialogs/ActiveConfigDialog.h"


Ip::Ip(QWidget *parent, ProjectDataModel *project) : AbstractEngine(parent, project), ui(new Ui::Ip),
    m_scanConfig(new ip::ScanConfig),
    m_scanArgs(new ip::ScanArgs),
    m_targetListModel(new QStringListModel),
    m_resultModel(new QStandardItemModel),
    m_resultProxyModel(new QSortFilterProxyModel)
{
    ui->setupUi(this);

    /* init... */
    ui->targets->setListName("Target Ip");
    ui->targets->setListModel(m_targetListModel);

    /* result model */
    m_resultModel->setHorizontalHeaderLabels({"Subdomain", "IpAddress"});
    m_resultProxyModel->setSourceModel(m_resultModel);
    m_resultProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_resultProxyModel->setRecursiveFilteringEnabled(true);
    m_resultProxyModel->setFilterKeyColumn(0);
    ui->tableViewResults->setModel(m_resultProxyModel);

    /* ... */
    ui->progressBar->hide();
    ui->buttonStop->setDisabled(true);

    /* placeholder texts */
    ui->lineEditFilter->setPlaceholderText("Enter filter...");

    /* equally seperate the widgets... */
    ui->splitter->setSizes(QList<int>() << static_cast<int>((this->width() * 0.50))
                                        << static_cast<int>((this->width() * 0.50)));

    /* initiate all actions for the context menus */
    this->m_initActions();

    /* syntax higlighting... */
    m_notesSyntaxHighlighter = new NotesSyntaxHighlighter(ui->plainTextEditNotes->document());

    /* ... */
    m_scanArgs->config = m_scanConfig;
}
Ip::~Ip(){
    delete m_scanArgs;
    delete m_scanConfig;
    delete m_targetListModel;
    delete m_resultModel;
    delete m_resultProxyModel;
    delete ui;
}

void Ip::onInfoLog(QString log){
    QString logTime = QDateTime::currentDateTime().toString("hh:mm:ss  ");
    ui->plainTextEditLogs->appendPlainText(logTime.append(log));
}

void Ip::onErrorLog(QString log){
    QString fontedLog;
    fontedLog.append("<font color=\"red\">").append(log).append("</font>");
    QString logTime = QDateTime::currentDateTime().toString("hh:mm:ss  ");
    ui->plainTextEditLogs->appendHtml(logTime.append(fontedLog));
}

void Ip::on_buttonStart_clicked(){
    /*
     checking if subdomainIp requirements are satisfied before scan if not prompt error
     then exit function...
    */
    if(!(m_targetListModel->rowCount() > 0)){
        QMessageBox::warning(this, "Error!", "Please Enter the subdomains Wordlist for Enumeration!");
        return;
    }

    /* disabling and Enabling widgets... */
    ui->buttonStart->setDisabled(true);
    ui->buttonStop->setEnabled(true);
    ui->progressBar->show();

    /* Resetting the scan arguments values... */
    m_scanArgs->targetList = m_targetListModel->stringList();
    m_scanArgs->currentTargetToEnumerate = 0;
    m_scanArgs->progress = 0;
    ui->progressBar->reset();

    /* Getting scan arguments.... */
    ui->progressBar->setMaximum(m_targetListModel->rowCount());

    /* start Ip subdomain enumeration... */
    this->m_startScan();
}

void Ip::on_buttonStop_clicked(){
    emit stopScanThread();
    status->isStopped = true;
}

void Ip::on_buttonConfig_clicked(){
    ActiveConfigDialog *configDialog = new ActiveConfigDialog(this, m_scanConfig);
    configDialog->setAttribute( Qt::WA_DeleteOnClose, true );
    configDialog->show();
}

void Ip::on_comboBoxOption_currentIndexChanged(int index){
    Q_UNUSED(index);
}

