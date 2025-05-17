#include "../Headers/home.h"
#include "ui_home.h"
#include <QSvgWidget>
#include <QVBoxLayout>
#include "ui_requesttask.h"
#include "../Headers/signup.h"
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
#include <QTextEdit>
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// for Requests page
void Home::setClient(int &id,QString &name,QString &password,QString &address,QString &email,QString &phone,QString &feedback){
    clientData.id = id;
    clientData.name = name;
    clientData.password = password;
    clientData.address = address;
    clientData.email = email;
    clientData.phone = phone;
    clientData.feedback = feedback;

    // put data into profile;
    ui->pushButton_2->setText(clientData.name);
    ui->pushButton_3->setText(clientData.name);
    ui->pushButton_4->setText(clientData.name);
    ui->pushButton_5->setText(clientData.name);
    ui->pushButton_6->setText(clientData.name);
    ui->pushButton_7->setText(clientData.name);
}
bool Home::connectToDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    // QString serverName = "DESKTOP-TKK26SO";
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
        "   background: #FFD8A9;"
        "   width: 8px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #E38B29;"
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
    titleDateLayout->setSpacing(0);
    titleDateLayout->setContentsMargins(0, 0, 0, 0);

    // Title label
    QLabel *titleLabel = new QLabel(taskName);
    titleLabel->setStyleSheet(
        "font-weight: bold;"
        "font-size: 14px;"
        "color: #E38B29;"
        "padding: 0px;"
        "margin: 0px;"
        );
    titleLabel->setMaximumWidth(300);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Date label
    QString dateString = requestTime.isValid()
                             ? QLocale().toString(requestTime, "M/d/yy")
                             : "Invalid";
    QLabel *dateLabel = new QLabel(dateString);
    dateLabel->setStyleSheet(
        "font-size: 10px;"
        "color: #E38B29;"
        "opacity: 0.4;"
        "padding: 0px;"
        "margin: 0px;"
        "margin-left: 5px;"
        "padding-top: 10px;"
        );
    dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    // Add title and date to the nested layout
    titleDateLayout->addWidget(titleLabel,0);
    titleDateLayout->addWidget(dateLabel,1);

    // Address label
    QString address = getAddressForRequest(requestId);
    QLabel *addressLabel = new QLabel();
    if (address.length() > 40) {
        addressLabel->setText(address.left(17) + "...");
        addressLabel->setToolTip(address);
    } else {
        addressLabel->setText(address);
    }
    addressLabel->setStyleSheet(
        "font-size: 14px;"
        "color: #E38B29;"
        "padding: 0px;"
        "margin: 0px;"
        );
    addressLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Add to the main layout
    mainLayout->addLayout(titleDateLayout, 4); // Title and date layout gets more space
    mainLayout->addStretch(1);                // Small flexible space
    mainLayout->addWidget(addressLabel, 3);    // Address gets moderate space

    // Workers list - more compact
    QString workersList = "Workers: " + getWorkersForRequest(requestId);
    QLabel *workersLabel = new QLabel(workersList);
    workersLabel->setStyleSheet(
        "font-size: 12px;"
        "color: #E38B29;"
        "padding-top: 0px;"
        "margin-top: 0px;"
        );
    workersLabel->setWordWrap(true);
    workersLabel->setMaximumHeight(30);

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
                                   "border-radius: 15px;"
                                   "padding: 2px 8px;"
                                   "font-weight: bold;"
                                   "font-size: 14px;"
                                   ).arg(statusColor));
    statusLabel->setMaximumHeight(30);
    statusLabel->setMaximumWidth(100);

    // Status row with horizontal layout - more compact
    QHBoxLayout *statusRow = new QHBoxLayout();
    statusRow->setContentsMargins(0, 0, 0, 0);
    statusRow->setSpacing(4);
    statusRow->addWidget(statusLabel);
    statusRow->addStretch();

    // Add layouts to main card layout - more compact overall structure
    cardLayout->addLayout(mainLayout);
    cardLayout->addWidget(workersLabel);
    cardLayout->addLayout(statusRow);

    // Set fixed height for the card to make it more compact
    card->setFixedHeight(120);

    // Make the entire card clickable
    card->setCursor(Qt::PointingHandCursor);
    card->installEventFilter(this);
    card->setProperty("requestId", requestId);

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
        else if (card && card->objectName().startsWith("workerCard_")) {
            int workerId = card->property("workerId").toInt();
            viewWorkerDetails(workerId);
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void Home::viewRequestDetails(int requestId)
{
    QSqlQuery query;
    query.prepare(
        "SELECT r.CLIENTID, r.TASKID, t.TASKNAME, r.ADDRESS, r.REQUESTTIME, r.REQUESTDESCRIPTION "
        "FROM REQUEST r "
        "JOIN TASK t ON r.TASKID = t.TASKID "
        "WHERE r.REQUESTID = :id"
        );
    query.bindValue(":id", requestId);

    if (!query.exec()) {
        qDebug() << "Query error:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch request details: " + query.lastError().text());
        return;
    }

    QWidget *requestPage = ui->stackedWidget_2->widget(2);
    if (!requestPage) {
        qDebug() << "Error: requestPage not found!";
        return;
    }

    if (query.next()) {
        QString taskName = query.value("TASKNAME").toString();
        QString address = query.value("ADDRESS").toString();
        QDateTime requestTime = query.value("REQUESTTIME").toDateTime();
        requestTime.setTimeZone(QTimeZone("Europe/Athens")); // Adjust for EEST
        QString description = query.value("REQUESTDESCRIPTION").toString();

        QString dateString = requestTime.isValid() ? QLocale().toString(requestTime, "yyyy-MM-dd hh:mm ap") : "Unknown";

        QFrame *mainFrame = requestPage->findChild<QFrame*>("frame_10");
        if (mainFrame) {
            QFrame *contentFrame = mainFrame->findChild<QFrame*>("RequestContent");
            if (contentFrame) {
                QFrame *headerFrame = contentFrame->findChild<QFrame*>("requestHeader");
                if (headerFrame) {
                    QHBoxLayout *headerLayout = qobject_cast<QHBoxLayout*>(headerFrame->layout());
                    if (!headerLayout) {
                        headerLayout = new QHBoxLayout(headerFrame);
                        headerFrame->setLayout(headerLayout);
                    }
                    QPushButton *backButton = headerFrame->findChild<QPushButton*>("backButton");
                    if (!backButton) {
                        backButton = new QPushButton(headerFrame);
                        backButton->setObjectName("backButton");
                        backButton->setStyleSheet(
                            "QPushButton {"
                            "   background-color: #E38B29;"
                            "   border-radius: 22px;"
                            "}"
                            "QPushButton:hover {"
                            "   background-color: #C37422;"
                            "}"
                            );
                        backButton->setIcon(QIcon(":/new/svgs/back.svg"));
                        backButton->setIconSize(QSize(25, 27));
                        backButton->setFixedSize(50, 44);
                        backButton->setCursor(Qt::PointingHandCursor);
                        headerLayout->insertWidget(0, backButton);
                        connect(backButton, &QPushButton::clicked, this, [this]() {
                            ui->stackedWidget_2->setCurrentIndex(0);
                        });
                    }


                    QFrame *titleDateFrame = headerFrame->findChild<QFrame*>("titleDateFrame");
                    QHBoxLayout *titleDateLayout;
                    if (!titleDateFrame) {
                        titleDateFrame = new QFrame(headerFrame);
                        titleDateFrame->setObjectName("titleDateFrame");
                        titleDateLayout = new QHBoxLayout(titleDateFrame);
                        titleDateFrame->setLayout(titleDateLayout);
                        titleDateLayout->setSpacing(0);
                        if (headerLayout) {
                            headerLayout->addWidget(titleDateFrame,0);
                        }

                    }else{
                        titleDateLayout = qobject_cast<QHBoxLayout*>(titleDateFrame->layout());
                    }

                    QLabel *titleLabel = titleDateFrame->findChild<QLabel*>("requestTitle");
                    if (!titleLabel) {
                        titleLabel = new QLabel(titleDateFrame);
                        titleLabel->setObjectName("requestTitle");
                        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #E38B29;");
                        titleLabel->setMaximumWidth(500);

                        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

                        titleLabel->setSizePolicy(sizePolicy);

                        titleDateLayout->addWidget(titleLabel,0);
                    }
                    titleLabel->setText(taskName);

                    QLabel *dateLabel = titleDateFrame->findChild<QLabel*>("requestDate");
                    if (!dateLabel) {
                        dateLabel = new QLabel(titleDateFrame);
                        dateLabel->setObjectName("requestDate");
                        dateLabel->setStyleSheet("font-size: 12px; color: #F1A661; opacity: 0.3; padding-left:2px;padding-top:15px;");
                        dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
                        titleDateLayout->addWidget(dateLabel,1);
                    }
                    dateLabel->setText(dateString);

                    QLabel *addressLabel = headerFrame->findChild<QLabel*>("requestAddress");
                    if (!addressLabel) {
                        addressLabel = new QLabel(headerFrame);
                        addressLabel->setObjectName("requestAddress");
                        addressLabel->setStyleSheet("font-size: 16px; color: #F1A661;");
                        headerFrame->layout()->addWidget(addressLabel);
                        addressLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
                    }
                    addressLabel->setText(address);
                }

                QFrame *tasksFrame = contentFrame->findChild<QFrame*>("tasks");
                if (tasksFrame) {
                    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(tasksFrame->layout());
                    if (!layout) {
                        layout = new QHBoxLayout(tasksFrame);
                        tasksFrame->setLayout(layout);
                    }

                    QLabel *taskTitleLabel = tasksFrame->findChild<QLabel*>("taskTitleLabel");
                    if (!taskTitleLabel) {
                        taskTitleLabel = new QLabel(tasksFrame);
                        taskTitleLabel->setObjectName("taskTitleLabel");
                        taskTitleLabel->setStyleSheet("font-weight: bold; font-size: 20px; color: #E38B29;");
                        layout->insertWidget(0, taskTitleLabel);
                    }
                    taskTitleLabel->setText("Tasks ");

                    QLabel *tasksLabel = tasksFrame->findChild<QLabel*>("tasksLabel");
                    if (!tasksLabel) {
                        tasksLabel = new QLabel(tasksFrame);
                        tasksLabel->setObjectName("tasksLabel");
                        tasksLabel->setStyleSheet("font-size: 16px; color: #E38B29;");
                        tasksLabel->setAlignment(Qt::AlignVCenter);
                        layout->addWidget(tasksLabel);
                    }
                    tasksLabel->setText(taskName);
                }


                // Populate description frame
                QFrame *descriptionFrame = contentFrame->findChild<QFrame*>("description");
                if (descriptionFrame) {
                    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(descriptionFrame->layout());
                    if (!layout) {
                        layout = new QVBoxLayout(descriptionFrame);
                        descriptionFrame->setLayout(layout);
                    }

                    QLabel *descriptionTitle = descriptionFrame->findChild<QLabel*>("descriptionTitle");
                    if (!descriptionTitle) {
                        descriptionTitle = new QLabel("Description", descriptionFrame);
                        descriptionTitle->setObjectName("descriptionTitle");
                        descriptionTitle->setStyleSheet(
                            "color: #E38B29;"
                            "font-size: 20px;"
                            "font-weight: bold;"
                            );
                        layout->addWidget(descriptionTitle);
                    }

                    // Check if the QTextEdit already exists
                    QTextEdit *descriptionText = descriptionFrame->findChild<QTextEdit*>("descriptionText");
                    if (!descriptionText) {
                        descriptionText = new QTextEdit(descriptionFrame);
                        descriptionText->setObjectName("descriptionText");
                        descriptionText->setReadOnly(true);
                        descriptionText->setStyleSheet(
                            "background-color: transparent;"
                            "color: #E38B29;"
                            "border-radius: 5px;"
                            "font-size: 16px;"
                            );
                        layout->addWidget(descriptionText);
                    }

                    descriptionText->setText(description.isEmpty() ? "No description provided" : description);
                }
            }

            QFrame *workersListFrame = mainFrame->findChild<QFrame*>("workersListFrame");
            if (!workersListFrame) {
                workersListFrame = new QFrame(mainFrame);
                workersListFrame->setObjectName("workersListFrame");

                QVBoxLayout *workersLayout = new QVBoxLayout(workersListFrame);
                workersListFrame->setLayout(workersLayout);

                QLabel *workerListLabel = new QLabel("Workers");
                workerListLabel->setObjectName("workerList");
                workerListLabel->setStyleSheet("font-size: 14px; color: #FFD8A9;");
                workersLayout->addWidget(workerListLabel);

                // Example: Adding worker names (replace with your actual worker list)
                QStringList workerList = {"Alice", "Bob", "Charlie"};
                for (const QString &workerName : workerList) {
                    QLabel *label = new QLabel(workerName);
                    label->setStyleSheet("color: #FDEEDC; background: #E38B29; border-radius: 8px; font-size: 14px; padding: 4px;");
                    workersLayout->addWidget(label);
                }

                // Add vertical spacer at the end to push everything above upwards
                workersLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

                // Ensure mainFrame has a layout if not present
                QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(mainFrame->layout());
                if (!mainLayout) {
                    mainLayout = new QVBoxLayout(mainFrame);
                    mainFrame->setLayout(mainLayout);
                    mainLayout->addWidget(contentFrame);
                    mainLayout->addWidget(workersListFrame);
                }
            }
        }

        // Switch to requestPage
        ui->stackedWidget_2->setCurrentIndex(2);
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
    }else{
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
// for workers page
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
        "   background: #DAA520;"
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
    query.prepare("SELECT WORKER.WORKERID, name, LOCATIONS, TASKNAME, RATING "
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

    int workerId;
    QString locations,taskName,name;
    float rating;

    while (query.next()) {
        workerId = query.value("WORKERID").toInt();
        name = query.value("name").toString();
        locations = query.value("LOCATIONS").toString();
        taskName = query.value("TASKNAME").toString();
        rating = query.value("RATING").toFloat();

        QFrame *card = createWorkerCard(workerId,name, taskName, locations, rating);
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

QFrame* Home::createWorkerCard(int workerId,QString name, QString &taskName,
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

    QLabel *workerLabel = new QLabel(QString("%1 - %2").arg(workerId).arg(name));
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

    // Set fixed height for the card to make it more compact
    card->setFixedHeight(90);

    card->setCursor(Qt::PointingHandCursor);
    card->setProperty("workerId", workerId);
    card->installEventFilter(this);

    return card;
}

void Home::setupWorkerCards()
{
    QSvgWidget *profileIcon = new QSvgWidget(":/new/svgs/Group.svg");
    if (connectToDatabase()) {
        loadAllWorkers();
    } else {
        auto *newLayout = new QHBoxLayout(ui->profile);
        newLayout->addWidget(profileIcon);
        ui->profile->setLayout(newLayout);
    }
}

void Home::setup_request_page(){
    QFrame *mainFrame = ui->stackedWidget_2->widget(2)->findChild<QFrame*>("frame_10");
    if (mainFrame) {
        QSvgWidget *profileIcon = new QSvgWidget(":/new/svgs/Group.svg");
        profileIcon->setFixedSize(58, 58);

        // Find or create profile_2 widget within mainFrame
        QWidget *profileWidget = mainFrame->findChild<QWidget*>("profile_2");
        if (!profileWidget) {
            profileWidget = new QFrame(mainFrame);
            profileWidget->setObjectName("profile_2");
            QHBoxLayout *newLayout = new QHBoxLayout(profileWidget);
            profileWidget->setLayout(newLayout);
            mainFrame->layout()->addWidget(profileWidget);
        }

        QHBoxLayout *profileLayout = qobject_cast<QHBoxLayout *>(profileWidget->layout());
        if (profileLayout) {
            profileLayout->insertWidget(0, profileIcon);
        } else {
            auto *newLayout = new QHBoxLayout(profileWidget);
            newLayout->addWidget(profileIcon);
            profileWidget->setLayout(newLayout);
        }

        // Ensure mainFrame has a layout if not present
        if (!mainFrame->layout()) {
            auto *mainLayout = new QVBoxLayout(mainFrame);
            mainLayout->addWidget(profileWidget); // Adjust based on where profile_2 should be
            mainFrame->setLayout(mainLayout);
        }
        // Check if RequestContent exists, create if not
        QFrame *contentFrame = mainFrame->findChild<QFrame*>("RequestContent");
        if (!contentFrame) {
            contentFrame = new QFrame(mainFrame);
            contentFrame->setObjectName("RequestContent");
            QHBoxLayout *contentLayout = new QHBoxLayout(contentFrame);
            contentFrame->setLayout(contentLayout);
            mainFrame->layout()->addWidget(contentFrame);
        }

        // Setup requestHeader within RequestContent
        QFrame *headerFrame = contentFrame->findChild<QFrame*>("requestHeader");
        if (!headerFrame) {
            headerFrame = new QFrame(contentFrame);
            headerFrame->setObjectName("requestHeader");
            QVBoxLayout *headerLayout = qobject_cast<QVBoxLayout *>(headerFrame->layout());
            if (!headerLayout) {
                headerLayout = new QVBoxLayout(headerFrame);
                headerFrame->setLayout(headerLayout);
            }
            contentFrame->layout()->addWidget(headerFrame);

            // Back button
            QPushButton *backButton = new QPushButton("Back");
            backButton->setObjectName("backButton");
            backButton->setStyleSheet(
                "QPushButton {"
                "   background-color: #E38B29;"
                "   border-radius: 22px;"
                "}"
                "QPushButton:hover {"
                "   background-color: #C37422;"
                "}"
                );
            backButton->setIcon(QIcon(":/new/svgs/back.svg"));
            backButton->setIconSize(QSize(25, 27));
            backButton->setFixedSize(50, 44);
            backButton->setCursor(Qt::PointingHandCursor);
            headerLayout->addWidget(backButton);


            // Title label
            QLabel *titleLabel = new QLabel("Title");
            titleLabel->setObjectName("requestTitle");
            titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #E38B29;");
            titleLabel->setMaximumWidth(500);
            QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
            titleLabel->setSizePolicy(sizePolicy);
            headerLayout->addWidget(titleLabel,0);

            // Date label
            QLabel *dateLabel = new QLabel("Date");
            dateLabel->setObjectName("requestDate");
            dateLabel->setStyleSheet("font-size: 16px; color: #F1A661; opacity: 0.6; padding-left:5px");
            dateLabel->setAlignment(Qt::AlignBottom);
            headerLayout->addWidget(dateLabel,1);

            // Address label
            QLabel *addressLabel = new QLabel("Address");
            addressLabel->setObjectName("requestAddress");
            addressLabel->setStyleSheet("font-size: 16px; color: #F1A661;");
            headerLayout->addWidget(addressLabel,1);

            connect(backButton, &QPushButton::clicked, this, [this]() {
                ui->stackedWidget_2->setCurrentIndex(0);
            });
        }

        // Setup description within RequestContent
        QFrame *descriptionFrame = contentFrame->findChild<QFrame*>("description");
        if (!descriptionFrame) {
            descriptionFrame = new QFrame(contentFrame);
            descriptionFrame->setObjectName("description");
            QVBoxLayout *descLayout = new QVBoxLayout(descriptionFrame);
            descriptionFrame->setLayout(descLayout);
            QTextEdit *descriptionText = new QTextEdit(descriptionFrame);
            descriptionText->setObjectName("descriptionText");
            descriptionText->setReadOnly(true);
            descriptionText->setStyleSheet(
                "background-color: #FFD8A9;"
                "color: #E38B29;"
                "border-radius: 5px;"
                "font-size: 14px;"
                );
            descLayout->addWidget(descriptionText);
            contentFrame->layout()->addWidget(descriptionFrame);
        }

        // Setup tasks within RequestContent
        QFrame *tasksFrame = contentFrame->findChild<QFrame*>("tasks");
        if (!tasksFrame) {
            tasksFrame = new QFrame(contentFrame);
            tasksFrame->setObjectName("tasks");
            QVBoxLayout *tasksLayout = new QVBoxLayout(tasksFrame);
            tasksFrame->setLayout(tasksLayout);
            QLabel *tasksLabel = new QLabel("Tasks");
            tasksLabel->setObjectName("tasksLabel");
            tasksLabel->setStyleSheet("font-size: 14px; color: #FFD8A9;");
            tasksLayout->addWidget(tasksLabel);
            contentFrame->layout()->addWidget(tasksFrame);
        }

        QFrame *workersListFrame = mainFrame->findChild<QFrame*>("workersListFrame");
        if (!workersListFrame) {
            workersListFrame = new QFrame(mainFrame);
            workersListFrame->setObjectName("workersListFrame");

            QVBoxLayout *workersLayout = new QVBoxLayout(workersListFrame);
            workersListFrame->setLayout(workersLayout);

            QLabel *workerListLabel = new QLabel("Workers");
            workerListLabel->setObjectName("workerList");
            workerListLabel->setStyleSheet("font-size: 14px; color: #FFD8A9;");
            workersLayout->addWidget(workerListLabel);
            workersLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

            QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(mainFrame->layout());
            if (!mainLayout) {
                mainLayout = new QVBoxLayout(mainFrame);
                mainFrame->setLayout(mainLayout);
                mainLayout->addWidget(contentFrame);
                mainLayout->addWidget(workersListFrame);
            }
        }

    }
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// for worker page

QFrame* Home::createClientCardForWorker(int clientId, QString name, QString &feedback, const float &rating) {
    QFrame *card = new QFrame();
    card->setObjectName(QString("workerCard_%1").arg(clientId));
    card->setFrameShape(QFrame::StyledPanel);

    card->setStyleSheet(
        "QFrame {"
        "   background-color: transparent;"
        "   border: 2px solid #DAA520;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "   max-height: 200px;"
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

    QHBoxLayout *topRowLayout = new QHBoxLayout();
    QHBoxLayout *leftGroupLayout = new QHBoxLayout();

    QLabel *workerLabel = new QLabel(QString("%1 - %2").arg(clientId).arg(name), card);
    workerLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 30px;"
        "   color: #E38B29;"
        "   border: none;"
        "}"
        );

    leftGroupLayout->addWidget(workerLabel);
    leftGroupLayout->addStretch();

    QLabel *ratingLabel;
    if (rating == NULL){
        ratingLabel = new QLabel(QString("Rating: NULL"));
        ratingLabel->setStyleSheet(
            "QLabel {"
            "   font-weight: bold;"
            "   font-size: 30px;"
            "   color: #E38B29;"
            "   border: none;"
            "}"
            );
    }
    else{
        ratingLabel = new QLabel(QString("Rating: %1").arg(rating, 0, 'f', 1), card);
        ratingLabel->setStyleSheet(
            "QLabel {"
            "   font-weight: bold;"
            "   font-size: 30px;"
            "   color: #E38B29;"
            "   border: none;"
            "}"
            );
    }

    topRowLayout->addLayout(leftGroupLayout);
    topRowLayout->addWidget(ratingLabel);

    QHBoxLayout *reviewLayout = new QHBoxLayout();

    // Add "Review:" label
    QLabel *reviewLabel = new QLabel("Review:", card);
    reviewLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 25px;"
        "   color: #E38B29;"
        "   margin-top: 8px;"
        "   border: none;"
        "   font-weight: bold;"  // Make "Review:" bold to distinguish it
        "}"
        );

    // Add feedback label
    QLabel *feedbackLabel = new QLabel(feedback, card);
    feedbackLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 20px;"
        "   color: #E38B29;"
        "   margin-top: -15px;"
        "   border: none;"
        "   margin-left: 5px;"
        "}"
        );
    feedbackLabel->setWordWrap(true);

    // Add both labels to the horizontal layout
    reviewLayout->addWidget(reviewLabel);
    reviewLayout->addWidget(feedbackLabel, 1);

    cardLayout->addLayout(topRowLayout);
    cardLayout->addLayout(reviewLayout);
    cardLayout->addWidget(feedbackLabel);

    card->setFixedHeight(90);
    return card;
}

void Home::loadAllClients(int workerId) {
    // Clear existing content first
    QFrame *contentFrame = ui->workersContent->findChild<QFrame*>("workersContent");
    if (contentFrame) {
        QLayout *layout = contentFrame->layout();
        if (layout) {
            QLayoutItem *item;
            while ((item = layout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
        }
    } else {
        contentFrame = new QFrame(ui->workersContent);
        contentFrame->setObjectName("workersContent");
        QVBoxLayout *contentLayout = new QVBoxLayout(contentFrame);
        contentFrame->setLayout(contentLayout);
        ui->workersContent->layout()->addWidget(contentFrame);
    }

    QScrollArea *scrollArea = new QScrollArea(contentFrame);
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
        "   background: #DAA520;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "}"
        );

    QWidget *scrollContent = new QWidget(scrollArea);
    scrollContent->setStyleSheet("QWidget { background-color: transparent; }");
    QVBoxLayout *cardsLayout = new QVBoxLayout(scrollContent);
    cardsLayout->setSpacing(15);

    QSqlQuery query;
    query.prepare("SELECT NAME, CLIENT.CLIENTID, WORKERRATING, FEEDBACKBYCLIENT "
                  "FROM ASSIGNMENT, CLIENT "
                  "WHERE ASSIGNMENT.CLIENTID = CLIENT.CLIENTID AND WORKERID = :workerId "
                  "ORDER BY WORKERRATING");
    query.bindValue(":workerId", workerId);

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch requests: " + query.lastError().text());
        delete scrollContent;
        delete scrollArea;
        return;
    }

    while (query.next()) {
        int clientId = query.value("CLIENT.CLIENTID").toInt();
        QString name = query.value("NAME").toString();
        QString feedback = query.value("FEEDBACKBYCLIENT").toString();
        float rating = query.value("WORKERRATING").toFloat();

        QFrame *card = createClientCardForWorker(clientId, name, feedback, rating);
        cardsLayout->addWidget(card);
    }

    cardsLayout->addStretch();
    scrollContent->setLayout(cardsLayout);
    scrollArea->setWidget(scrollContent);

    QVBoxLayout *frameLayout = qobject_cast<QVBoxLayout*>(contentFrame->layout());
    if (frameLayout) {
        frameLayout->addWidget(scrollArea);
    }
}

void Home::viewWorkerDetails(int workerId) {
    QWidget *workerPage = ui->stackedWidget_2->findChild<QWidget*>("workerPage");
    if (!workerPage) {
        qDebug() << "Error: workerPage not found!";
        return;
    }
    ui->stackedWidget_2->setCurrentWidget(workerPage);

    // Find the existing content frame
    QFrame *workersContent = workerPage->findChild<QFrame*>("workersContent");
    if (!workersContent) {
        qDebug() << "Error: workersContent frame not found!";
        return;
    }

    // Clear existing content
    QLayout *layout = workersContent->layout();
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    } else {
        // Create new layout if none exists
        layout = new QVBoxLayout(workersContent);
        workersContent->setLayout(layout);
    }

    // Fetch worker details
    QSqlQuery query;
    query.prepare("SELECT LOCATIONS, name, TASKNAME, RATING "
                  "FROM WORKER, SPECIALTY, TASK "
                  "WHERE WORKER.WORKERID = SPECIALTY.WORKERID AND "
                  "TASK.TASKID = SPECIALTY.TASKID AND WORKER.WORKERID = :workerId");
    query.bindValue(":workerId", workerId);

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch worker details: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        QString name = query.value("name").toString();
        QString locations = query.value("LOCATIONS").toString();
        QString taskName = query.value("TASKNAME").toString();
        float rating = query.value("RATING").toFloat();

        // Create worker info card
        QFrame *card = new QFrame(workersContent);
        card->setObjectName(QString("workerCard_%1").arg(workerId));
        card->setStyleSheet(
            "QFrame {"
            "   background-color: transparent;"
            "   border: none;"
            "   border-radius: 10px;"
            "   padding: 0px;"
            "   margin: 5px;"
            "}"
            );

        QPushButton *backButton = ui->workersContent->findChild<QPushButton*>("backButton");
        if (!backButton) {
            backButton = new QPushButton;
            backButton->setObjectName("backButton");
            backButton->setStyleSheet(
                "QPushButton {"
                "   background-color: #E38B29;"
                "   border-radius: 5px;"
                "   border: none;"
                "}"
                "QPushButton:hover {"
                "   background-color: #C37422;"
                "}"
                );

            // Use QIcon directly from SVG resource
            backButton->setIcon(QIcon(":/new/svgs/back.svg"));
            backButton->setIconSize(QSize(50, 38));
            backButton->setFixedSize(50, 38);

            // Wrap in a container layout
            QWidget *backContainer = new QWidget;
            QHBoxLayout *backLayout = new QHBoxLayout(backContainer);
            backLayout->setContentsMargins(0, 0, 0, 0);

            // Add to top of main layout
            QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->workersContent->layout());
            if (!mainLayout) {
                mainLayout = new QVBoxLayout(ui->workersContent);
                ui->workersContent->setLayout(mainLayout);
            }
            mainLayout->insertWidget(0, backContainer);

            // Action
            connect(backButton, &QPushButton::clicked, this, [this]() {
                QWidget *workersPage = ui->stackedWidget_2->findChild<QWidget*>("workersPage");
                ui->stackedWidget_2->setCurrentWidget(workersPage);
            });

        }

        QVBoxLayout *cardLayout = new QVBoxLayout(card);

        // Top row with name and rating
        QHBoxLayout *topRow = new QHBoxLayout();
        topRow->setSpacing(20);  // Add space between name, rating, and locations
        topRow->setContentsMargins(0, 0, 0, 0);

        QLabel *nameLabel = new QLabel(QString("%1 - %2").arg(workerId).arg(name), card);
        nameLabel->setStyleSheet(
            "QLabel {"
            "   border: none;"
            "   font-weight: bold;"
            "   font-size: 30px;"
            "   color: #E38B29;"
            "}"
            );

        QString ratingText = QString("<span style='font-size:30px; font-weight:bold;'>Rating: </span>"
                                     "<span style='font-size:25px;'>%1</span>")
                                 .arg(rating, 0, 'f', 1);

        QLabel *ratingLabel = new QLabel(card);
        ratingLabel->setText(ratingText);
        ratingLabel->setStyleSheet(
            "QLabel {"
            "   color: #E38B29;"
            "   border: none;"
            "   margin: 5px;"
            "}"
            );

        QLabel *locationsLabel = new QLabel(card);
        locationsLabel->setText("<span style='font-size:30px; font-weight:bold;'>Locations: </span>"
                                "<span style='font-size:25px;'>" + locations + "</span>");
        locationsLabel->setStyleSheet(
            "QLabel {"
            "   color: #E38B29;"
            "   border: none;"
            "   margin: 5px;"
            "}"
            );

        // Add all widgets to the top row
        topRow->addWidget(backButton);
        topRow->addSpacing(5);
        topRow->addWidget(nameLabel);
        topRow->addSpacing(15);
        topRow->addWidget(ratingLabel);
        topRow->addStretch();              // Push name + rating to the left
        topRow->addWidget(locationsLabel);

        // Add the top row to the card layout
        cardLayout->addLayout(topRow);

        // Specialty row
        QLabel *specialtyLabel = new QLabel(card);
        specialtyLabel->setText("<span style='font-size:30px; font-weight:bold;'>Specialists: </span>"
                                "<span style='font-size:20px;'>" + taskName + "</span>");
        specialtyLabel->setStyleSheet(
            "QLabel {"
            "   color: #E38B29;"
            "   border: none;"
            "   margin: 5px;"
            "}"
            );
        cardLayout->addWidget(specialtyLabel);

        layout->addWidget(card);
    }

    // Load client reviews into the same container
    loadAllClients(workerId);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// for client page

