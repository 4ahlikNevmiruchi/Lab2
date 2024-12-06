#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "linkmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddLinkClicked();
    void onSearchClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onExportClicked();
    void onImportClicked();
    void onPreviewClicked();
    void updateTable();
    void onTableLinksClicked();
    void onBackClicked();

private:
    Ui::MainWindow *ui;
    LinkManager *manager;
    QVector<Link> fullLinks;
};

#endif // MAINWINDOW_H
