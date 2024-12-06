#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "link.h"
#include "linkmanager.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QFileDialog>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), manager(new LinkManager()) {
    ui->setupUi(this);
    ui->tableLinks->setSortingEnabled(true);
    ui->tableLinks->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->btnAddLink, &QPushButton::clicked, this, &MainWindow::onAddLinkClicked);
    connect(ui->btnSearch, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(ui->btnEdit, &QPushButton::clicked, this, &MainWindow::onEditClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(ui->btnExport, &QPushButton::clicked, this, &MainWindow::onExportClicked);
    connect(ui->btnImport, &QPushButton::clicked, this, &MainWindow::onImportClicked);
    connect(ui->btnPreview, &QPushButton::clicked, this, &MainWindow::onPreviewClicked);
    connect(ui->tableLinks, &QTableWidget::clicked, this, &MainWindow::onTableLinksClicked);
    connect(ui->btnBack, &QPushButton::clicked, this, &MainWindow::onBackClicked);
}

MainWindow::~MainWindow() {
    delete ui;
    delete manager;
}

void MainWindow::onAddLinkClicked() {
    QString type = ui->lineEditType->text().trimmed();
    QString url = ui->lineEditUrl->text().trimmed();
    QString context = ui->lineEditContext->text().trimmed();

    if (type.isEmpty() || url.isEmpty() || context.isEmpty()) {
        QMessageBox::warning(this, "Error", "All fields must be filled to add a link!");
        return;
    }

    QRegularExpression urlRegex(R"((https?|ftp)://[^\s/$.?#].[^\s]*)");
    if (!urlRegex.match(url).hasMatch()) {
        QMessageBox::warning(this, "Error", "Invalid URL format!");
        return;
    }

    manager->addLink(Link(type, url, context));
    updateTable();

    ui->lineEditType->clear();
    ui->lineEditUrl->clear();
    ui->lineEditContext->clear();

    QMessageBox::information(this, "Success", "Link added successfully!");
}

void MainWindow::onSearchClicked() {
    QString query = ui->lineEditType->text().trimmed();
    QString selectedField = ui->comboSearchField->currentText();

    if (query.isEmpty()) {
        QMessageBox::warning(this, "Error", "Search query cannot be empty!");
        return;
    }

    if (fullLinks.isEmpty()) {
        fullLinks = manager->getAllLinks();
    }

    QVector<Link> results;

    for (const Link &link : fullLinks) {
        bool match = false;

        if (selectedField == "All Fields") {
            match = link.getType().contains(query, Qt::CaseInsensitive) ||
                    link.getUrl().contains(query, Qt::CaseInsensitive) ||
                    link.getContext().contains(query, Qt::CaseInsensitive);
        } else if (selectedField == "Type") {
            match = link.getType().contains(query, Qt::CaseInsensitive);
        } else if (selectedField == "URL") {
            match = link.getUrl().contains(query, Qt::CaseInsensitive);
        } else if (selectedField == "Context") {
            match = link.getContext().contains(query, Qt::CaseInsensitive);
        }

        if (match) {
            results.append(link);
        }
    }

    manager->setLinks(results);
    updateTable();

    if (results.isEmpty()) {
        QMessageBox::information(this, "Search Results", "No matching links found.");
    }
}

void MainWindow::updateTable() {
    ui->tableLinks->setRowCount(0);
    QVector<Link> allLinks = manager->getAllLinks();

    ui->tableLinks->setRowCount(allLinks.size());
    ui->tableLinks->setColumnCount(3);
    ui->tableLinks->setHorizontalHeaderLabels({"Type", "URL", "Context"});

    for (int i = 0; i < allLinks.size(); ++i) {
        const Link &link = allLinks[i];
        ui->tableLinks->setItem(i, 0, new QTableWidgetItem(link.getType()));
        ui->tableLinks->setItem(i, 1, new QTableWidgetItem(link.getUrl()));
        ui->tableLinks->setItem(i, 2, new QTableWidgetItem(link.getContext()));
    }
}

void MainWindow::onEditClicked() {
    int row = ui->tableLinks->currentRow();
    if (row == -1) {
        QMessageBox::warning(this, "Error", "Please select a link to edit!");
        return;
    }

    QString type = ui->tableLinks->item(row, 0)->text();
    QString url = ui->tableLinks->item(row, 1)->text();
    QString context = ui->tableLinks->item(row, 2)->text();

    ui->lineEditType->setText(type);
    ui->lineEditUrl->setText(url);
    ui->lineEditContext->setText(context);

    manager->getAllLinks().remove(row);
    updateTable();
}

void MainWindow::onDeleteClicked() {
    int row = ui->tableLinks->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Please select a link to delete!");
        return;
    }

    if (QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this link?") == QMessageBox::Yes) {
        manager->removeLinkAt(row);
        updateTable();
        QMessageBox::information(this, "Success", "Link deleted successfully!");
    }
}