QFrame* Home::createWorkerCardForClient(int workerId, QString name, QString &feedback, const float &rating) {
    QFrame *card = new QFrame();
    card->setObjectName(QString("workerCard_%1").arg(workerId));
    card->setFrameShape(QFrame::StyledPanel);

    card->setStyleSheet(
        "QFrame {"
        "   background-color: transparent;"
        "   border: 2px solid #DAA520;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "   max-height: 200px;"
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

    QHBoxLayout *topRowLayout = new QHBoxLayout();
    QHBoxLayout *leftGroupLayout = new QHBoxLayout();

    QLabel *workerLabel = new QLabel(QString("%1 - %2").arg(workerId).arg(name), card);
    workerLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 30px;"
        "   color: #E38B29;"
        "   border: none;"
        "}"
        );

    leftGroupLayout->addWidget(workerLabel);
    leftGroupLayout->addStretch();

    QLabel *ratingLabel;
    if (rating == NULL){
        ratingLabel = new QLabel(QString("Rating: NULL"));
        ratingLabel->setStyleSheet(
            "QLabel {"
            "   font-weight: bold;"
            "   font-size: 30px;"
            "   color: #E38B29;"
            "   border: none;"
            "}"
            );
    }
    else{
        ratingLabel = new QLabel(QString("Rating: %1").arg(rating, 0, 'f', 1), card);
        ratingLabel->setStyleSheet(
            "QLabel {"
            "   font-weight: bold;"
            "   font-size: 30px;"
            "   color: #E38B29;"
            "   border: none;"
            "}"
            );
    }

    topRowLayout->addLayout(leftGroupLayout);
    topRowLayout->addWidget(ratingLabel);

    QHBoxLayout *reviewLayout = new QHBoxLayout();

    // Add "Review:" label
    QLabel *reviewLabel = new QLabel("Review:", card);
    reviewLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 25px;"
        "   color: #E38B29;"
        "   margin-top: 8px;"
        "   border: none;"
        "   font-weight: bold;"  // Make "Review:" bold to distinguish it
        "}"
        );

    // Add feedback label
    QLabel *feedbackLabel = new QLabel(feedback, card);
    feedbackLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 20px;"
        "   color: #E38B29;"
        "   margin-top: -15px;"
        "   border: none;"
        "   margin-left: 5px;"
        "}"
        );
    feedbackLabel->setWordWrap(true);

    // Add both labels to the horizontal layout
    reviewLayout->addWidget(reviewLabel);
    reviewLayout->addWidget(feedbackLabel, 1);

    cardLayout->addLayout(topRowLayout);
    cardLayout->addLayout(reviewLayout);
    cardLayout->addWidget(feedbackLabel);

    card->setFixedHeight(90);
    return card;
}

