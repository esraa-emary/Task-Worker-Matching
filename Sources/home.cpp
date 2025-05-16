#include "../Headers/home.h"
#include "ui_home.h"
#include <QSvgWidget>
#include <QVBoxLayout>
#include "ui_requesttask.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>

#include <QTableView>
#include <QSqlTableModel>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>


#include <QTimeZone>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
bool connectToDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    QString serverName = "HEFNY";
    QString dbName = "TaskWorkerMatching";

    QString connectionString = QString(
                                   "Driver={ODBC Driver 17 for SQL Server};"
                                   "Server=%1;"
                                   "Database=%2;"
                                   "Trusted_Connection=yes;"
                                   "Encrypt=yes;"
                                   "TrustServerCertificate=yes;"
                                   ).arg(serverName).arg(dbName);

    db.setDatabaseName(connectionString);

    if (!db.open()) {
        qDebug() << "Error connecting to database:" << db.lastError().text();
        return false;
    }

    qDebug() << "Connected to database successfully";
    qDebug() << "System timezone:" << QTimeZone::systemTimeZoneId();
    return true;
}

void Home::loadAllRequests()
{
    QScrollArea *scrollArea = new (std::nothrow) QScrollArea(ui->frame_7);
    if (!scrollArea) {
        qDebug() << "Failed to allocate QScrollArea";
        return;
    }
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "   background-color: transparent;"
        "   border: none;"
        "}"
        "QScrollBar:vertical {"
        "   background: #f1f1f1;"
        "   width: 10px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #DAA520;" /* Orange scrollbar handle to match design */
        "   border-radius: 5px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "}"
        );

    QWidget *scrollContent = new (std::nothrow) QWidget();
    if (!scrollContent) {
        qDebug() << "Failed to allocate QWidget";
        delete scrollArea;
        return;
    }
    scrollContent->setStyleSheet(
        "QWidget {"
        "   background-color: transparent;"
        "}"
        );
    QVBoxLayout *cardsLayout = new QVBoxLayout(scrollContent);
    cardsLayout->setSpacing(15);

    QSqlQuery query;
    query.prepare("SELECT ClientID, RequestID, TaskName, RequestTime, Status "
                  "FROM ClientRequestHistoryView ORDER BY RequestTime DESC");

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch requests: " + query.lastError().text());
        delete scrollContent;
        delete scrollArea;
        return;
    }

    while (query.next()) {
        int clientId = query.value("ClientID").toInt();
        int requestId = query.value("RequestID").toInt();
        QString taskName = query.value("TaskName").toString();
        QVariant requestTimeVar = query.value("RequestTime");
        QString status = query.value("Status").toString();

        QDateTime requestTime;
        if (!requestTimeVar.isNull() && requestTimeVar.canConvert<QDateTime>()) {
            requestTime = requestTimeVar.toDateTime();
            requestTime.setTimeZone(QTimeZone("Europe/Athens")); // EEST
            if (requestTime.timeZone().isValid() && requestTime.timeSpec() != Qt::TimeZone) {
                qDebug() << "Warning: Datetime timezone conversion applied for RequestID:" << requestId;
            }
        } else {
            qDebug() << "Invalid or NULL RequestTime for RequestID:" << requestId;
            continue;
        }

        QFrame *card = createRequestCard(clientId, requestId, taskName, requestTime, status);
        cardsLayout->addWidget(card);
    }

    cardsLayout->addStretch();
    scrollContent->setLayout(cardsLayout);
    scrollArea->setWidget(scrollContent);

    QLayout *existingLayout = ui->frame_7->layout();
    if (existingLayout) {
        existingLayout->addWidget(scrollArea);
    } else {
        QVBoxLayout *newLayout = new QVBoxLayout(ui->frame_7);
        newLayout->addWidget(scrollArea);
        ui->frame_7->setLayout(newLayout);
    }

    if (ui->frame_7->findChild<QTableView*>("tableView")) {
        delete ui->frame_7->findChild<QTableView*>("tableView");
    }
}
QFrame* Home::createRequestCard(int clientId, int requestId, const QString &taskName,
                                const QDateTime &requestTime, const QString &status)
{
    QFrame *card = new QFrame();
    card->setObjectName(QString("requestCard_%1").arg(requestId));
    card->setFrameShape(QFrame::StyledPanel);

    card->setStyleSheet(
        "QFrame {"
        "   background-color: transparent;"
        "   border: 2px solid #DAA520;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "}"
        "QFrame:hover {"
        "   background-color: #F0D8A8;"
        "}"
        );

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(card);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 50));
    shadowEffect->setOffset(2, 2);
    card->setGraphicsEffect(shadowEffect);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);

    // Title Label
    QLabel *titleLabel = new QLabel(QString("Request %1").arg(requestId));
    titleLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "   color: #E38B29;"
        "   margin-bottom: 5px;"
        "}"
        );

    QString dateString = requestTime.isValid()
                             ? QLocale().toString(requestTime, "M-d-yyyy, d-M-yyyy")
                             : "Unknown Date";
    QLabel *dateLabel = new QLabel(dateString);
    dateLabel->setStyleSheet(
        "QLabel {"
        "   color: #E38B29;"
        "   margin-bottom: 5px;"
        "}"
        );

    // Workers Label
    QString workersString = getWorkersForRequest(requestId);
    QLabel *workersLabel = new QLabel("Workers: " + workersString);
    workersLabel->setStyleSheet(
        "QLabel {"
        "   color: #E38B29;" /* Updated font color */
        "   margin-bottom: 5px;"
        "}"
        );

    // Address Label
    QString address = getAddressForRequest(requestId);
    QLabel *addressLabel = new QLabel("Address: " + address);
    addressLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   color: #E38B29;"
        "   margin-top: 5px;"
        "}"
        );

    // Status Label
    QLabel *statusLabel = new QLabel(status);
    QString statusColor;
    if (status.toLower() == "completed") {
        statusColor = "#4CAF50";
    } else if (status.toLower() == "in progress") {
        statusColor = "#2196F3";
    } else if (status.toLower() == "pending") {
        statusColor = "#FF9800";
    } else {
        statusColor = "#9E9E9E";
    }
    statusLabel->setStyleSheet(QString(
                                   "QLabel {"
                                   "   background-color: %1;"
                                   "   color: white;"
                                   "   border-radius: 10px;"
                                   "   padding: 3px 10px;"
                                   "   font-weight: bold;"
                                   "}"
                                   ).arg(statusColor));

    // Layout
    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(dateLabel);
    cardLayout->addWidget(workersLabel);
    cardLayout->addWidget(addressLabel);

    QHBoxLayout *statusRow = new QHBoxLayout();
    statusRow->addWidget(statusLabel);
    statusRow->addStretch();

    QPushButton *viewButton = new QPushButton("View Details");
    viewButton->setCursor(Qt::PointingHandCursor);
    viewButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #DAA520;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 5px 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #C68E17;"
        "}"
        );
    statusRow->addWidget(viewButton);
    cardLayout->addLayout(statusRow);

    card->setCursor(Qt::PointingHandCursor);
    card->installEventFilter(this);
    card->setProperty("requestId", requestId);

    connect(viewButton, &QPushButton::clicked, this, [this, requestId]() {
        this->viewRequestDetails(requestId);
    });

    return card;
}
bool Home::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QFrame *card = qobject_cast<QFrame*>(watched);
        if (card && card->objectName().startsWith("requestCard_")) {
            int requestId = card->property("requestId").toInt();
            viewRequestDetails(requestId);
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void Home::viewRequestDetails(int requestId)
{
    qDebug() << "Viewing details for request ID:" << requestId;

    QSqlQuery query;
    query.prepare("SELECT CLIENTID, TASKID, ADDRESS, REQUESTTIME, PREFERREDTIMESLOT, REQUESTDESCRIPTION "
                  "FROM REQUEST WHERE REQUESTID = :id");
    query.bindValue(":id", requestId);

    if (!query.exec()) {
        qDebug() << "Query error:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch request details: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        int clientId = query.value("CLIENTID").toInt();
        int taskId = query.value("TASKID").toInt();
        QString address = query.value("ADDRESS").toString();
        QVariant requestTimeVar = query.value("REQUESTTIME");
        QVariant preferredTimeVar = query.value("PREFERREDTIMESLOT");
        QString description = query.value("REQUESTDESCRIPTION").toString();

        QDateTime requestTime;
        QString requestTimeStr = "Unknown";
        if (!requestTimeVar.isNull() && requestTimeVar.canConvert<QDateTime>()) {
            requestTime = requestTimeVar.toDateTime();
            requestTime.setTimeZone(QTimeZone("Europe/Athens")); // EEST
            requestTimeStr = QLocale().toString(requestTime, "yyyy-MM-dd hh:mm ap");
        }

        QDateTime preferredTime;
        QString preferredTimeStr = "Unknown";
        if (!preferredTimeVar.isNull() && preferredTimeVar.canConvert<QDateTime>()) {
            preferredTime = preferredTimeVar.toDateTime();
            preferredTime.setTimeZone(QTimeZone("Europe/Athens")); // EEST
            preferredTimeStr = QLocale().toString(preferredTime, "yyyy-MM-dd hh:mm ap");
        }

        QString descriptionStr = description.isEmpty() ? "No description provided" : description;

        QMessageBox::information(this, "Request Details",
                                 QString("Request #%1\nClient ID: %2\nTask ID: %3\n"
                                         "Address: %4\nRequest Time: %5\n"
                                         "Preferred Time: %6\nDescription: %7")
                                     .arg(requestId)
                                     .arg(clientId)
                                     .arg(taskId)
                                     .arg(address)
                                     .arg(requestTimeStr)
                                     .arg(preferredTimeStr)
                                     .arg(descriptionStr));
    } else {
        QMessageBox::warning(this, "Request Not Found",
                             "Could not find details for the selected request.");
    }
}

void Home::setupRequestCards()
{
    if (connectToDatabase()) {
        loadAllRequests();
    } else {
        QMessageBox::critical(this, "Database Connection Error",
                              "Failed to connect to the database.");
    }
}

Home::Home(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Home)
    , profile(nullptr)
    , requestTask(nullptr)
    , offeredTasks(nullptr)
{
    ui->setupUi(this);
    QSvgWidget *profileIcon = new QSvgWidget(":/new/svgs/Group.svg");
    profileIcon->setFixedSize(58, 58);
    QHBoxLayout *profileLayout = qobject_cast<QHBoxLayout *>(ui->profile->layout());
    if (profileLayout) {
        profileLayout->insertWidget(0, profileIcon);
    } else {
        auto *newLayout = new QHBoxLayout(ui->profile);
        newLayout->addWidget(profileIcon);
        ui->profile->setLayout(newLayout);
    }

    QSvgWidget *filtersSvg = new QSvgWidget(":/new/svgs/Frame 10.svg");
    filtersSvg->setFixedSize(116, 34);
    QVBoxLayout *filtersLayout = qobject_cast<QVBoxLayout *>(ui->filters->layout());
    if (filtersLayout) {
        filtersLayout->insertWidget(0, filtersSvg);
    } else {
        auto *newLayout = new QVBoxLayout(ui->filters);
        newLayout->addWidget(filtersSvg);
        ui->filters->setLayout(newLayout);
    }

    QIcon calender(":/new/svgs/calender.svg");
    ui->startDate->setIcon(calender);
    ui->startDate->setIconSize(QSize(24, 24));
    ui->startDate->setText(" Start Date");
   ui->startDate->setFixedHeight(40);
    QIcon calender2(":/new/svgs/calender.svg");
    ui->endDate->setIcon(calender2);
    ui->endDate->setIconSize(QSize(24, 24));
    ui->endDate->setText(" End Date");
    ui->endDate->setFixedHeight(40);

    setupRequestCards();
    ui->addRequest->setCursor(Qt::PointingHandCursor);
}

Home::~Home()
{
    delete ui;
    delete profile;
    delete requestTask;
    delete offeredTasks;
}

void Home::on_offeredTasks_clicked()
{
    offeredTasks = new OfferedTasks(this);
    offeredTasks->show();
}

void Home::on_profile_clicked()
{
    profile = new Profile(this);
    profile->show();
}

void Home::on_request_clicked()
{
    requestTask = new RequestTask(this);
    requestTask->show();
}

void Home::on_logout_clicked()
{
    emit backToMainWindow();
    this->close();
}


void Home::on_pushButton_clicked()
{

}

