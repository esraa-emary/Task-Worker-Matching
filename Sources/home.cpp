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
#include <QSqlQuery>
#include <QMessageBox>
#include <QTimeZone>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>


bool Home::connectToDatabase()
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
    // Clear existing content in ui->frame_7
    QLayout *existingLayout = ui->Requests->layout();
    if (existingLayout) {
        QLayoutItem *item;
        while ((item = existingLayout->takeAt(0)) != nullptr) {
            if (QWidget *widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
        delete existingLayout;
    }

    // Clear any remaining child widgets
    for (QWidget *child : ui->Requests->findChildren<QWidget*>()) {
        child->deleteLater();
    }

    QScrollArea *scrollArea = new (std::nothrow) QScrollArea(ui->Requests);
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
        "   background: #FFD8A9;" // Light orange
        "   width: 8px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #E38B29;" // Dark orange
        "   border-radius: 4px;"
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
    cardsLayout->setSpacing(8); // Compact spacing

    QSqlQuery query;
    query.prepare(
        "SELECT ClientID, RequestID, TaskName, RequestTime, Status "
        "FROM ClientRequestHistoryView "
        "WHERE RequestTime >= :startDate AND RequestTime <= :endDate "
        "ORDER BY RequestTime DESC"
        );
    query.bindValue(":startDate", startDateValue.isValid() ? startDateValue : QDate(2000, 1, 1));
    query.bindValue(":endDate", endDateValue.isValid() ? endDateValue.addDays(1) : QDate(2100, 12, 31));

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

    QVBoxLayout *newLayout = new QVBoxLayout(ui->Requests);
    newLayout->setContentsMargins(0, 0, 0, 0);
    newLayout->addWidget(scrollArea);
    ui->Requests->setLayout(newLayout);
}

QFrame* Home::createRequestCard(int clientId, int requestId, const QString &taskName,
                                const QDateTime &requestTime, const QString &status)
{
    QFrame *card = new QFrame();
    card->setObjectName(QString("requestCard_%1").arg(requestId));
    card->setFrameShape(QFrame::StyledPanel);
    card->setStyleSheet(
        QString("#%1 {"
                "   background-color: #FDEEDC;" // Light peach
                "   border-radius: 20px;"
                "   border: 2px solid #E38B29;" // Border
                "}")
            .arg(card->objectName())
        );

    // Add shadow effect
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(card);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 30));
    shadowEffect->setOffset(0, 2);
    card->setGraphicsEffect(shadowEffect);

    // Main layout for card content
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(8, 8, 8, 8); // Minimized margins
    cardLayout->setSpacing(2); // Tighter spacing

    // Create the main horizontal layout
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create the nested horizontal layout for title and date
    QHBoxLayout *titleDateLayout = new QHBoxLayout();
    titleDateLayout->setSpacing(0); // No spacing between title and date
    titleDateLayout->setContentsMargins(0, 0, 0, 0);

    // Title label
    QLabel *titleLabel = new QLabel(taskName);
    titleLabel->setStyleSheet(
        "font-weight: bold;"
        "font-size: 13px;"
        "color: #F1A661;"
        "padding: 0px;"
        "margin: 0px;"
        );
    titleLabel->setMaximumWidth(100);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Date label
    QString dateString = requestTime.isValid()
                             ? QLocale().toString(requestTime, "M/d/yy")
                             : "Invalid";
    QLabel *dateLabel = new QLabel(dateString);
    dateLabel->setStyleSheet(
        "font-size: 10px;"
        "color: #FFD8A9;"
        "padding: 0px;"
        "padding-left: 2px;"
        "margin: 0px;"
        );
    dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter); // Changed to AlignLeft

    // Add title and date to the nested layout
    titleDateLayout->addWidget(titleLabel); // No stretch to keep them adjacent
    titleDateLayout->addWidget(dateLabel);

    // Address label
    QString address = getAddressForRequest(requestId);
    QLabel *addressLabel = new QLabel();
    if (address.length() > 20) {
        addressLabel->setText(address.left(17) + "...");
        addressLabel->setToolTip(address);
    } else {
        addressLabel->setText(address);
    }
    addressLabel->setStyleSheet(
        "font-size: 10px;"
        "color: #FFD8A9;"
        "padding: 0px;"
        "margin: 0px;"
        );
    addressLabel->setAlignment(Qt::AlignRight);

    // Add to the main layout
    mainLayout->addLayout(titleDateLayout, 4); // Title and date layout gets more space
    mainLayout->addStretch(1);                // Small flexible space
    mainLayout->addWidget(addressLabel, 3);    // Address gets moderate space

    // Workers list - more compact
    QString workersList = getWorkersForRequest(requestId);
    QLabel *workersLabel = new QLabel(workersList);
    workersLabel->setStyleSheet(
        "font-size: 10px;" // Smaller font
        "color: #FFD8A9;" // Light orange
        "padding-top: 0px;"
        "margin-top: 0px;"
        );
    workersLabel->setWordWrap(true); // Allow wrapping for long lists
    workersLabel->setMaximumHeight(20); // Limit height

    // Status label with color coding - more compact design
    QLabel *statusLabel = new QLabel(status);
    QString statusColor;
    if (status.toLower() == "completed") {
        statusColor = "#F1A661"; // Medium orange
    } else if (status.toLower() == "in progress") {
        statusColor = "#FFD8A9"; // Light orange
    } else if (status.toLower() == "pending") {
        statusColor = "#E38B29"; // Dark orange
    } else {
        statusColor = "#FDEEDC"; // Light peach
    }
    statusLabel->setStyleSheet(QString(
                                   "background-color: %1;"
                                   "color: #FFFFFF;"
                                   "border-radius: 8px;"
                                   "padding: 2px 8px;"
                                   "font-weight: bold;"
                                   "font-size: 10px;"
                                   ).arg(statusColor));
    statusLabel->setMaximumHeight(18);
    statusLabel->setMaximumWidth(80);

    // Status row with horizontal layout - more compact
    QHBoxLayout *statusRow = new QHBoxLayout();
    statusRow->setContentsMargins(0, 0, 0, 0);
    statusRow->setSpacing(4);
    statusRow->addWidget(statusLabel);
    statusRow->addStretch();

    // View details button - more compact
    QPushButton *viewButton = new QPushButton("Details");
    viewButton->setCursor(Qt::PointingHandCursor);
    viewButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #E38B29;" // Dark orange
        "   color: #FFFFFF;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 2px 8px;"
        "   font-size: 10px;"
        "   max-height: 18px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #C37422;" // Darker orange
        "}"
        );
    viewButton->setMaximumWidth(60);
    statusRow->addWidget(viewButton);

    // Add layouts to main card layout - more compact overall structure
    cardLayout->addLayout(mainLayout);
    cardLayout->addWidget(workersLabel);
    cardLayout->addLayout(statusRow);

    // Set fixed height for the card to make it more compact
    card->setFixedHeight(90);

    // Make the entire card clickable
    card->setCursor(Qt::PointingHandCursor);
    card->installEventFilter(this);
    card->setProperty("requestId", requestId);

    // Connect the view button click
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