void Home::loadAllWorkersinClientPage() {
    // Clear existing content first
    QFrame *contentFrame = ui->workersContent->findChild<QFrame*>("cilentContent");
    if (contentFrame) {
        QLayout *layout = contentFrame->layout();
        if (layout) {
            QLayoutItem *item;
            while ((item = layout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
        }
    } else {
        contentFrame = new QFrame(ui->workersContent);
        contentFrame->setObjectName("cilentContent");
        QVBoxLayout *contentLayout = new QVBoxLayout(contentFrame);
        contentFrame->setLayout(contentLayout);
        ui->workersContent->layout()->addWidget(contentFrame);
    }

    QScrollArea *scrollArea = new QScrollArea(contentFrame);
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
        "   background: #DAA520;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "}"
        );

    QWidget *scrollContent = new QWidget(scrollArea);
    scrollContent->setStyleSheet("QWidget { background-color: transparent; }");
    QVBoxLayout *cardsLayout = new QVBoxLayout(scrollContent);
    cardsLayout->setSpacing(15);

    QSqlQuery query;
    query.prepare("SELECT WORKER.NAME, WORKER.WORKERID, CLIENTRATING, FEEDBACKBYWORKER "
                  "FROM ASSIGNMENT, WORKER "
                  "WHERE ASSIGNMENT.WORKERID = WORKER.WORKERID AND clientId = :clientId "
                  "ORDER BY CLIENTRATING");
    query.bindValue(":clientId", clientData.id);

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch requests: " + query.lastError().text());
        delete scrollContent;
        delete scrollArea;
        return;
    }

    while (query.next()) {
        int workerId = query.value("WORKER.WORKERID").toInt();
        QString name = query.value("WORKER.NAME").toString();
        QString feedback = query.value("FEEDBACKBYWORKER").toString();
        float rating = query.value("CLIENTRATING").toFloat();

        QFrame *card = createWorkerCardForClient(workerId, name, feedback, rating);
        cardsLayout->addWidget(card);
    }

    cardsLayout->addStretch();
    scrollContent->setLayout(cardsLayout);
    scrollArea->setWidget(scrollContent);

    QVBoxLayout *frameLayout = qobject_cast<QVBoxLayout*>(contentFrame->layout());
    if (frameLayout) {
        frameLayout->addWidget(scrollArea);
    }
}

