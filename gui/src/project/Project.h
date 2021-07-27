#ifndef PROJECT_H
#define PROJECT_H

#include "src/core.h"

namespace Ui {
    class Project;
}

class Project : public QWidget{
        Q_OBJECT

    private:
        Ui::Project *ui;

    public:
        void updateFilter();
        //...
        ResultsModel *m_resultsModel;
        QSortFilterProxyModel *m_proxyModel;

    public:
        explicit Project(QWidget *parent = nullptr, ResultsModel *resultsModel = nullptr);
        ~Project();

    private slots:
        void on_pushButton_clearInScope_clicked();
        void on_pushButton_removeInScope_clicked();
        void on_pushButton_loadInScope_clicked();
        void on_pushButton_addInScope_clicked();
        void on_lineEdit_inScope_returnPressed();
        //...
        void on_pushButton_clearOutScope_clicked();
        void on_pushButton_removeOutScope_clicked();
        void on_pushButton_loadOutScope_clicked();
        void on_pushButton_addOutScope_clicked();
        void on_lineEdit_outScope_returnPressed();
};

#endif // PROJECT_H