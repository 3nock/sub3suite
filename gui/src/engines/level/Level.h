#ifndef LEVEL_H
#define LEVEL_H

#include "src/engines/Base.h"
#include "lib-level.h"
#include "LevelEnumerator.h"

namespace Ui {
    class Level;
}

class Level : public BaseClass{
        Q_OBJECT

    private:
        Ui::Level *ui;

    private:
        void startScan();
        void nextLevel();
        //...
        ResultsModel *m_resultsModel;
        //...
        ScanStatus *m_scanStatus;
        ScanConfig *m_scanConfig;
        level::ScanArguments *m_scanArguments;
        //...
        int lastScanResultsCount = 0;

    public:
        explicit Level(QWidget *parent = nullptr, ResultsModel *resultsModel = nullptr);
        ~Level();

    public slots:
        void scanThreadEnd();
        void scanResult(QString subdomain, QString ipAddress);
        //...
        void choosenWordlist(QString wordlistFilename);

    private slots:
        void on_pushButton_start_clicked();
        void on_pushButton_pause_clicked();
        void on_pushButton_stop_clicked();
        void on_toolButton_config_clicked();
        void on_pushButton_clearResults_clicked();
        void on_pushButton_action_clicked();
        void on_pushButton_removeTargets_clicked();
        void on_pushButton_clearTargets_clicked();
        void on_pushButton_loadTargets_clicked();
        void on_pushButton_removeWordlist_clicked();
        void on_pushButton_clearWordlist_clicked();
        void on_pushButton_loadWordlist_clicked();
        void on_pushButton_addWordlist_clicked();
        void on_toolButton_wordlist_clicked();
        void on_lineEdit_wordlist_returnPressed();
        void on_pushButton_addTargets_clicked();
        void on_lineEdit_targets_returnPressed();
        void on_tableView_results_customContextMenuRequested(const QPoint &pos);
};

#endif // LEVEL_H