void Home::on_requests_clicked()
{
    QWidget *workerPage = ui->stackedWidget_2->findChild<QWidget*>("workerPage");
    if (!workerPage) {
        qDebug() << "Error: workerPage not found!";
        return;
    }
    ui->stackedWidget_2->setCurrentWidget(workerPage);

    // Find the existing content frame
    QFrame *workersContent = workerPage->findChild<QFrame*>("workersContent");
    if (!workersContent) {
        qDebug() << "Error: workersContent frame not found!";
        return;
    }

    // Clear existing content
    QLayout *layout = workersContent->layout();
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    } else {
        // Create new layout if none exists
        layout = new QVBoxLayout(workersContent);
        workersContent->setLayout(layout);
    }

    // Create worker info card
    QFrame *card = new QFrame(ui->clientContent);
    card->setObjectName(QString("workerCard_%1").arg(clientData.id));
    card->setStyleSheet(
        "QFrame {"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 10px;"
        "   padding: 0px;"
        "   margin: 5px;"
        "}"
        );

    QPushButton *backButton = ui->workersContent->findChild<QPushButton*>("backButton");
    if (!backButton) {
        backButton = new QPushButton;
        backButton->setObjectName("backButton");
        backButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #E38B29;"
            "   border-radius: 5px;"
            "   border: none;"
            "}"
            "QPushButton:hover {"
            "   background-color: #C37422;"
            "}"
            );

        // Use QIcon directly from SVG resource
        backButton->setIcon(QIcon(":/new/svgs/back.svg"));
        backButton->setIconSize(QSize(50, 38));
        backButton->setFixedSize(50, 38);

        // Wrap in a container layout
        QWidget *backContainer = new QWidget;
        QHBoxLayout *backLayout = new QHBoxLayout(backContainer);
        backLayout->setContentsMargins(0, 0, 0, 0);

        // Add to top of main layout
        QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->workersContent->layout());
        if (!mainLayout) {
            mainLayout = new QVBoxLayout(ui->workersContent);
            ui->workersContent->setLayout(mainLayout);
        }
        mainLayout->insertWidget(0, backContainer);

        // Action
        connect(backButton, &QPushButton::clicked, this, [this]() {
            QWidget *profilePage = ui->stackedWidget_2->findChild<QWidget*>("profilePage");
            ui->stackedWidget_2->setCurrentWidget(profilePage);
        });

    }

    QVBoxLayout *cardLayout = new QVBoxLayout(card);

    // Top row with name and rating
    QHBoxLayout *topRow = new QHBoxLayout();
    topRow->setSpacing(20);  // Add space between name, rating, and locations
    topRow->setContentsMargins(0, 0, 0, 0);

    QLabel *nameLabel = new QLabel(QString("%1 - %2").arg(clientData.id).arg(clientData.name), card);
    nameLabel->setStyleSheet(
        "QLabel {"
        "   border: none;"
        "   font-weight: bold;"
        "   font-size: 30px;"
        "   color: #E38B29;"
        "}"
        );

    QLabel *locationsLabel = new QLabel(card);
    locationsLabel->setText("<span style='font-size:30px; font-weight:bold;'>Locations: </span>"
                            "<span style='font-size:25px;'>" + clientData.address + "</span>");
    locationsLabel->setStyleSheet(
        "QLabel {"
        "   color: #E38B29;"
        "   border: none;"
        "   margin: 5px;"
        "}"
        );

    // Add all widgets to the top row
    topRow->addWidget(backButton);
    topRow->addSpacing(5);
    topRow->addWidget(nameLabel);
    topRow->addStretch();              // Push name + rating to the left
    topRow->addWidget(locationsLabel);

    // Add the top row to the card layout
    cardLayout->addLayout(topRow);

    // Specialty row
    QLabel *specialtyLabel = new QLabel(card);
    specialtyLabel->setText("<span style='font-size:30px; font-weight:bold;'>Feedback: </span>"
                            "<span style='font-size:20px;'>" + clientData.feedback + "</span>");
    specialtyLabel->setStyleSheet(
        "QLabel {"
        "   color: #E38B29;"
        "   border: none;"
        "   margin: 5px;"
        "}"
        );
    cardLayout->addWidget(specialtyLabel);

    layout->addWidget(card);

    // Load client reviews into the same container
    loadAllWorkersinClientPage();
    loadDataInProfile();
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