void MainWindow::onExportClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Links", "", "CSV Files (*.csv);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Unable to open file for writing!");
        return;
    }

    QTextStream out(&file);
    out << "Type,URL,Context\n";

    for (const Link &link : manager->getAllLinks()) {
        out << QString("%1,%2,%3\n").arg(link.getType(), link.getUrl(), link.getContext());
    }

    file.close();
    QMessageBox::information(this, "Success", "Links exported successfully!");
}

void MainWindow::onImportClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Import Links", "", "CSV Files (*.csv);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Unable to open file for reading!");
        return;
    }

    QTextStream in(&file);
    QVector<Link> newLinks;

    QString header = in.readLine();

while (!in.atEnd()) {
    QString line = in.readLine().trimmed();

    if (line.startsWith("URL=")) {
        line = line.mid(4);
    }

    QStringList parts;

    QRegularExpression csvRegex("^\"([^\"]*)\",\"([^\"]*)\",\"([^\"]*)\"$");

    QRegularExpressionMatch match = csvRegex.match(line);

    if (match.hasMatch()) {
        parts = {match.captured(1), match.captured(2), match.captured(3)};
    } else {
        parts = line.split(",");
    }

    if (parts.size() != 3) {
        QMessageBox::warning(this, "Error", "Malformed line in CSV file: " + line);
        continue;
    }

    QString type = parts[0].trimmed();
    QString url = parts[1].trimmed();
    QString context = parts[2].trimmed();

    // Validate URL format
    QRegularExpression urlRegex(R"((https?|ftp)://[^\s/$.?#].[^\s]*)");
    if (!urlRegex.match(url).hasMatch()) {
        QMessageBox::warning(this, "Error", "Invalid URL in line: " + line);
        continue;
    }

    newLinks.append(Link(type, url, context));
}

    manager->setLinks(newLinks);
    updateTable();

    file.close();
    QMessageBox::information(this, "Success", "Links imported successfully!");
}

void MainWindow::onPreviewClicked() {

    int row = ui->tableLinks->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Please select a link to preview!");
        return;
    }

    QString url = ui->tableLinks->item(row, 1)->text();

    if (!url.startsWith("http://") && !url.startsWith("https://")) {
        QMessageBox::warning(this, "Error", "Invalid URL! Ensure it starts with 'http://' or 'https://'.");
        return;
    }

    if (!QDesktopServices::openUrl(QUrl(url))) {
        QMessageBox::critical(this, "Error", "Failed to open the URL in the browser.");
    }
}

void MainWindow::onTableLinksClicked() {
    if (ui->tableLinks->selectionModel()->selectedRows().isEmpty()) {
        ui->tableLinks->clearSelection();
    }
}

void MainWindow::onBackClicked() {
    if (!fullLinks.isEmpty()) {
        manager->setLinks(fullLinks);
        updateTable();
        fullLinks.clear();
    } else {
        QMessageBox::information(this, "Info", "The full list is already displayed.");
    }
}