QString Home::getWorkersForRequest(int requestId) {
    QString workers;
    QSqlQuery query;
    query.prepare("SELECT w.NAME "
                  "FROM ASSIGNMENT a "
                  "JOIN WORKER w ON a.WORKERID = w.WORKERID "
                  "WHERE a.REQUESTID = :id");
    query.bindValue(":id", requestId);
    if (query.exec()) {
        QStringList workerNames;
        while (query.next()) {
            workerNames << query.value("NAME").toString();
        }
        workers = workerNames.join(", ");
        qDebug() << "Error fetching workers:" << query.lastError().text();
    }
    return workers.isEmpty() ? "None" : workers;
}


QString Home::getAddressForRequest(int requestId) {
    QString address;
    QSqlQuery query;
    query.prepare("SELECT ADDRESS FROM REQUEST WHERE REQUESTID = :id");
    query.bindValue(":id", requestId);
    if (query.exec() && query.next()) {
        address = query.value("ADDRESS").toString();
    } else {
        qDebug() << "Error fetching address:" << query.lastError().text();
        address = "Unknown";
    }
    return address;
}



// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// for worker page
void Home::loadAllWorkers()
{
    QScrollArea *scrollArea = new (std::nothrow) QScrollArea(ui->frame_21);
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
    query.prepare("SELECT WORKER.WORKERID, LOCATIONS, TASKNAME, RATING "
                  "FROM WORKER, SPECIALTY, TASK "
                  "WHERE WORKER.WORKERID = SPECIALTY.WORKERID AND TASK.TASKID = SPECIALTY.TASKID "
                  "ORDER BY RATING DESC");

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch requests: " + query.lastError().text());
        delete scrollContent;
        delete scrollArea;
        return;
    }

    while (query.next()) {
        int workerId = query.value("WORKERID").toInt();
        QString locations = query.value("LOCATIONS").toString();
        QString taskName = query.value("TASKNAME").toString();
        float rating = query.value("RATING").toFloat();

        QFrame *card = createWorkerCard(workerId, taskName, locations, rating);
        cardsLayout->addWidget(card);
    }

    cardsLayout->addStretch();
    scrollContent->setLayout(cardsLayout);
    scrollArea->setWidget(scrollContent);

    QLayout *existingLayout = ui->frame_21->layout();
    if (existingLayout) {
        existingLayout->addWidget(scrollArea);
    } else {
        QVBoxLayout *newLayout = new QVBoxLayout(ui->frame_21);
        newLayout->addWidget(scrollArea);
        ui->frame_21->setLayout(newLayout);
    }

    if (ui->frame_21->findChild<QTableView*>("tableView")) {
        delete ui->frame_21->findChild<QTableView*>("tableView");
    }
}