Home::Home(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Home)
    , requestTask(nullptr)
{
    ui->setupUi(this);
    // Setup profile icon in requests
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
        filtersLayout->insertWidget(0, filtersSvg,0,Qt::AlignHCenter);
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

    // Setup profile icon in worker
    QSvgWidget *profileIcon3 = new QSvgWidget(":/new/svgs/Group.svg");
    profileIcon3->setFixedSize(58, 58);
    QHBoxLayout *profileLayout3 = qobject_cast<QHBoxLayout *>(ui->profile_6->layout());
    if (profileLayout3) {
        profileLayout3->insertWidget(0, profileIcon3);
    } else {
        auto *newLayout = new QHBoxLayout(ui->profile_6);
        newLayout->addWidget(profileIcon3);
        ui->profile_6->setLayout(newLayout);
    }

    // Setup profile icon in client
    QSvgWidget *profileIcon4 = new QSvgWidget(":/new/svgs/Group.svg");
    profileIcon4->setFixedSize(58, 58);
    QHBoxLayout *profileLayout4 = qobject_cast<QHBoxLayout *>(ui->profile_7->layout());
    if (profileLayout4) {
        profileLayout4->insertWidget(0, profileIcon4);
    } else {
        auto *newLayout = new QHBoxLayout(ui->profile_7);
        newLayout->addWidget(profileIcon4);
        ui->profile_7->setLayout(newLayout);
    }

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
    // Setup profile icon
    QSvgWidget *profileIcon1 = new QSvgWidget(":/new/svgs/Group.svg");
    profileIcon1->setFixedSize(58, 58);
    QHBoxLayout *profileLayout1 = qobject_cast<QHBoxLayout *>(ui->profile_3->layout());
    if (profileLayout1) {
        profileLayout1->insertWidget(0, profileIcon1);
    } else {
        auto *newLayout = new QHBoxLayout(ui->profile_3);
        newLayout->addWidget(profileIcon1);
        ui->profile_3->setLayout(newLayout);
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

    // Setup profile icon in profile
    QSvgWidget *profileIcon2 = new QSvgWidget(":/new/svgs/Group.svg");
    profileIcon2->setFixedSize(58, 58);
    QHBoxLayout *profileLayout2 = qobject_cast<QHBoxLayout *>(ui->profile_4->layout());
    if (profileLayout2) {
        profileLayout2->insertWidget(0, profileIcon2);
    } else {
        auto *newLayout = new QHBoxLayout(ui->profile_4);
        newLayout->addWidget(profileIcon2);
        ui->profile_2->setLayout(newLayout);
    }

    setup_request_page();
    ui->stackedWidget_2->setCurrentIndex(0);
}

Home::~Home()
{
    if (startDateCalendar) {
        startDateCalendar->deleteLater();
        startDateCalendar = nullptr;
    }

    if (endDateCalendar) {
        endDateCalendar->deleteLater();
        endDateCalendar = nullptr;
    }

    ui->name->setEnabled(false);
    ui->phone->setEnabled(false);
    ui->password->setEnabled(false);
    ui->email->setEnabled(false);
    ui->address->setEnabled(false);

    ui->update->setEnabled(false);

    delete ui;
    delete requestTask;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// for buttons

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
    requestTask->setClient( clientData.id, clientData.name, clientData.password, clientData.address, clientData.email, clientData.phone,clientData.feedback);
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

}

void Home::on_filterName_clicked()
{

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

void Home::on_startDate_3_clicked()
{

}

void Home::on_endDate_3_clicked()
{

}



void Home::on_requestsPageBtn_4_clicked()
{
    QWidget *requestsPage = ui->stackedWidget_2->findChild<QWidget*>("requestsPage");

    if (requestsPage) {
        ui->stackedWidget_2->setCurrentWidget(requestsPage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}

void Home::on_workersPageBtn_4_clicked()
{
    QWidget *workersPage = ui->stackedWidget_2->findChild<QWidget*>("workersPage");

    if (workersPage) {
        ui->stackedWidget_2->setCurrentWidget(workersPage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}

void Home::on_workersPageBtn_3_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
}
void Home::loadDataInProfile() {
    QSqlQuery query;
    query.prepare("SELECT * FROM client WHERE ClientID = :clientId");
    query.bindValue(":clientId", clientData.id);

    if (!query.exec()) {
        qDebug() << "Query error:" << query.lastError().text();
        return;
    }
    if (!query.next()) {
        qDebug() << "No client found with ID:" << clientData.id;
        return;
    }

    clientData.name = query.value("name").toString();
    clientData.address = query.value("address").toString();
    clientData.feedback = query.value("overAllFeedback").toString();
    clientData.phone = query.value("phone").toString();
    clientData.email = query.value("email").toString();
    clientData.password = query.value("password").toString();

    ui->name->setText(clientData.name);
    ui->phone->setText(clientData.phone);
    ui->password->setText(clientData.password);
    ui->email->setText(clientData.email);
    ui->address->setText(clientData.address);
    ui->feedback->setText(clientData.feedback);

    // put data into profile;
    ui->pushButton_2->setText(clientData.name);
    ui->pushButton_3->setText(clientData.name);
    ui->pushButton_4->setText(clientData.name);
    ui->pushButton_5->setText(clientData.name);
    ui->pushButton_6->setText(clientData.name);
}

// for profile page
void Home::on_pushButton_5_clicked()
{
    QWidget *profilePage = ui->stackedWidget_2->findChild<QWidget*>("profilePage");

    if (profilePage) {
        ui->stackedWidget_2->setCurrentWidget(profilePage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}

void Home::on_pushButton_4_clicked()
{
    QWidget *profilePage = ui->stackedWidget_2->findChild<QWidget*>("profilePage");

    if (profilePage) {
        ui->stackedWidget_2->setCurrentWidget(profilePage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}

void Home::on_pushButton_2_clicked()
{
    QWidget *profilePage = ui->stackedWidget_2->findChild<QWidget*>("profilePage");

    if (profilePage) {
        ui->stackedWidget_2->setCurrentWidget(profilePage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}

void Home::on_pushButton_3_clicked()
{
    QWidget *profilePage = ui->stackedWidget_2->findChild<QWidget*>("profilePage");

    if (profilePage) {
        ui->stackedWidget_2->setCurrentWidget(profilePage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}

void Home::on_edit_clicked()
{
    ui->name->setEnabled(true);
    ui->phone->setEnabled(true);
    ui->password->setEnabled(true);
    ui->email->setEnabled(true);
    ui->address->setEnabled(true);

    ui->update->setEnabled(true);
}

void Home::on_update_clicked()
{
    int clientId = clientData.id;

    QString name,phone,password,email,address;
    name = ui->name->text();
    phone = ui->phone->text();
    password = ui->password->text();
    email = ui->email->text();
    address = ui->address->text();

    if (name.isEmpty() || phone.isEmpty() ||password.isEmpty() ||email.isEmpty() ||address.isEmpty()){
        QMessageBox::information(this, "Error", "Feilds is empty!");
        return;
    }

    QSqlQuery query2;
    query2.prepare("SELECT email FROM client WHERE email = :email AND clientID != :currentId");
    query2.bindValue(":email", email);
    query2.bindValue(":currentId", clientData.id);

    if (!query2.exec()) {
        QMessageBox::critical(this, "Error", "Database error: " + query2.lastError().text());
        return;
    }

    if (query2.next()) {
        QMessageBox::warning(this, "Email Exists",
                             "This email is already registered by another account.");
        return;
    }

    // Email format
    QRegularExpression emailRegex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    if (!emailRegex.match(email).hasMatch() || email.isEmpty()) {
        QMessageBox::warning(this, "Invalid Email", "Please enter a valid email address.");
        return;
    }

    // Phone must be digits only
    QRegularExpression phoneRegex(R"(^\d+$)");
    if (!phoneRegex.match(phone).hasMatch() || phone.isEmpty()) {
        QMessageBox::warning(this, "Invalid Phone", "Phone number must contain digits only.");
        return;
    }

    QSqlQuery query;
    query.prepare("UPDATE client SET name = :name , phone = :phone , address = :address , email = :email , password = :password WHERE clientId = :clientId");
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    query.bindValue(":password", password);
    query.bindValue(":email", email);
    query.bindValue(":address", address);
    query.bindValue(":clientId", clientId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Update failed: " + query.lastError().text());
        return;
    } else {
        QMessageBox::information(this, "Success", "Updated successfully!");
    }

    ui->name->setEnabled(false);
    ui->phone->setEnabled(false);
    ui->password->setEnabled(false);
    ui->email->setEnabled(false);
    ui->address->setEnabled(false);

    ui->update->setEnabled(false);
}

void Home::on_requestsPageBtn_6_clicked()
{
    QWidget *requestsPage = ui->stackedWidget_2->findChild<QWidget*>("requestsPage");

    if (requestsPage) {
        ui->stackedWidget_2->setCurrentWidget(requestsPage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}

void Home::on_workersPageBtn_6_clicked()
{
    QWidget *workersPage = ui->stackedWidget_2->findChild<QWidget*>("workersPage");

    if (workersPage) {
        ui->stackedWidget_2->setCurrentWidget(workersPage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}

void Home::on_pushButton_6_clicked()
{
    QWidget *profilePage = ui->stackedWidget_2->findChild<QWidget*>("profilePage");

    if (profilePage) {
        ui->stackedWidget_2->setCurrentWidget(profilePage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}


void Home::on_workersPageBtn_7_clicked()
{
    QWidget *workersPage = ui->stackedWidget_2->findChild<QWidget*>("workersPage");

    if (workersPage) {
        ui->stackedWidget_2->setCurrentWidget(workersPage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}


void Home::on_requestsPageBtn_7_clicked()
{
    QWidget *requestsPage = ui->stackedWidget_2->findChild<QWidget*>("requestsPage");

    if (requestsPage) {
        ui->stackedWidget_2->setCurrentWidget(requestsPage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}


void Home::on_pushButton_7_clicked()
{
    QWidget *profilePage = ui->stackedWidget_2->findChild<QWidget*>("profilePage");

    if (profilePage) {
        ui->stackedWidget_2->setCurrentWidget(profilePage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}

void Home::on_startDate_clicked()
{
    if (startDateCalendar) {
        startDateCalendar->deleteLater();
    }

    startDateCalendar = new QCalendarWidget();
    startDateCalendar->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    startDateCalendar->setStyleSheet(
        "QCalendarWidget {"
        "   background-color: #FDEEDC;"
        "   color: #F1A661;"
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

    QPoint pos = ui->startDate->mapToGlobal(QPoint(0, ui->startDate->height()));
    startDateCalendar->move(pos);

    connect(startDateCalendar, &QCalendarWidget::clicked, this, &Home::onStartDateSelected);
    connect(startDateCalendar, &QCalendarWidget::clicked, startDateCalendar, &QCalendarWidget::deleteLater);
    connect(startDateCalendar, &QCalendarWidget::destroyed, [this]() { startDateCalendar = nullptr; });
    startDateCalendar->show();
}


void Home::on_endDate_clicked()
{
    if (endDateCalendar) {
        endDateCalendar->deleteLater();
    }

    endDateCalendar = new QCalendarWidget();
    endDateCalendar->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    endDateCalendar->setStyleSheet(
        "QCalendarWidget {"
        "   background-color: #FDEEDC;"
        "   color: #F1A661;"
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

    QPoint pos = ui->endDate->mapToGlobal(QPoint(0, ui->endDate->height()));
    endDateCalendar->move(pos);

    connect(endDateCalendar, &QCalendarWidget::clicked, this, &Home::onEndDateSelected);
    connect(endDateCalendar, &QCalendarWidget::clicked, endDateCalendar, &QCalendarWidget::deleteLater);
    connect(endDateCalendar, &QCalendarWidget::destroyed, [this]() { endDateCalendar = nullptr; });

    endDateCalendar->show();
}


void Home::on_requestsPageBtn_3_clicked()
{

}

