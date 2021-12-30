#include "Brute.h"
#include "ui_Brute.h"

#include <QDateTime>
#include "src/utils/Definitions.h"
#include "src/dialogs/ActiveConfigDialog.h"
#include "src/dialogs/wordlist/WordlistDialog.h"


Brute::Brute(QWidget *parent, ProjectDataModel *project) : AbstractEngine(parent, project), ui(new Ui::Brute),
    m_scanConfig(new brute::ScanConfig),
    m_scanArgs(new brute::ScanArgs),
    m_wordlistModel(new QStringListModel),
    m_targetListModel(new QStringListModel),
    m_resultModelSubdomain(new QStandardItemModel),
    m_resultModelTld(new QStandardItemModel),
    m_resultProxyModel(new QSortFilterProxyModel)
{
    ui->setupUi(this);

    /* wordlist & target models */
    ui->targets->setListName("Targets");
    ui->wordlist->setListName("Wordlist");
    ui->targets->setListModel(m_targetListModel);
    ui->wordlist->setListModel(m_wordlistModel);

    /* results models */
    m_resultModelSubdomain->setHorizontalHeaderLabels({"Subdomain", "IpAddress"});
    m_resultModelTld->setHorizontalHeaderLabels({"TLD", "Ip"});
    /* default is subdomain result model */
    m_resultProxyModel->setSourceModel(m_resultModelSubdomain);
    m_resultProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_resultProxyModel->setRecursiveFilteringEnabled(true);
    m_resultProxyModel->setFilterKeyColumn(0);
    ui->tableViewResults->setModel(m_resultProxyModel);

    /* placeholder texts */
    ui->lineEditTarget->setPlaceholderText(PLACEHOLDERTEXT_DOMAIN);
    ui->lineEditFilter->setPlaceholderText("Enter filter...");

    /* ...*/
    ui->buttonStop->setDisabled(true);
    ui->targets->hide();
    ui->progressBar->hide();

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
Brute::~Brute(){
    delete m_scanConfig;
    delete m_scanArgs;
    delete m_targetListModel;
    delete m_wordlistModel;
    delete m_resultModelSubdomain;
    delete m_resultModelTld;
    delete m_resultProxyModel;
    delete ui;
}

void Brute::onInfoLog(QString log){
    ui->plainTextEditLogs->appendPlainText(log);
}

void Brute::onErrorLog(QString log){
    QString fontedLog("<font color=\"red\">"+log+"</font>");
    ui->plainTextEditLogs->appendHtml(fontedLog);
}

void Brute::on_buttonStart_clicked(){
    /* checks */
    if(!ui->checkBoxMultipleTargets->isChecked() && ui->lineEditTarget->text().isEmpty()){
        QMessageBox::warning(this, "Error!", "Please Enter the Target for Enumeration!");
        return;
    }
    if(ui->checkBoxMultipleTargets->isChecked() && m_targetListModel->rowCount() < 1){
        QMessageBox::warning(this, "Error!", "Please Enter the Targets for Enumeration!");
        return;
    }
    if(m_wordlistModel->rowCount() < 1){
        QMessageBox::warning(this, "Error!", "Please Enter the Wordlist for Enumeration!");
        return;
    }

    /* disabling, enabling & resseting widgets... */
    ui->buttonStart->setDisabled(true);
    ui->buttonStop->setEnabled(true);
    ui->progressBar->show();
    ui->progressBar->reset();

    /* Resetting the scan arguments values */
    m_scanArgs->wordlist = m_wordlistModel->stringList();

    switch (ui->comboBoxOutput->currentIndex()) {
    case brute::OUTPUT::SUBDOMAIN:
        if(ui->checkBoxMultipleTargets->isChecked()){
            foreach(const QString &target, m_wordlistModel->stringList())
                m_scanArgs->targets.enqueue(this->targetFilterSubdomain(target));
        }
        else
            m_scanArgs->targets.enqueue(this->targetFilterSubdomain(ui->lineEditTarget->text()));

        break;
    case brute::OUTPUT::TLD:
        if(ui->checkBoxMultipleTargets->isChecked()){
            foreach(const QString &target, m_wordlistModel->stringList())
                m_scanArgs->targets.enqueue(this->targetFilterTLD(target));
        }
        else
            m_scanArgs->targets.enqueue(this->targetFilterTLD(ui->lineEditTarget->text()));
    }

    ui->progressBar->setMaximum(m_scanArgs->wordlist.length() * m_scanArgs->targets.length());

    /* debug info */
    qDebug() << "Threads: " << m_scanConfig->threads;
    qDebug() << "Timeout: " << m_scanConfig->timeout;
    qDebug() << "Nameserver:" << m_scanConfig->nameservers.at(0);
    qDebug() << "Record Type: " << m_scanConfig->recordType;
    qDebug() << "**************************************";

    /* start scan */
    this->m_startScan();
}

void Brute::on_lineEditTarget_returnPressed(){
    this->on_buttonStart_clicked();
}

void Brute::on_buttonStop_clicked(){
    emit stopScanThread();
    if(status->isPaused)
    {
        m_scanArgs->targets.clear();
        status->isPaused = false;
        status->isStopped = false;
        status->isRunning = false;

        /* enabling and disabling widgets */
        ui->buttonStart->setEnabled(true);
        ui->buttonStop->setDisabled(true);
    }
    status->isStopped = true;
}

void Brute::on_buttonConfig_clicked(){
    ActiveConfigDialog *configDialog = new ActiveConfigDialog(this, m_scanConfig);
    configDialog->setAttribute( Qt::WA_DeleteOnClose, true );
    configDialog->show();
}

void Brute::on_buttonWordlist_clicked(){
    /*
    WordListDialog *wordlistDialog = nullptr;

    switch (ui->comboBoxOutput->currentIndex()) {
    case brute::OUTPUT::SUBDOMAIN:
        wordlistDialog = new WordListDialog(this, ENGINE::SUBBRUTE);
        break;
    case brute::OUTPUT::TLD:
        wordlistDialog = new WordListDialog(this, ENGINE::TLDBRUTE);
    }

    wordlistDialog->setAttribute( Qt::WA_DeleteOnClose, true );
    connect(wordlistDialog, &WordListDialog::choosenWordlist, this, &Brute::onChoosenWordlist);
    wordlistDialog->show();
    */
}

void Brute::on_checkBoxMultipleTargets_stateChanged(int newState){
    if(newState == Qt::Checked)
        ui->targets->show();
    else
        ui->targets->hide();
}

void Brute::on_comboBoxOutput_currentIndexChanged(int index){
    switch (index) {
    case brute::OUTPUT::SUBDOMAIN:
        m_resultProxyModel->setSourceModel(m_resultModelSubdomain);
        break;
    case brute::OUTPUT::TLD:
        m_resultProxyModel->setSourceModel(m_resultModelTld);
    }

    ui->labelResultsCount->setNum(m_resultProxyModel->rowCount());
}