QFrame* Home::createWorkerCard(int workerId, QString &taskName,
                               QString &locations, const float &rating)
{
    QFrame *card = new QFrame();
    card->setObjectName(QString("workerCard_%1").arg(workerId));
    card->setFrameShape(QFrame::StyledPanel);

    card->setStyleSheet(
        "QFrame {"
        "   background-color: transparent;"
        "   border: 2px solid #DAA520;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "   max-height: 130px;"
        "}"
        "QFrame:hover {"
        "   background-color: #F0D8A8;"
        "}"
        );

    card->setCursor(Qt::PointingHandCursor);

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(card);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 50));
    shadowEffect->setOffset(2, 2);
    card->setGraphicsEffect(shadowEffect);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);

    QHBoxLayout *topRowLayout = new QHBoxLayout();

    QHBoxLayout *leftGroupLayout = new QHBoxLayout();

    QLabel *workerLabel = new QLabel(QString("Worker %1").arg(workerId));
    workerLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 25px;"
        "   color: #E38B29;"
        "   border: none;"
        "}"
        );

    QLabel *ratingLabel = new QLabel(QString("Rating: %1").arg(rating, 0, 'f', 1));
    ratingLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 25px;"
        "   color: #E38B29;"
        "   border: none;"
        "}"
        );

    leftGroupLayout->addWidget(workerLabel);
    leftGroupLayout->addWidget(ratingLabel);
    leftGroupLayout->addStretch();

    QLabel *locationsLabel = new QLabel("Locations: " + locations);
    locationsLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 25px;"
        "   color: #E38B29;"
        "   border: none;"
        "}"
        );

    topRowLayout->addLayout(leftGroupLayout);
    topRowLayout->addWidget(locationsLabel);

    QLabel *taskLabel = new QLabel(taskName);
    taskLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 15px;"
        "   color: #E38B29;"
        "   margin-top: 8px;"
        "   border: none;"
        "}"
        );

    cardLayout->addLayout(topRowLayout);
    cardLayout->addWidget(taskLabel);
    cardLayout->addStretch();

    return card;
}

void Home::setupWorkerCards()
{
    loadAllWorkers();
}

Home::Home(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Home)
    , requestTask(nullptr)
{
    ui->setupUi(this);
    // Setup profile icon
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

    // Setup filters icon
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

    // Setup date buttons
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

    // Set cursors for interactive elements
    ui->requestsPageBtn_2->setCursor(Qt::PointingHandCursor);
    ui->workersPageBtn_2->setCursor(Qt::PointingHandCursor);
    ui->startDate->setCursor(Qt::PointingHandCursor);
    ui->endDate->setCursor(Qt::PointingHandCursor);
    ui->filterName->setCursor(Qt::PointingHandCursor);

    // for filter icons in workers page
    QSvgWidget *filtersSvg_3 = new QSvgWidget(":/new/svgs/Frame 10.svg");
    filtersSvg_3->setFixedSize(116, 34);
    QVBoxLayout *filtersLayout_3 = qobject_cast<QVBoxLayout *>(ui->filters_3->layout());
    if (filtersLayout_3) {
        filtersLayout_3->insertWidget(0, filtersSvg_3,0,Qt::AlignHCenter);
    } else {
        auto *newLayout_3 = new QVBoxLayout(ui->filters_3);
        newLayout_3->addWidget(filtersSvg_3);
        ui->filters_3->setLayout(newLayout_3);
    }

    QIcon calender_3(":/new/svgs/calender.svg");
    ui->startDate_3->setIcon(calender_3);
    ui->startDate_3->setIconSize(QSize(24, 24));
    ui->startDate_3->setText(" Start Date");
    ui->startDate_3->setFixedHeight(40);

    QIcon calender2_3(":/new/svgs/calender.svg");
    ui->endDate_3->setIcon(calender2_3);
    ui->endDate_3->setIconSize(QSize(24, 24));
    ui->endDate_3->setText(" End Date");
    ui->endDate_3->setFixedHeight(40);
    setupRequestCards();
    setupWorkerCards();
    ui->addRequest->setCursor(Qt::PointingHandCursor);
}

Home::~Home()
{
    // Properly clean up any open calendar widgets
    if (startDateCalendar) {
        startDateCalendar->deleteLater();
        startDateCalendar = nullptr;
    }

    if (endDateCalendar) {
        endDateCalendar->deleteLater();
        endDateCalendar = nullptr;
    }

    delete ui;
    delete requestTask;
}

void Home::on_workersPageBtn_clicked()
{
    QWidget *workersPage = ui->stackedWidget_2->findChild<QWidget*>("workersPage");

    if (workersPage) {
        ui->stackedWidget_2->setCurrentWidget(workersPage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
}

void Home::on_requestsPageBtn_2_clicked()
{
    QWidget *requestsPage = ui->stackedWidget_2->findChild<QWidget*>("requestsPage");

    if (requestsPage) {
        ui->stackedWidget_2->setCurrentWidget(requestsPage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
}

void Home::on_addRequest_clicked()
{
    requestTask = new RequestTask(this);
    requestTask->show();
    this->hide();
}

void Home::on_request_clicked()
{
    if (requestTask) {
        delete requestTask;
    }
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
    // Implementation for other button if needed
}

void Home::on_filterName_clicked()
{
    // Implementation for filtering by name
    // This was missing but should be added if needed
}

void Home::onStartDateSelected(const QDate &date)
{
    startDateValue = date;
    ui->startDate->setText(" " + startDateValue.toString("yyyy-MM-dd"));
    loadAllRequests();
}

void Home::onEndDateSelected(const QDate &date)
{
    endDateValue = date;
    ui->endDate->setText(" " + endDateValue.toString("yyyy-MM-dd"));
    loadAllRequests();
}

void Home::on_startDate_clicked()
{
    // Cleanup any existing calendar to prevent memory leaks
    if (startDateCalendar) {
        startDateCalendar->deleteLater();
    }

    startDateCalendar = new QCalendarWidget();
    startDateCalendar->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    startDateCalendar->setStyleSheet(
        "QCalendarWidget {"
        "   background-color: #FDEEDC;" // Light peach
        "   color: #F1A661;" // Medium orange
        "   font-size: 10px;"
        "}"
        "QCalendarWidget QToolButton {"
        "   color: #F1A661;"
        "}"
        "QCalendarWidget QMenu {"
        "   background-color: #FDEEDC;"
        "   color: #F1A661;"
        "}"
        );
    startDateCalendar->setSelectedDate(startDateValue.isValid() ? startDateValue : QDate::currentDate());
    startDateCalendar->setMinimumDate(QDate(2000, 1, 1));
    startDateCalendar->setMaximumDate(QDate(2100, 12, 31));

    // Position below the button
    QPoint pos = ui->startDate->mapToGlobal(QPoint(0, ui->startDate->height()));
    startDateCalendar->move(pos);

    connect(startDateCalendar, &QCalendarWidget::clicked, this, &Home::onStartDateSelected);
    connect(startDateCalendar, &QCalendarWidget::clicked, startDateCalendar, &QCalendarWidget::deleteLater);
    connect(startDateCalendar, &QCalendarWidget::destroyed, [this]() { startDateCalendar = nullptr; });
    startDateCalendar->show();
}

void Home::on_endDate_clicked()
{
    // Cleanup any existing calendar to prevent memory leaks
    if (endDateCalendar) {
        endDateCalendar->deleteLater();
    }

    endDateCalendar = new QCalendarWidget();
    endDateCalendar->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    endDateCalendar->setStyleSheet(
        "QCalendarWidget {"
        "   background-color: #FDEEDC;" // Light peach
        "   color: #F1A661;" // Medium orange
        "   font-size: 10px;"
        "}"
        "QCalendarWidget QToolButton {"
        "   color: #F1A661;"
        "}"
        "QCalendarWidget QMenu {"
        "   background-color: #FDEEDC;"
        "   color: #F1A661;"
        "}"
        );
    endDateCalendar->setSelectedDate(endDateValue.isValid() ? endDateValue : QDate::currentDate());
    endDateCalendar->setMinimumDate(QDate(2000, 1, 1));
    endDateCalendar->setMaximumDate(QDate(2100, 12, 31));

    // Position below the button
    QPoint pos = ui->endDate->mapToGlobal(QPoint(0, ui->endDate->height()));
    endDateCalendar->move(pos);

    connect(endDateCalendar, &QCalendarWidget::clicked, this, &Home::onEndDateSelected);
    connect(endDateCalendar, &QCalendarWidget::clicked, endDateCalendar, &QCalendarWidget::deleteLater);
    connect(endDateCalendar, &QCalendarWidget::destroyed, [this]() { endDateCalendar = nullptr; });

    endDateCalendar->show();
}
