#include "../Headers/home.h"
#include "ui_home.h"
#include <QSvgWidget>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include "ui_requesttask.h"
#include "../Headers/signup.h"

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
    setupRequestCards();
}
bool Home::connectToDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    QString serverName = ".";
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
    QSqlQuery query;
    query.prepare("SELECT taskName FROM task");
    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch tasks: " + query.lastError().text());
        return false;
    }

    while (query.next()) {
        QString task = query.value("taskName").toString();
        ui->comboBox_edit->addItem(task);
    }
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
        "SELECT r.ClientID, r.RequestID, t.TaskName, r.RequestTime, a.Status "
        "FROM Request r "
        "LEFT JOIN Assignment a ON r.RequestID = a.RequestID "
        "JOIN Task t ON r.TaskID = t.TaskID "
        "WHERE r.RequestTime >= :startDate AND r.RequestTime < :endDate AND r.CLIENTID = :clientid "
        "ORDER BY r.RequestTime"
        );
    query.bindValue(":startDate", startDateValue.isValid() ? startDateValue : QDate(2000, 1, 1));
    query.bindValue(":endDate", endDateValue.isValid() ? endDateValue.addDays(1) : QDate(2100, 12, 31));
    query.bindValue(":clientid",clientData.id);
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
        qDebug() << clientId;
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
        else if (card && card->objectName().startsWith("taskCard_")){
            QString taskName = card->property("taskName").toString();
            int taskId = card->property("taskId").toInt();
            goToRequestPage(taskId,taskName);
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}


void Home::viewRequestDetails(int requestId)
{
    currentRequestId = requestId;
    // Query request details with client name
    QSqlQuery query;
    query.prepare(
        "SELECT r.CLIENTID, c.NAME AS CLIENTNAME, r.TASKID, t.TASKNAME, r.ADDRESS, "
        "r.REQUESTTIME, r.REQUESTDESCRIPTION, r.PREFERREDTIMESLOT "
        "FROM REQUEST r "
        "JOIN TASK t ON r.TASKID = t.TASKID "
        "JOIN CLIENT c ON r.CLIENTID = c.CLIENTID "
        "WHERE r.REQUESTID = :id"
        );
    query.bindValue(":id", requestId);

    if (!query.exec()) {
        qDebug() << "Condition met: !query.exec() - Query error:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch request details: " + query.lastError().text());
        return;
    } else {
        qDebug() << "Condition met: query.exec() - Query executed successfully for request ID:" << requestId;
    }

    // Find RequestPage by object name
    QWidget *requestPage = ui->stackedWidget_2->findChild<QWidget*>("RequestPage");
    if (!requestPage) {
        qDebug() << "Condition met: !requestPage - Error: RequestPage not found in stackedWidget_2!";
        QMessageBox::critical(this, "UI Error", "RequestPage not found.");
        return;
    } else {
        qDebug() << "Condition met: requestPage - RequestPage found.";
    }

    if (query.next()) {
        qDebug() << "Condition met: query.next() - Request details found for ID:" << requestId;
        QString clientName = query.value("CLIENTNAME").toString();
        QString taskName = query.value("TASKNAME").toString();
        QString address = query.value("ADDRESS").toString();
        QDateTime requestTime = query.value("REQUESTTIME").toDateTime();
        requestTime.setTimeZone(QTimeZone("Europe/Athens")); // Adjust for EEST
        QString description = query.value("REQUESTDESCRIPTION").toString();
        QDateTime preferredTimeSlot = query.value("PREFERREDTIMESLOT").toDateTime();

        QString dateString = requestTime.isValid() ? QLocale().toString(requestTime, "yyyy-MM-dd hh:mm ap") : "Unknown";
        if (requestTime.isValid()) {
            qDebug() << "Condition met: requestTime.isValid() - Request time is valid.";
        } else {
            qDebug() << "Condition met: !requestTime.isValid() - Request time is invalid.";
        }


        QFrame *mainFrame = requestPage->findChild<QFrame*>("frame_10");
        if (!mainFrame) {
            qDebug() << "Condition met: !mainFrame - Error: mainFrame (frame_10) not found on RequestPage.";
            QMessageBox::critical(this, "UI Error", "Main frame not found.");
            return;
        } else {
            qDebug() << "Condition met: mainFrame - mainFrame (frame_10) found.";
        }

        // Reuse existing layout or create a new one
        QHBoxLayout *mainLayout = qobject_cast<QHBoxLayout*>(mainFrame->findChild<QFrame*>("Content")->layout());
        if (mainLayout) {
            qDebug() << "Condition met: mainLayout - Existing mainLayout found.";
        } else {
            qDebug() << "Condition met: !mainLayout - mainLayout not found or not QHBoxLayout.";
        }


        QFrame *contentFrame = mainFrame->findChild<QFrame*>("RequestContent");
        if (contentFrame) {
            qDebug() << "Condition met: contentFrame - RequestContent frame found.";
            // Header frame
            QFrame *headerFrame = contentFrame->findChild<QFrame*>("requestHeader");
            if (headerFrame) {
                qDebug() << "Condition met: headerFrame - requestHeader frame found.";
                QHBoxLayout *headerLayout = qobject_cast<QHBoxLayout*>(headerFrame->layout());
                if (!headerLayout) {
                    qDebug() << "Condition met: !headerLayout - headerLayout not found for requestHeader. Creating new QHBoxLayout.";
                    headerLayout = new QHBoxLayout(headerFrame);
                    headerFrame->setLayout(headerLayout);
                } else {
                    qDebug() << "Condition met: headerLayout - Existing headerLayout found.";
                }

                // Back button
                QPushButton *backButton = headerFrame->findChild<QPushButton*>("backButton");
                if (!backButton) {
                    qDebug() << "Condition met: !backButton - backButton not found in requestHeader. Creating new QPushButton.";
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
                        ui->stackedWidget_2->setCurrentWidget(ui->stackedWidget_2->findChild<QWidget*>("requestsPage"));
                    });
                } else {
                    qDebug() << "Condition met: backButton - Existing backButton found.";
                }


                // Title and date frame
                QFrame *titleDateFrame = headerFrame->findChild<QFrame*>("titleDateFrame");
                QHBoxLayout *titleDateLayout;
                if (!titleDateFrame) {
                    qDebug() << "Condition met: !titleDateFrame - titleDateFrame not found in requestHeader. Creating new QFrame.";
                    titleDateFrame = new QFrame(headerFrame);
                    titleDateFrame->setObjectName("titleDateFrame");
                    titleDateLayout = new QHBoxLayout(titleDateFrame);
                    titleDateFrame->setLayout(titleDateLayout);
                    titleDateLayout->setSpacing(0);
                    if (headerLayout) { // Check if headerLayout is valid before adding
                        qDebug() << "Condition met: headerLayout (adding titleDateFrame)";
                        headerLayout->addWidget(titleDateFrame, 0);
                    } else {
                        qDebug() << "Condition met: !headerLayout (adding titleDateFrame)";
                        // Handle error or create headerLayout if it was null
                    }
                } else {
                    qDebug() << "Condition met: titleDateFrame - Existing titleDateFrame found.";
                }
                titleDateLayout = qobject_cast<QHBoxLayout*>(titleDateFrame->layout());
                if (!titleDateLayout) {
                    qDebug() << "Condition met: !titleDateLayout (after qobject_cast) - titleDateLayout not found. Creating new QHBoxLayout.";
                    titleDateLayout = new QHBoxLayout(titleDateFrame);
                    titleDateFrame->setLayout(titleDateLayout);
                } else {
                    qDebug() << "Condition met: titleDateLayout (after qobject_cast) - Existing titleDateLayout found.";
                }
                // Request title
                QLabel *titleLabel = titleDateFrame->findChild<QLabel*>("requestTitle");
                if (!titleLabel) {
                    qDebug() << "Condition met: !titleLabel - requestTitle label not found in titleDateFrame. Creating new QLabel.";
                    titleLabel = new QLabel(titleDateFrame);
                    titleLabel->setObjectName("requestTitle");
                    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #E38B29;");
                    titleLabel->setMaximumWidth(500);
                    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
                    titleLabel->setSizePolicy(sizePolicy);
                    if (titleDateLayout) { // Check if layout is valid before adding
                        qDebug() << "Condition met: titleDateLayout (adding titleLabel)";
                        titleDateLayout->addWidget(titleLabel, 0);
                    } else {
                        qDebug() << "Condition met: !titleDateLayout (adding titleLabel)";
                        // Handle error
                    }
                } else {
                    qDebug() << "Condition met: titleLabel - Existing requestTitle label found.";
                }
                titleLabel->setText(taskName);

                // Request date
                QLabel *dateLabel = titleDateFrame->findChild<QLabel*>("requestDate");
                if (!dateLabel) {
                    qDebug() << "Condition met: !dateLabel - requestDate label not found in titleDateFrame. Creating new QLabel.";
                    dateLabel = new QLabel(titleDateFrame);
                    dateLabel->setObjectName("requestDate");
                    dateLabel->setStyleSheet("font-size: 12px; color: #F1A661; opacity: 0.3; padding-left:2px;padding-top:15px;");
                    dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
                    if (titleDateLayout) { // Check if layout is valid before adding
                        qDebug() << "Condition met: titleDateLayout (adding dateLabel)";
                        titleDateLayout->addWidget(dateLabel, 1);
                    } else {
                        qDebug() << "Condition met: !titleDateLayout (adding dateLabel)";
                        // Handle error
                    }
                } else {
                    qDebug() << "Condition met: dateLabel - Existing requestDate label found.";
                }
                dateLabel->setText(dateString);

                // Client name
                QLabel *clientLabel = headerFrame->findChild<QLabel*>("clientName");
                if (!clientLabel) {
                    qDebug() << "Condition met: !clientLabel - clientName label not found in headerFrame. Creating new QLabel.";
                    clientLabel = new QLabel(headerFrame);
                    clientLabel->setObjectName("clientName");
                    clientLabel->setStyleSheet("font-size: 16px; color: #F1A661;");
                    if (headerLayout) { // Check if layout is valid before adding
                        qDebug() << "Condition met: headerLayout (adding clientLabel)";
                        headerLayout->addWidget(clientLabel);
                    } else {
                        qDebug() << "Condition met: !headerLayout (adding clientLabel)";
                        // Handle error
                    }
                    clientLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
                } else {
                    qDebug() << "Condition met: clientLabel - Existing clientName label found.";
                }
                clientLabel->setText("Client: " + clientName);

                // Address
                QLabel *addressLabel = headerFrame->findChild<QLabel*>("requestAddress");
                if (!addressLabel) {
                    qDebug() << "Condition met: !addressLabel - requestAddress label not found in headerFrame. Creating new QLabel.";
                    addressLabel = new QLabel(headerFrame);
                    addressLabel->setObjectName("requestAddress");
                    addressLabel->setStyleSheet("font-size: 16px; color: #F1A661;");
                    if (headerLayout) { // Check if layout is valid before adding
                        qDebug() << "Condition met: headerLayout (adding addressLabel)";
                        headerLayout->addWidget(addressLabel);
                    } else {
                        qDebug() << "Condition met: !headerLayout (adding addressLabel)";
                        // Handle error
                    }
                    addressLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
                } else {
                    qDebug() << "Condition met: addressLabel - Existing requestAddress label found.";
                }
                addressLabel->setText(address);
            } else {
                qDebug() << "Condition met: !headerFrame - Error: requestHeader frame not found.";
            }

            // Tasks frame
            QFrame *tasksFrame = contentFrame->findChild<QFrame*>("tasks");
            if (tasksFrame) {
                qDebug() << "Condition met: tasksFrame - tasksFrame found.";
                QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(tasksFrame->layout());
                if (!layout) {
                    qDebug() << "Condition met: !layout - Layout not found for tasksFrame. Creating new QHBoxLayout.";
                    layout = new QHBoxLayout(tasksFrame);
                    tasksFrame->setLayout(layout);
                } else {
                    qDebug() << "Condition met: layout - Existing layout found for tasksFrame.";
                }

                QLabel *taskTitleLabel = tasksFrame->findChild<QLabel*>("taskTitleLabel");
                if (!taskTitleLabel) {
                    qDebug() << "Condition met: !taskTitleLabel - taskTitleLabel not found in tasksFrame. Creating new QLabel.";
                    taskTitleLabel = new QLabel(tasksFrame);
                    taskTitleLabel->setObjectName("taskTitleLabel");
                    taskTitleLabel->setStyleSheet("font-weight: bold; font-size: 20px; color: #E38B29;");
                    if (layout) { // Check if layout is valid before adding
                        qDebug() << "Condition met: layout (inserting taskTitleLabel)";
                        layout->insertWidget(0, taskTitleLabel);
                    } else {
                        qDebug() << "Condition met: !layout (inserting taskTitleLabel)";
                        // Handle error
                    }
                } else {
                    qDebug() << "Condition met: taskTitleLabel - Existing taskTitleLabel found.";
                }
                taskTitleLabel->setText("Tasks ");

                QLabel *tasksLabel = tasksFrame->findChild<QLabel*>("tasksLabel");
                if (!tasksLabel) {
                    qDebug() << "Condition met: !tasksLabel - tasksLabel not found in tasksFrame. Creating new QLabel.";
                    tasksLabel = new QLabel(tasksFrame);
                    tasksLabel->setObjectName("tasksLabel");
                    tasksLabel->setStyleSheet("font-size: 16px; color: #E38B29;");
                    tasksLabel->setAlignment(Qt::AlignVCenter);
                    if (layout) { // Check if layout is valid before adding
                        qDebug() << "Condition met: layout (adding tasksLabel)";
                        layout->addWidget(tasksLabel);
                    } else {
                        qDebug() << "Condition met: !layout (adding tasksLabel)";
                        // Handle error
                    }
                } else {
                    qDebug() << "Condition met: tasksLabel - Existing tasksLabel found.";
                }
                tasksLabel->setText(taskName);
            } else {
                qDebug() << "Condition met: !tasksFrame - Error: tasksFrame not found.";
            }

            // Description frame
            QFrame *descriptionFrame = contentFrame->findChild<QFrame*>("description");
            if (descriptionFrame) {
                qDebug() << "Condition met: descriptionFrame - descriptionFrame found.";
                QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(descriptionFrame->layout());
                if (!layout) {
                    qDebug() << "Condition met: !layout - Layout not found for descriptionFrame. Creating new QVBoxLayout.";
                    layout = new QVBoxLayout(descriptionFrame);
                    descriptionFrame->setLayout(layout);
                } else {
                    qDebug() << "Condition met: layout - Existing layout found for descriptionFrame.";
                }

                QLabel *descriptionTitle = descriptionFrame->findChild<QLabel*>("descriptionTitle");
                if (!descriptionTitle) {
                    qDebug() << "Condition met: !descriptionTitle - descriptionTitle label not found in descriptionFrame. Creating new QLabel.";
                    descriptionTitle = new QLabel("Description", descriptionFrame);
                    descriptionTitle->setObjectName("descriptionTitle");
                    descriptionTitle->setStyleSheet(
                        "color: #E38B29;"
                        "font-size: 20px;"
                        "font-weight: bold;"
                        );
                    if (layout) { // Check if layout is valid before adding
                        qDebug() << "Condition met: layout (adding descriptionTitle)";
                        layout->addWidget(descriptionTitle);
                    } else {
                        qDebug() << "Condition met: !layout (adding descriptionTitle)";
                        // Handle error
                    }
                } else {
                    qDebug() << "Condition met: descriptionTitle - Existing descriptionTitle label found.";
                }

                QTextEdit *descriptionText = descriptionFrame->findChild<QTextEdit*>("descriptionText");
                if (!descriptionText) {
                    qDebug() << "Condition met: !descriptionText - descriptionText QTextEdit not found in descriptionFrame. Creating new QTextEdit.";
                    descriptionText = new QTextEdit(descriptionFrame);
                    descriptionText->setObjectName("descriptionText");
                    descriptionText->setReadOnly(true);
                    descriptionText->setStyleSheet(
                        "background-color: transparent;"
                        "color: #E38B29;"
                        "border-radius: 5px;"
                        "font-size: 16px;"
                        );
                    if (layout) { // Check if layout is valid before adding
                        qDebug() << "Condition met: layout (adding descriptionText)";
                        layout->addWidget(descriptionText);
                    } else {
                        qDebug() << "Condition met: !layout (adding descriptionText)";
                        // Handle error
                    }
                } else {
                    qDebug() << "Condition met: descriptionText - Existing descriptionText QTextEdit found.";
                }
                descriptionText->setText(description.isEmpty() ? "No description provided" : description);
            } else {
                qDebug() << "Condition met: !descriptionFrame - Error: descriptionFrame not found.";
            }
        } else {
            qDebug() << "Condition met: !contentFrame - Error: RequestContent frame not found within mainFrame.";
        }

        // Workers list frame
        QFrame *workersListFrame = mainFrame->findChild<QFrame*>("workersListFrame");
        if (!workersListFrame) {
            qDebug() << "Condition met: !workersListFrame - workersListFrame not found in mainFrame. Creating new QFrame.";
            workersListFrame = new QFrame(mainFrame);
            workersListFrame->setObjectName("workersListFrame");

            QVBoxLayout *workersLayout = new QVBoxLayout(workersListFrame);
            workersListFrame->setLayout(workersLayout);

            QLabel *workerListLabel = new QLabel("Workers", workersListFrame);
            workerListLabel->setObjectName("workerList");
            workerListLabel->setStyleSheet("font-size: 14px; color: #FFD8A9;");
            if (workersLayout) { // Check if layout is valid before adding
                qDebug() << "Condition met: workersLayout (adding workerListLabel)";
                workersLayout->addWidget(workerListLabel);
            } else {
                qDebug() << "Condition met: !workersLayout (adding workerListLabel)";
                // Handle error
            }
        } else {
            qDebug() << "Condition met: workersListFrame - workersListFrame found in mainFrame.";
        }

        QVBoxLayout *workersLayout = qobject_cast<QVBoxLayout*>(workersListFrame->layout());
        if (workersLayout) {
            qDebug() << "Condition met: workersLayout (clearing layout) - workersListFrame layout found. Clearing previous worker list.";
            // Clear existing worker widgets except for the title label
            QLayoutItem *item;
            QList<QWidget*> widgetsToDelete;
            // Iterate in reverse to safely remove items
            for (int i = workersLayout->count() - 1; i >= 0; --i) {
                item = workersLayout->takeAt(i);
                if (QWidget *widget = item->widget()) {
                    // Keep the "Workers" title label, delete others
                    if (widget->objectName() != "workerList") {
                        widgetsToDelete.append(widget);
                        qDebug() << "Condition met: widget->objectName() != \"workerList\" - Marking widget" << widget->objectName() << "for deletion.";
                    } else {
                        qDebug() << "Condition met: widget->objectName() == \"workerList\" - Keeping workerList label.";
                    }
                }
                // Delete the layout item itself
                delete item;
                qDebug() << "Deleted layout item.";
            }

            // Now delete the marked widgets outside the loop
            for (QWidget *widget : widgetsToDelete) {
                widget->deleteLater();
                qDebug() << "Deleted old worker widget.";
            }

            // Re-add the static "Workers" label if it was present initially
            QLabel *workerListLabel = workersListFrame->findChild<QLabel*>("workerList");
            if (workerListLabel && workersLayout->indexOf(workerListLabel) == -1) {
                qDebug() << "Re-adding workerList label to workersLayout.";
                workersLayout->insertWidget(0, workerListLabel);
            } else if (!workerListLabel) {
                qDebug() << "Condition met: !workerListLabel - workerList label not found after clearing. This shouldn't happen if setup is correct.";
            }


            // Query assigned workers
            QSqlQuery workerQuery;
            workerQuery.prepare(
                "SELECT w.WORKERID, w.NAME, w.RATING "
                "FROM ASSIGNMENT a "
                "JOIN WORKER w ON a.WORKERID = w.WORKERID "
                "WHERE a.REQUESTID = :id"
                );
            workerQuery.bindValue(":id", requestId);

            bool hasWorkers = false;
            if (!workerQuery.exec()) {
                qDebug() << "Condition met: !workerQuery.exec() - Worker query error:" << workerQuery.lastError().text();
                QLabel *errorLabel = new QLabel("Failed to load workers.", workersListFrame);
                errorLabel->setStyleSheet("color: #FF0000; font-size: 14px;");
                workersLayout->addWidget(errorLabel);
                qDebug() << "Added error label for workers.";
            } else {
                qDebug() << "Condition met: workerQuery.exec() - Worker query executed successfully.";
                if (!workerQuery.next()) {
                    qDebug() << "Condition met: !workerQuery.next() - No workers found for this request.";
                    QLabel *noWorkersLabel = new QLabel("No workers assigned.", workersListFrame);
                    noWorkersLabel->setStyleSheet("color: #FFD8A9; font-size: 14px;");
                    workersLayout->addWidget(noWorkersLabel);
                    qDebug() << "Added 'No workers assigned' label.";
                } else {
                    qDebug() << "Condition met: workerQuery.next() - Workers found. Adding worker buttons.";
                    hasWorkers = true;
                    do {
                        int workerId = workerQuery.value("WORKERID").toInt();
                        QString workerName = workerQuery.value("NAME").toString();
                        float rating = workerQuery.value("RATING").toFloat();
                        QString displayText = QString("%1 (Rating: %2)").arg(workerName).arg(rating, 0, 'f', 1);

                        QPushButton *workerButton = new QPushButton(displayText, workersListFrame);
                        workerButton->setStyleSheet(
                            "QPushButton {"
                            "   color: #FDEEDC;"
                            "   background: #E38B29;"
                            "   border-radius: 8px;"
                            "   font-size: 14px;"
                            "   padding: 4px;"
                            "   text-align: left;"
                            "   margin: 2px 0;" // Added vertical margin for spacing
                            "}"
                            "QPushButton:hover {"
                            "   background: #C37422;"
                            "}"
                            );
                        workerButton->setCursor(Qt::PointingHandCursor);
                        workersLayout->addWidget(workerButton);

                        // Connect button click to navigate to WorkerPage
                        connect(workerButton, &QPushButton::clicked, this, [this, workerId]() {
                            QWidget *workerPage = ui->stackedWidget_2->findChild<QWidget*>("workerPage");
                            if (workerPage) {
                                qDebug() << "Condition met: workerPage (navigation)";
                                ui->stackedWidget_2->setCurrentWidget(workerPage);
                                this->viewWorkerDetails(workerId); // Assuming viewWorkerDetails is a slot in Home class
                                qDebug() << "Navigated to WorkerPage for WorkerID:" << workerId;
                            } else {
                                qDebug() << "Condition met: !workerPage (navigation)";
                                qDebug() << "Error: WorkerPage not found in stackedWidget_2!";
                                QMessageBox::critical(this, "UI Error", "WorkerPage not found.");
                            }
                        });
                        qDebug() << "Added button for worker:" << workerName;
                    } while (workerQuery.next());
                }
            }

            // Add vertical spacer at the end to push everything above upwards
            // Ensure the spacer is added *after* all worker widgets
            workersLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
            qDebug() << "Added vertical spacer to workersListFrame layout.";

        } else {
            qDebug() << "Condition met: !workersLayout (initial check) - Error: workersListFrame found but has no QVBoxLayout. Ensure setup_request_page() creates one.";
        }


        // Ensure contentFrame and workersListFrame are in mainLayout only if not already present
        if (contentFrame && mainLayout && mainLayout->indexOf(contentFrame) == -1) {
            qDebug() << "Condition met: contentFrame && mainLayout && mainLayout->indexOf(contentFrame) == -1 - Adding contentFrame to mainLayout.";
            mainLayout->addWidget(contentFrame);
        } else if (!contentFrame) {
            qDebug() << "Condition met: !contentFrame (adding to mainLayout) - contentFrame is null.";
        } else if (!mainLayout) {
            qDebug() << "Condition met: !mainLayout (adding to mainLayout) - mainLayout is null.";
        } else if (mainLayout->indexOf(contentFrame) != -1) {
            qDebug() << "Condition met: mainLayout->indexOf(contentFrame) != -1 - contentFrame already in mainLayout.";
        }


        if (workersListFrame && mainLayout && mainLayout->indexOf(workersListFrame) == -1) {
            qDebug() << "Condition met: workersListFrame && mainLayout && mainLayout->indexOf(workersListFrame) == -1 - Adding workersListFrame to mainLayout.";
            mainLayout->addWidget(workersListFrame);
        } else if (!workersListFrame) {
            qDebug() << "Condition met: !workersListFrame (adding to mainLayout) - workersListFrame is null.";
        } else if (!mainLayout) {
            qDebug() << "Condition met: !mainLayout (adding to mainLayout) - mainLayout is null.";
        } else if (mainLayout->indexOf(workersListFrame) != -1) {
            qDebug() << "Condition met: mainLayout->indexOf(workersListFrame) != -1 - workersListFrame already in mainLayout.";
        }


        // Force layout update to ensure rendering
        mainFrame->update();
        requestPage->update();
        qDebug() << "Called mainFrame->update() and requestPage->update().";


        // Switch to RequestPage
        ui->stackedWidget_2->setCurrentWidget(requestPage);
        qDebug() << "Switched to RequestPage";
    } else {
        qDebug() << "Condition met: !query.next() - No request found with ID:" << requestId;
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
    // Clear existing content in ui->frame_21
    QLayout *existingLayout = ui->frame_23->layout();
    if (existingLayout) {
        QLayoutItem *item;
        while ((item = existingLayout->takeAt(0)) != nullptr) {
            if (QWidget *widget = item->widget()) {
                widget->deleteLater(); // Safely delete the widget
            }
            delete item; // Delete the layout item
        }
    }

    // Create a new scroll area
    QScrollArea *scrollArea = new QScrollArea(ui->frame_23);
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

    QWidget *scrollContent = new QWidget(scrollArea);
    if (!scrollContent) {
        qDebug() << "Failed to allocate QWidget";
        delete scrollArea;
        return;
    }
    scrollContent->setStyleSheet("QWidget { background-color: transparent; }");

    QVBoxLayout *cardsLayout = new QVBoxLayout(scrollContent);
    cardsLayout->setSpacing(15);

    QSqlQuery query;
    query.prepare(
        "WITH WorkerWagesInRange AS ( "
        "    SELECT "
        "        a.WorkerID, "
        "        SUM(ISNULL(a.Wage, 0)) AS TotalWageInPeriod "
        "    FROM ASSIGNMENT a "
        "    JOIN REQUEST r ON a.RequestID = r.RequestID "
        "    WHERE a.Status = 'Completed' AND r.RequestTime >= :startDate AND r.RequestTime < :endDatePlusOneDay "
        "    GROUP BY a.WorkerID "
        ") "
        "SELECT DISTINCT "
        "    w.WORKERID, w.name AS WorkerName, w.LOCATIONS, STRING_AGG(t.TASKNAME, ', ') WITHIN GROUP (ORDER BY t.TASKNAME) AS Specialties, w.RATING, "
        "    ISNULL(wwir.TotalWageInPeriod, 0) AS CalculatedWage "
        "FROM WORKER w "
        "LEFT JOIN SPECIALTY s ON w.WORKERID = s.WORKERID "
        "LEFT JOIN TASK t ON s.TASKID = t.TASKID "
        "LEFT JOIN WorkerWagesInRange wwir ON w.WORKERID = wwir.WorkerID "
        "GROUP BY w.WORKERID, w.name, w.LOCATIONS, w.RATING, wwir.TotalWageInPeriod "
        "ORDER BY w.RATING DESC, WorkerName ASC"
        );
    query.bindValue(":startDate", QDateTime(workerStartDateValue, QTime(0, 0)));
    query.bindValue(":endDatePlusOneDay", QDateTime(workerEndDateValue.addDays(1), QTime(0, 0)));

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
        QString name = query.value("WorkerName").toString();
        QString locations = query.value("LOCATIONS").toString();
        QString taskName = query.value("Specialties").toString();
        float rating = query.value("RATING").toFloat();
        float calculatedWage = query.value("CalculatedWage").toFloat();

        QFrame *card = createWorkerCard(workerId, name, taskName, locations, rating, calculatedWage);
        cardsLayout->addWidget(card);
    }

    cardsLayout->addStretch();
    scrollContent->setLayout(cardsLayout);
    scrollArea->setWidget(scrollContent);

    if (existingLayout) {
        existingLayout->addWidget(scrollArea);
    } else {
        QVBoxLayout *newLayout = new QVBoxLayout(ui->frame_23);
        newLayout->addWidget(scrollArea);
        ui->frame_23->setLayout(newLayout);
    }
}

QFrame* Home::createWorkerCard(int workerId, QString name, QString &taskName,
                               QString &locations, const float &rating, const float &calculatedWage)
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
        "   max-height: 120px;"
        "   max-height: 170px;"
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
        "   font-size: 20px;"
        "   font-size: 20px;"
        "   color: #E38B29;"
        "   border: none;"
        "}"
        );
    workerLabel->setWordWrap(true);

    QLabel *ratingLabel = new QLabel(QString("Rating: %1").arg(rating, 0, 'f', 1));
    ratingLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 20px;"
        "   font-size: 18px;"
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
        "   font-size: 20px;"
        "   font-size: 16px;"
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

    QLabel *wageLabel = new QLabel(QString("Wage: $%1").arg(calculatedWage, 0, 'f', 2));
    wageLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 18px;"
        "   color: #E38B29;"
        "   margin-top: 5px;"
        "   border: none;"
        "}"
        );

    cardLayout->addLayout(topRowLayout);
    cardLayout->addWidget(taskLabel);
    cardLayout->addWidget(wageLabel);

    cardLayout->addStretch();

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
                ui->stackedWidget_2->setCurrentWidget(ui->stackedWidget_2->findChild<QWidget *>("requestsPage"));
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
        "   max-height: 150px;"
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
        "   font-size: 20px;"
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
            "   font-size: 20px;"
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
            "   font-size: 20px;"
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
        "   font-size: 17px;"
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
        "   font-size: 15px;"
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
                  "WHERE ASSIGNMENT.CLIENTID = CLIENT.CLIENTID AND WORKERID = :workerId AND ASSIGNMENT.STATUS = 'Completed' "
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
            "   font-size: 20px;"
            "   color: #E38B29;"
            "}"
            );

        QString ratingText = QString("<span style='font-size:20px; font-weight:bold;'>Rating: </span>"
                                     "<span style='font-size:17px;'>%1</span>")
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
        locationsLabel->setText("<span style='font-size:20px; font-weight:bold;'>Locations: </span>"
                                "<span style='font-size:17px;'>" + locations + "</span>");
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
        specialtyLabel->setText("<span style='font-size:20px; font-weight:bold;'>Specialists: </span>"
                                "<span style='font-size:17px;'>" + taskName + "</span>");
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
        "   max-height: 150px;"
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
        "   font-size: 20px;"
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
            "   font-size: 20px;"
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
            "   font-size: 20px;"
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
        "   font-size: 17px;"
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
        "   font-size: 15px;"
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
        "   font-size: 20px;"
        "   color: #E38B29;"
        "}"
        );

    QLabel *locationsLabel = new QLabel(card);
    locationsLabel->setText("<span style='font-size:20px; font-weight:bold;'>Locations: </span>"
                            "<span style='font-size:17px;'>" + clientData.address + "</span>");
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
    specialtyLabel->setText("<span style='font-size:20px; font-weight:bold;'>Feedback: </span>"
                            "<span style='font-size:17px;'>" + clientData.feedback + "</span>");
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
// for tasks page

void Home::goToRequestPage(int taskId,QString taskName){
    requestTask = new RequestTask(this,taskId,taskName);
    requestTask->setClient(taskId,taskName, clientData.id, clientData.name, clientData.password, clientData.address, clientData.email, clientData.phone,clientData.feedback);
    requestTask->show();
    connect(requestTask, &RequestTask::backToHome, this, [this]() {
        this->show();
        this->loadAllRequests();
    });
    this->hide();
}

QFrame* Home::createTaskCard(int &taskId, QString &taskName, QString &workersName,int &timeFinish, float &fee){
    QFrame *card = new QFrame();
    card->setObjectName(QString("taskCard_%1").arg(taskId));
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

    card->setCursor(Qt::PointingHandCursor);
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(card);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 50));
    shadowEffect->setOffset(2, 2);
    card->setGraphicsEffect(shadowEffect);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);

    QLabel *taskLabel = new QLabel(QString("%1 - %2").arg(taskId).arg(taskName), card);
    taskLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 20px;"
        "   color: #E38B29;"
        "   border: none;"
        "   margin: 0px;"
        "   padding: 0px;"
        "}"
        );

    QLabel *timeFinishLabel = new QLabel(QString("Average Time To Finish: %1 hour").arg(timeFinish), card);
    timeFinishLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 20px;"
        "   color: #E38B29;"
        "   border: none;"
        "   margin: 0px;"
        "   padding: 0px;"
        "}"
        );

    QLabel *feeLabel;
    feeLabel = new QLabel(QString("Average Fee: %1 $").arg(fee, 0, 'f', 1), card);
    feeLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 20px;"
        "   color: #E38B29;"
        "   border: none;"
        "   margin: 0px;"
        "   padding: 0px;"
        "}"
        );

    QLabel *workerLabel;
    workerLabel = new QLabel(QString("Workers: %1").arg(workersName),card);
    workerLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 20px;"
        "   color: #E38B29;"
        "   border: none;"
        "   margin: 0px;"
        "   padding: 0px;"
        "}"
        );

    cardLayout->addWidget(taskLabel);
    cardLayout->addWidget(timeFinishLabel);
    cardLayout->addWidget(feeLabel);
    cardLayout->addWidget(workerLabel);

    card->setFixedHeight(150);
    card->setProperty("taskId", taskId);
    card->setProperty("taskName", taskName);
    card->installEventFilter(this);
    return card;
}

void Home::loadAllTasks() {
    QWidget *tasksPage = ui->stackedWidget_2->findChild<QWidget*>("tasksPage");
    if (!tasksPage) {
        qDebug() << "Error: tasksPage not found!";
        return;
    }
    ui->stackedWidget_2->setCurrentWidget(tasksPage);

    // Find the existing content frame
    QFrame *content = tasksPage->findChild<QFrame*>("content");
    if (!content) {
        qDebug() << "Error: content frame not found!";
        return;
    }

    // Clear existing content
    QLayout *layout = content->layout();
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    } else {
        // Create new layout if none exists
        layout = new QVBoxLayout(content);
        content->setLayout(layout);
    }

    QFrame *contentFrame = tasksPage->findChild<QFrame*>("content");
    if (!contentFrame) {
       QMessageBox::critical(nullptr, "Error", "Error: content frame not found!");
        return;
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
    query.prepare("select WORKER.NAME, TASK.TASKNAME,AVGTIMETOFINISH,AVGFEE,TASK.TASKID "
                    "from TASK,WORKER,SPECIALTY "
                    "where SPECIALTY.TASKID = TASK.TASKID and SPECIALTY.WORKERID = WORKER.WORKERID "
                    "order by TASK.TASKID");

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch requests: " + query.lastError().text());
        delete scrollContent;
        delete scrollArea;
        return;
    }

    while (query.next()) {
        int taskId = query.value("TASK.TASKID").toInt();
        QString taskName = query.value("TASK.TASKNAME").toString();
        QString workersName = query.value("WORKER.NAME").toString();
        int timeFinish = query.value("AVGTIMETOFINISH").toInt();
        float fee = query.value("AVGFEE").toFloat();

        QFrame *card = createTaskCard(taskId, taskName, workersName, timeFinish,fee);
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

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// for statistics page

void Home::loadAllstatistics(){
    QWidget *statisticPage = ui->stackedWidget_2->findChild<QWidget*>("statisticPage");
    if (!statisticPage) {
        qDebug() << "Error: statisticPage not found!";
        return;
    }
    ui->stackedWidget_2->setCurrentWidget(statisticPage);

    // Find the existing content frame
    QFrame *content = statisticPage->findChild<QFrame*>("statistics");
    if (!content) {
        qDebug() << "Error: statistics frame not found!";
        return;
    }

    // Clear existing content
    QLayout *layout = content->layout();
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    } else {
        // Create new layout if none exists
        layout = new QVBoxLayout(content);
        content->setLayout(layout);
    }

    QFrame *contentFrame = statisticPage->findChild<QFrame*>("statistics");
    if (!contentFrame) {
        QMessageBox::critical(nullptr, "Error", "Error: statistics frame not found!");
        return;
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

    // base for each card
    QString labelStyle3 =
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
        ;

    // Apply consistent styling
    QString labelStyle =
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 20px;"
        "   color: #E38B29;"
        "   border: none;"
        "   margin: 0px;"
        "   padding: 0px;"
        "}";

    // Apply consistent styling
    QString labelStyle2 =
        "QLabel {"
        "   font-weight: bold;"
        "   font-size: 25px;"
        "   color: #E38B29;"
        "   border: none;"
        "   margin: 0px;"
        "   padding: 0px;"
        "}";

    QSqlQuery query;
    // best worker
    query.prepare(
        "SELECT w.*, t.taskName "
        "FROM worker w "
        "JOIN specialty s ON w.workerId = s.workerId "
        "JOIN task t ON s.taskId = t.taskId "
        "WHERE w.rating = ( "
        "SELECT MIN(w2.rating) "
        "FROM worker w2 "
        "JOIN specialty s2 ON w2.workerId = s2.workerId "
        "WHERE s2.taskId = s.taskId "
        ")"
        );

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch requests: " + query.lastError().text());
        delete scrollContent;
        delete scrollArea;
        return;
    }

    while (query.next()) {
        QFrame *card = new QFrame();
        card->setStyleSheet(labelStyle3);
        QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(card);
        card->setFrameShape(QFrame::StyledPanel);
        card->setGraphicsEffect(shadowEffect);
        shadowEffect->setBlurRadius(10);
        shadowEffect->setColor(QColor(0, 0, 0, 50));
        shadowEffect->setOffset(2, 2);

        int workerId = query.value("workerId").toInt();
        QString workerName = query.value("name").toString();
        QString locations = query.value("locations").toString();
        QString taskName = query.value("taskName").toString();
        QVariant ratingVar = query.value("rating");

        QHBoxLayout *leftGroupLayout = new QHBoxLayout();

        QLabel *workerLabel = new QLabel(QString("%1 - %2").arg(workerId).arg(workerName), card);
        QLabel *ratingLabel;
        if (ratingVar.isNull()) {
            ratingLabel = new QLabel("Rating: NULL", card);
        } else {
            float rating = ratingVar.toFloat();
            ratingLabel = new QLabel(QString("Rating: %1").arg(rating, 0, 'f', 1), card);
        }
        QLabel *locationsLabel = new QLabel(QString("Location: %1").arg(locations), card);
        QLabel *taskLabel = new QLabel(QString("Task: %1").arg(taskName), card);
        QLabel *label = new QLabel(QString("Best worker in %1").arg(QString(taskName)));

        workerLabel->setStyleSheet(labelStyle);
        ratingLabel->setStyleSheet(labelStyle);
        locationsLabel->setStyleSheet(labelStyle);
        taskLabel->setStyleSheet(labelStyle);
        label->setStyleSheet(labelStyle2);

        leftGroupLayout->addWidget(workerLabel);
        leftGroupLayout->setSpacing(50);
        leftGroupLayout->addWidget(ratingLabel);
        leftGroupLayout->addStretch();
        leftGroupLayout->addWidget(locationsLabel);

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->addWidget(label);
        cardLayout->addLayout(leftGroupLayout);
        cardLayout->addWidget(taskLabel);

        card->setFixedHeight(120);
        cardsLayout->addWidget(card);
    }

    // worst worker
    query.prepare(
        "SELECT w.*, t.taskName "
        "FROM worker w "
            "JOIN specialty s ON w.workerId = s.workerId "
              "JOIN task t ON s.taskId = t.taskId "
             "WHERE w.rating = ( "
            "SELECT MAX(w2.rating) "
            "FROM worker w2 "
                "JOIN specialty s2 ON w2.workerId = s2.workerId "
                  "WHERE s2.taskId = s.taskId "
            ")"
        );

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              "Could not fetch requests: " + query.lastError().text());
        delete scrollContent;
        delete scrollArea;
        return;
    }

    while (query.next()) {
        QFrame *card2 = new QFrame();
        card2->setStyleSheet(labelStyle3);
        QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(card2);
        card2->setFrameShape(QFrame::StyledPanel);
        card2->setGraphicsEffect(shadowEffect);
        shadowEffect->setBlurRadius(10);
        shadowEffect->setColor(QColor(0, 0, 0, 50));
        shadowEffect->setOffset(2, 2);

        int workerId = query.value("workerId").toInt();
        QString workerName = query.value("name").toString();
        QString locations = query.value("locations").toString();
        QString taskName = query.value("taskName").toString();
        QVariant ratingVar = query.value("rating");

        QHBoxLayout *leftGroupLayout = new QHBoxLayout();

        QLabel *workerLabel = new QLabel(QString("%1 - %2").arg(workerId).arg(workerName), card2);
        QLabel *ratingLabel;
        if (ratingVar.isNull()) {
            ratingLabel = new QLabel("Rating: NULL", card2);
        } else {
            float rating = ratingVar.toFloat();
            ratingLabel = new QLabel(QString("Rating: %1").arg(rating, 0, 'f', 1), card2);
        }
        QLabel *locationsLabel = new QLabel(QString("Location: %1").arg(locations), card2);
        QLabel *taskLabel = new QLabel(QString("Task: %1").arg(taskName), card2);
        QLabel *label = new QLabel(QString("Worst worker in %1").arg(QString(taskName)));

        // Apply consistent styling
        QString labelStyle =
            "QLabel {"
            "   font-weight: bold;"
            "   font-size: 20px;"
            "   color: #E38B29;"
            "   border: none;"
            "   margin: 0px;"
            "   padding: 0px;"
            "}";

        // Apply consistent styling
        QString labelStyle2 =
            "QLabel {"
            "   font-weight: bold;"
            "   font-size: 25px;"
            "   color: #E38B29;"
            "   border: none;"
            "   margin: 0px;"
            "   padding: 0px;"
            "}";

        workerLabel->setStyleSheet(labelStyle);
        ratingLabel->setStyleSheet(labelStyle);
        locationsLabel->setStyleSheet(labelStyle);
        taskLabel->setStyleSheet(labelStyle);
        label->setStyleSheet(labelStyle2);

        leftGroupLayout->addWidget(workerLabel);
        leftGroupLayout->setSpacing(50);
        leftGroupLayout->addWidget(ratingLabel);
        leftGroupLayout->addStretch();
        leftGroupLayout->addWidget(locationsLabel);

        QVBoxLayout *cardLayout = new QVBoxLayout(card2);
        cardLayout->addWidget(label);
        cardLayout->addLayout(leftGroupLayout);
        cardLayout->addWidget(taskLabel);

        card2->setFixedHeight(120);
        cardsLayout->addWidget(card2);
    }

    cardsLayout->addStretch();
    scrollContent->setLayout(cardsLayout);
    scrollArea->setWidget(scrollContent);

    QVBoxLayout *frameLayout = qobject_cast<QVBoxLayout*>(contentFrame->layout());
    if (frameLayout) {
        frameLayout->addWidget(scrollArea);
    }
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

Home::Home(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Home)
    , requestTask(nullptr)
{
    ui->setupUi(this);
    setup_request_page();
    ui->startDate->setText(" " + startDateValue.toString("yyyy-MM-dd"));
    ui->endDate->setText(" " + endDateValue.toString("yyyy-MM-dd"));
    ui->startDate_3->setText(" " + workerStartDateValue.toString("yyyy-MM-dd"));
    ui->endDate_3->setText(" " + workerEndDateValue.toString("yyyy-MM-dd"));
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

    // Setup profile icon in statistic
    QSvgWidget *profileIcon8 = new QSvgWidget(":/new/svgs/Group.svg");
    profileIcon8->setFixedSize(58, 58);
    QHBoxLayout *profileLayout8 = qobject_cast<QHBoxLayout *>(ui->profile_8->layout());
    if (profileLayout8) {
        profileLayout8->insertWidget(0, profileIcon8);
    } else {
        auto *newLayout = new QHBoxLayout(ui->profile_8);
        newLayout->addWidget(profileIcon8);
        ui->profile_8->setLayout(newLayout);
    }

    // Setup profile icon in request
    QSvgWidget *profileIcon7 = new QSvgWidget(":/new/svgs/Group.svg");
    profileIcon7->setFixedSize(58, 58);
    QHBoxLayout *profileLayout7 = qobject_cast<QHBoxLayout *>(ui->profile_2->layout());
    if (profileLayout7) {
        profileLayout7->insertWidget(0, profileIcon7);
    } else {
        auto *newLayout = new QHBoxLayout(ui->profile_2);
        newLayout->addWidget(profileIcon7);
        ui->profile_2->setLayout(newLayout);
    }

    // Setup profile icon in tasks
    QSvgWidget *profileIcon5 = new QSvgWidget(":/new/svgs/Group.svg");
    profileIcon5->setFixedSize(58, 58);
    QHBoxLayout *profileLayout5 = qobject_cast<QHBoxLayout *>(ui->profile_5->layout());
    if (profileLayout5) {
        profileLayout5->insertWidget(0, profileIcon5);
    } else {
        auto *newLayout = new QHBoxLayout(ui->profile_5);
        newLayout->addWidget(profileIcon5);
        ui->profile_5->setLayout(newLayout);
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
    setupWorkerCards();

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

    ui->stackedWidget_2->setCurrentWidget(ui->stackedWidget_2->findChild<QWidget *>("requestsPage"));
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
    if (workerStartDateCalendar) {
        workerStartDateCalendar->deleteLater();
        workerStartDateCalendar = nullptr;
    }

    workerStartDateCalendar = new QCalendarWidget(this);
    workerStartDateCalendar->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    workerStartDateCalendar->setStyleSheet(
        "QCalendarWidget { background-color: #FDEEDC; color: #E38B29; font-size: 10px; border: 1px solid #E38B29; }"
        "QCalendarWidget QToolButton { color: #E38B29; background-color: transparent; }"
        "QCalendarWidget QMenu { background-color: #FDEEDC; color: #E38B29; }"
        "QCalendarWidget QAbstractItemView:enabled { color: #E38B29; selection-background-color: #E38B29; selection-color: #FFFFFF; }"
        "QCalendarWidget QAbstractItemView:disabled { color: #D3D3D3; }"
        );

    workerStartDateCalendar->setSelectedDate(workerStartDateValue.isValid()
                                                 ? workerStartDateValue
                                                 : QDate::currentDate());
    workerStartDateCalendar->setMinimumDate(QDate(2000, 1, 1));
    workerStartDateCalendar->setMaximumDate(QDate::currentDate().addYears(5));

    QPoint pos = ui->startDate_3->mapToGlobal(QPoint(0, ui->startDate_3->height()));
    workerStartDateCalendar->move(pos);

    // Proper signal connections
    connect(workerStartDateCalendar, &QCalendarWidget::clicked, this, [this](const QDate &date) {
        onWorkerStartDateSelected(date);
        workerStartDateCalendar->deleteLater();
    });

    connect(workerStartDateCalendar, &QCalendarWidget::destroyed, [this]() {
        workerStartDateCalendar = nullptr;
    });

    workerStartDateCalendar->show();
}

// Worker End Date Selection
void Home::on_endDate_3_clicked()
{
    if (workerEndDateCalendar) {
        workerEndDateCalendar->deleteLater();
        workerEndDateCalendar = nullptr;
    }

    workerEndDateCalendar = new QCalendarWidget(this);
    workerEndDateCalendar->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    workerEndDateCalendar->setStyleSheet(
        "QCalendarWidget { background-color: #FDEEDC; color: #E38B29; font-size: 10px; border: 1px solid #E38B29; }"
        "QCalendarWidget QToolButton { color: #E38B29; background-color: transparent; }"
        "QCalendarWidget QMenu { background-color: #FDEEDC; color: #E38B29; }"
        "QCalendarWidget QAbstractItemView:enabled { color: #E38B29; selection-background-color: #E38B29; selection-color: #FFFFFF; }"
        "QCalendarWidget QAbstractItemView:disabled { color: #D3D3D3; }"
        );

    workerEndDateCalendar->setSelectedDate(workerEndDateValue.isValid()
                                               ? workerEndDateValue
                                               : QDate::currentDate());
    workerEndDateCalendar->setMinimumDate(workerStartDateValue.isValid()
                                              ? workerStartDateValue
                                              : QDate(2000, 1, 1));
    workerEndDateCalendar->setMaximumDate(QDate::currentDate().addYears(5));

    QPoint pos = ui->endDate_3->mapToGlobal(QPoint(0, ui->endDate_3->height()));
    workerEndDateCalendar->move(pos);

    // Proper signal connections
    connect(workerEndDateCalendar, &QCalendarWidget::clicked, this, [this](const QDate &date) {
        onWorkerEndDateSelected(date);
        workerEndDateCalendar->deleteLater();
    });

    connect(workerEndDateCalendar, &QCalendarWidget::destroyed, [this]() {
        workerEndDateCalendar = nullptr;
    });

    workerEndDateCalendar->show();
}

// Date selection handlers
void Home::onWorkerStartDateSelected(const QDate &date)
{
    workerStartDateValue = date;
    ui->startDate_3->setText(" " + date.toString("yyyy-MM-dd"));
    loadAllWorkers();
}

void Home::onWorkerEndDateSelected(const QDate &date)
{
    workerEndDateValue = date;
    ui->endDate_3->setText(" " + date.toString("yyyy-MM-dd"));
    loadAllWorkers();
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
    ui->pushButton_14->setText(clientData.name);
    ui->pushButton_16->setText(clientData.name);
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

void Home::on_deleteAccount_clicked()
{
    try {
        QSqlQuery query;

        // Delete assignments
        query.prepare("DELETE FROM ASSIGNMENT WHERE CLIENTID = :clientId");
        query.bindValue(":clientId", clientData.id);
        if (!query.exec()) throw query.lastError();

        // Delete requests
        query.prepare("DELETE FROM REQUEST WHERE CLIENTID = :clientId");
        query.bindValue(":clientId", clientData.id);
        if (!query.exec()) throw query.lastError();

        // Finally delete the client
        query.prepare("DELETE FROM CLIENT WHERE CLIENTID = :clientId");
        query.bindValue(":clientId", clientData.id);
        if (!query.exec()) throw query.lastError();

        QSqlDatabase::database().commit(); // Commit the transaction
        QMessageBox::information(this, "Success", "Client and all related data deleted successfully!");
        emit backToMainWindow();
        this->close();
    } catch (const QSqlError &error) {
        QSqlDatabase::database().rollback(); // Rollback on error
        QMessageBox::critical(this, "Error", "Failed to delete client: " + error.text());
    }
}

void Home::on_request_clicked()
{
    if (requestTask) {
        delete requestTask;
    }
    requestTask = new RequestTask(this);
    requestTask->show();
}

void Home::requests(){
    QWidget *requestsPage = ui->stackedWidget_2->findChild<QWidget*>("requestsPage");
    if (requestsPage) {
        ui->stackedWidget_2->setCurrentWidget(requestsPage);
    } else {
        qDebug() << "Error: requestsPage not found!";
    }
    loadDataInProfile();
}

void Home::workers(){
    QWidget *workersPage = ui->stackedWidget_2->findChild<QWidget*>("workersPage");
    if (workersPage) {
        ui->stackedWidget_2->setCurrentWidget(workersPage);
    } else {
        qDebug() << "Error: workersPage not found!";
    }
    loadDataInProfile();
}

void Home::profile(){
    QWidget *profilePage = ui->stackedWidget_2->findChild<QWidget*>("profilePage");
    if (profilePage) {
        ui->stackedWidget_2->setCurrentWidget(profilePage);
    } else {
        qDebug() << "Error: profilePage not found!";
    }
    loadDataInProfile();
}

void Home::tasks(){
    QWidget *tasksPage = ui->stackedWidget_2->findChild<QWidget*>("tasksPage");
    if (tasksPage) {
        ui->stackedWidget_2->setCurrentWidget(tasksPage);
    } else {
        qDebug() << "Error: tasksPage not found!";
    }
    loadDataInProfile();
    loadAllTasks();
}

void Home::statistics(){
    QWidget *statisticPage = ui->stackedWidget_2->findChild<QWidget*>("statisticPage");
    if (statisticPage) {
        ui->stackedWidget_2->setCurrentWidget(statisticPage);
    } else {
        qDebug() << "Error: statisticPage not found!";
    }
    loadDataInProfile();
    loadAllstatistics();
}

void Home::logout(){
    emit backToMainWindow();
    this->close();
}

void Home::on_pushButton_8_clicked(){logout();}
void Home::on_pushButton_12_clicked(){logout();}
void Home::on_pushButton_11_clicked(){logout();}
void Home::on_pushButton_10_clicked(){logout();}
void Home::on_pushButton_9_clicked(){logout();}
void Home::on_pushButton_clicked(){logout();}
void Home::on_taskBtn_clicked(){tasks();}
void Home::on_taskBtn_2_clicked(){tasks();}
void Home::on_taskBtn_3_clicked(){tasks();}
void Home::on_taskBtn_4_clicked(){tasks();}
void Home::on_taskBtn_5_clicked(){tasks();}
void Home::on_taskBtn_6_clicked(){tasks();}
void Home::on_taskBtn_7_clicked(){tasks();}
void Home::on_workersPageBtn_5_clicked(){ workers();}
void Home::on_requestsPageBtn_5_clicked(){requests();}
void Home::on_pushButton_14_clicked(){profile();}
void Home::on_pushButton_13_clicked(){logout();}
void Home::on_workersPageBtn_clicked(){workers();}
void Home::on_requestsPageBtn_2_clicked(){requests();}
void Home::on_logout_clicked(){logout();}
void Home::on_requestsPageBtn_4_clicked(){requests();}
void Home::on_workersPageBtn_4_clicked(){workers();}
void Home::on_workersPageBtn_3_clicked(){workers();}
void Home::on_pushButton_5_clicked(){profile();}
void Home::on_pushButton_4_clicked(){profile();}
void Home::on_pushButton_2_clicked(){profile();}
void Home::on_pushButton_3_clicked(){profile();}
void Home::on_requestsPageBtn_6_clicked(){requests();}
void Home::on_workersPageBtn_6_clicked(){workers();}
void Home::on_pushButton_6_clicked(){profile();}
void Home::on_workersPageBtn_7_clicked(){workers();}
void Home::on_requestsPageBtn_7_clicked(){requests();}
void Home::on_pushButton_7_clicked(){profile();}
void Home::on_taskBtn_8_clicked(){tasks();}
void Home::on_workersPageBtn_8_clicked(){workers();}
void Home::on_requestsPageBtn_8_clicked(){requests();}
void Home::on_pushButton_16_clicked(){profile();}
void Home::on_pushButton_15_clicked(){logout();}
void Home::on_statisticBtn_2_clicked(){statistics();}
void Home::on_statisticBtn_3_clicked(){statistics();}
void Home::on_statisticBtn_4_clicked(){statistics();}
void Home::on_statisticBtn_5_clicked(){statistics();}
void Home::on_statisticBtn_clicked(){statistics();}
void Home::on_statisticBtn_6_clicked(){statistics();}
void Home::on_statisticBtn_7_clicked(){statistics();}
void Home::on_statisticBtn_8_clicked(){statistics();}

void Home::on_deleteRequest_clicked()
{
    if (currentRequestId <= 0) {
        qDebug() << "No valid request ID selected for deletion.";
        QMessageBox::warning(this, "Invalid Request", "No request selected for deletion.");
        return;
    }

    QSqlDatabase::database().transaction();

    QSqlQuery assignmentQuery;
    assignmentQuery.prepare("DELETE FROM ASSIGNMENT WHERE REQUESTID = :id");
    assignmentQuery.bindValue(":id", currentRequestId);
    if (!assignmentQuery.exec()) {
        qDebug() << "Assignment delete error:" << assignmentQuery.lastError().text();
        QSqlDatabase::database().rollback();
        QMessageBox::critical(this, "Database Error",
                              "Could not delete related assignments: " + assignmentQuery.lastError().text());
        return;
    }

    QSqlQuery workerQuery;
    workerQuery.prepare("UPDATE WORKER SET REQUESTID = NULL WHERE REQUESTID = :id");
    workerQuery.bindValue(":id", currentRequestId);
    if (!workerQuery.exec()) {
        qDebug() << "Worker update error:" << workerQuery.lastError().text();
        QSqlDatabase::database().rollback();
        QMessageBox::critical(this, "Database Error",
                              "Could not update worker references: " + workerQuery.lastError().text());
        return;
    }

    QSqlQuery requestQuery;
    requestQuery.prepare("DELETE FROM REQUEST WHERE REQUESTID = :id");
    requestQuery.bindValue(":id", currentRequestId);
    if (!requestQuery.exec()) {
        qDebug() << "Request delete error:" << requestQuery.lastError().text();
        QSqlDatabase::database().rollback();
        QMessageBox::critical(this, "Database Error",
                              "Could not delete request: " + requestQuery.lastError().text());
        return;
    }

    if (requestQuery.numRowsAffected() == 0) {
        qDebug() << "No request found with ID:" << currentRequestId;
        QSqlDatabase::database().rollback();
        QMessageBox::warning(this, "Request Not Found",
                             "No request found with the specified ID.");
        return;
    }

    if (!QSqlDatabase::database().commit()) {
        qDebug() << "Transaction commit failed.";
        QSqlDatabase::database().rollback();
        QMessageBox::critical(this, "Database Error", "Failed to commit transaction.");
        return;
    }

    QMessageBox::information(this, "Success", "Request deleted successfully.");

    loadAllRequests();
    ui->stackedWidget_2->setCurrentWidget(ui->stackedWidget_2->findChild<QWidget*>("requestsPage"));
}

void Home::on_editRequest_clicked()
{
    ui->stackedWidget_2->setCurrentWidget(ui->stackedWidget_2->findChild<QWidget*>("editPage"));
    // Validate current request ID
    if (currentRequestId <= 0) {
        qDebug() << "No valid request ID selected for editing.";
        QMessageBox::warning(this, "Invalid Request", "No request selected for editing.");
        return;
    }

    // Navigate to EditRequestPage
    QWidget *editPage = ui->stackedWidget_2->findChild<QWidget*>("editPage");
    if (!editPage) {
        qDebug() << "Error: editPage not found in stackedWidget_2!";
        QMessageBox::critical(this, "UI Error", "editPage not found.");
        return;
    }
    ui->stackedWidget_2->setCurrentWidget(editPage);

    // Find UI elements
    QComboBox *taskCombo = editPage->findChild<QComboBox*>("comboBox_edit_2");
    QLineEdit *descriptionEdit = editPage->findChild<QLineEdit*>("description_edit_2");
    QDateTimeEdit *startDateEdit = editPage->findChild<QDateTimeEdit*>("startDate_edit_2");
    QDateTimeEdit *endDateEdit = editPage->findChild<QDateTimeEdit*>("endDate_edit_2");
    QDateTimeEdit *preferredTimeSlotEdit = editPage->findChild<QDateTimeEdit*>("preferredTimeSlots");

    // Validate UI elements
    if (!taskCombo || !descriptionEdit || !startDateEdit || !endDateEdit || !preferredTimeSlotEdit) {
        qDebug() << "Error: Missing UI elements on editPage.";
        QMessageBox::critical(this, "UI Error", "Required UI elements for editing not found.");
        return;
    }

    // Populate comboBox_2 with TASKNAME values if not already populated
    if (taskCombo->count() == 0) {
        QSqlQuery taskQuery("SELECT TASKNAME FROM TASK ORDER BY TASKNAME");
        while (taskQuery.next()) {
            taskCombo->addItem(taskQuery.value("TASKNAME").toString());
        }
    }

    // Query the current request details
    QSqlQuery query;
    query.prepare(
        "SELECT r.TASKID, t.TASKNAME, r.REQUESTDESCRIPTION, r.PREFERREDTIMESLOT "
        "FROM REQUEST r "
        "JOIN TASK t ON r.TASKID = t.TASKID "
        "WHERE r.REQUESTID = :id"
        );
    query.bindValue(":id", currentRequestId);
    if (!query.exec()) {
        qDebug() << "Error fetching request details:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error", "Failed to fetch request details: " + query.lastError().text());
        return;
    }

    if (!query.next()) {
        qDebug() << "No request found with ID:" << currentRequestId;
        QMessageBox::warning(this, "Request Not Found", "No request found with the specified ID.");
        return;
    }

    // Extract request details
    QString taskName = query.value("TASKNAME").toString();
    QString description = query.value("REQUESTDESCRIPTION").toString();
    QDateTime preferredTimeSlot = query.value("PREFERREDTIMESLOT").toDateTime();
    preferredTimeSlot.setTimeZone(QTimeZone("Europe/Athens")); // Ensure EEST

    // Populate UI elements
    taskCombo->setCurrentText(taskName);
    descriptionEdit->setText(description);
    preferredTimeSlotEdit->setDateTime(preferredTimeSlot);

    // Set startDate_2 and endDate_2 to bracket preferredTimeSlot (e.g., ±1 hour)
    QDateTime startDate = preferredTimeSlot.addSecs(-3600); // 1 hour before
    QDateTime endDate = preferredTimeSlot.addSecs(3600);   // 1 hour after
    startDateEdit->setDateTime(startDate);
    endDateEdit->setDateTime(endDate);
}


void Home::on_comboBox_2_currentTextChanged(const QString &arg1)
{
    qDebug() << arg1 + '\n';
}


void Home::on_cancel_edit_clicked()
{
    viewRequestDetails(currentRequestId);
}


void Home::on_edit_edit_2_clicked()
{
    // Validate current request ID
    if (currentRequestId <= 0) {
        qDebug() << "No valid request ID selected for editing.";
        QMessageBox::warning(this, "Invalid Request", "No request selected for editing.");
        return;
    }

    // Find EditRequestPage
    QWidget *editPage = ui->stackedWidget_2->findChild<QWidget*>("editPage");
    if (!editPage) {
        qDebug() << "Error: EditRequestPage not found in stackedWidget_2!";
        QMessageBox::critical(this, "UI Error", "EditRequestPage not found.");
        return;
    }

    // Find UI elements
    QComboBox *taskCombo = editPage->findChild<QComboBox*>("comboBox_edit");
    QComboBox *statusCombo = editPage->findChild<QComboBox*>("comboBox_edit_2");
    QLineEdit *descriptionEdit = editPage->findChild<QLineEdit*>("description_edit_2");
    QDateTimeEdit *startDateEdit = editPage->findChild<QDateTimeEdit*>("startDate_edit_2");
    QDateTimeEdit *endDateEdit = editPage->findChild<QDateTimeEdit*>("endDate_edit_2");
    QDateTimeEdit *preferredTimeSlotEdit = editPage->findChild<QDateTimeEdit*>("preferredTimeSlots");

    // Validate UI elements
    if (!taskCombo || !statusCombo || !descriptionEdit || !startDateEdit || !endDateEdit || !preferredTimeSlotEdit) {
        qDebug() << "Error: Missing UI elements for editing request.";
        QMessageBox::critical(this, "UI Error", "Required UI elements for editing not found.");
        return;
    }

    // Validate task selection
    if (taskCombo->currentIndex() == -1) {
        QMessageBox::warning(this, "Error", "Please select a task!");
        return;
    }

    // Validate status selection
    if (statusCombo->currentIndex() == -1) {
        QMessageBox::warning(this, "Error", "Please select a status!");
        return;
    }

    // Validate date range
    QDateTime startDate = startDateEdit->dateTime();
    QDateTime endDate = endDateEdit->dateTime();
    if (startDate >= endDate) {
        QMessageBox::warning(this, "Error", "End time must be after start time!");
        return;
    }

    // Validate preferred time slot
    QDateTime preferredTimeSlot = preferredTimeSlotEdit->dateTime();
    QDateTime currentTime = QDateTime::currentDateTime().toTimeZone(QTimeZone("Europe/Athens"));
    if (preferredTimeSlot <= currentTime) {
        QMessageBox::warning(this, "Error", "Preferred time slot must be in the future!");
        return;
    }
    if (preferredTimeSlot < startDate || preferredTimeSlot > endDate) {
        QMessageBox::warning(this, "Error", "Preferred time slot must be between start and end times!");
        return;
    }

    // Get input values
    QString taskName = taskCombo->currentText();
    QString status = statusCombo->currentText();
    QString description = descriptionEdit->text();

    // Start transaction
    QSqlDatabase::database().transaction();

    // Get taskId from taskName
    QSqlQuery query;
    query.prepare("SELECT taskId FROM task WHERE taskName = :task");
    query.bindValue(":task", taskName);
    if (!query.exec() || !query.next()) {
        qDebug() << "Error fetching taskId for task:" << taskName;
        QMessageBox::critical(this, "Error", "Invalid task selected");
        QSqlDatabase::database().rollback();
        return;
    }
    int taskId = query.value(0).toInt();

    // Get current assignment status to check for transition
    query.prepare("SELECT STATUS, WORKERID, CLIENTID FROM ASSIGNMENT WHERE REQUESTID = :requestId");
    query.bindValue(":requestId", currentRequestId);
    if (!query.exec()) {
        qDebug() << "Error fetching assignment status:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to fetch assignment details: " + query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }

    QString oldStatus;
    int workerId = -1;
    int clientId = -1;
    bool hasAssignment = false;
    if (query.next()) {
        oldStatus = query.value("STATUS").toString();
        workerId = query.value("WORKERID").toInt();
        clientId = query.value("CLIENTID").toInt();
        hasAssignment = true;
    }

    // Update REQUEST
    query.prepare(
        "UPDATE request "
        "SET taskId = :taskId, requestDescription = :description, "
        "preferredTimeSlot = :preferredTimeSlot "
        "WHERE requestId = :requestId"
        );
    query.bindValue(":taskId", taskId);
    query.bindValue(":description", description.isEmpty() ? QVariant(QVariant::String) : description);
    query.bindValue(":preferredTimeSlot", preferredTimeSlot);
    query.bindValue(":requestId", currentRequestId);

    if (!query.exec()) {
        qDebug() << "Error updating request:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to update request: " + query.lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }

    if (query.numRowsAffected() == 0) {
        qDebug() << "No request found with ID:" << currentRequestId;
        QMessageBox::warning(this, "Request Not Found", "No request found with the specified ID.");
        QSqlDatabase::database().rollback();
        return;
    }

    // Update ASSIGNMENT status if it exists
    bool showRatingDialog = false;
    if (hasAssignment) {
        query.prepare(
            "UPDATE assignment "
            "SET status = :status "
            "WHERE requestId = :requestId"
            );
        query.bindValue(":status", status);
        query.bindValue(":requestId", currentRequestId);

        if (!query.exec()) {
            qDebug() << "Error updating assignment status:" << query.lastError().text();
            QMessageBox::critical(this, "Error", "Failed to update assignment status: " + query.lastError().text());
            QSqlDatabase::database().rollback();
            return;
        }

        // Check if status transitioned from Pending to Completed
        if (oldStatus == "Pending" && status == "Completed") {
            showRatingDialog = true;
        }
    } else {
        qDebug() << "No assignment found for request ID:" << currentRequestId;
    }

    // Commit transaction
    if (!QSqlDatabase::database().commit()) {
        qDebug() << "Transaction commit failed.";
        QMessageBox::critical(this, "Error", "Failed to commit transaction.");
        QSqlDatabase::database().rollback();
        return;
    }

    // Show rating/review dialog if needed
    if (showRatingDialog && workerId != -1 && clientId != -1) {
        QDialog *ratingDialog = new QDialog(this);
        ratingDialog->setWindowTitle("Rate and Review Worker");
        QVBoxLayout *dialogLayout = new QVBoxLayout(ratingDialog);

        // Rating
        QLabel *ratingLabel = new QLabel("Rate the worker (0 to 5):", ratingDialog);
        QDoubleSpinBox *ratingSpinBox = new QDoubleSpinBox(ratingDialog);
        ratingSpinBox->setRange(0.0, 5.0);
        ratingSpinBox->setSingleStep(0.1);
        ratingSpinBox->setValue(0.0);

        // Feedback
        QLabel *feedbackLabel = new QLabel("Provide feedback (optional):", ratingDialog);
        QTextEdit *feedbackEdit = new QTextEdit(ratingDialog);
        feedbackEdit->setPlaceholderText("Enter your feedback here...");

        // Buttons
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, ratingDialog);
        connect(buttonBox, &QDialogButtonBox::accepted, ratingDialog, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, ratingDialog, &QDialog::reject);

        // Add widgets to layout
        dialogLayout->addWidget(ratingLabel);
        dialogLayout->addWidget(ratingSpinBox);
        dialogLayout->addWidget(feedbackLabel);
        dialogLayout->addWidget(feedbackEdit);
        dialogLayout->addWidget(buttonBox);

        ratingDialog->setLayout(dialogLayout);

        if (ratingDialog->exec() == QDialog::Accepted) {
            double rating = ratingSpinBox->value();
            QString feedback = feedbackEdit->toPlainText();

            // Update ASSIGNMENT with rating and feedback
            QSqlQuery ratingQuery;
            ratingQuery.prepare(
                "UPDATE assignment "
                "SET workerRating = :rating, feedbackByClient = :feedback "
                "WHERE requestId = :requestId AND workerId = :workerId AND clientId = :clientId"
                );
            ratingQuery.bindValue(":rating", rating);
            ratingQuery.bindValue(":feedback", feedback.isEmpty() ? QVariant(QVariant::String) : feedback);
            ratingQuery.bindValue(":requestId", currentRequestId);
            ratingQuery.bindValue(":workerId", workerId);
            ratingQuery.bindValue(":clientId", clientId);

            if (!ratingQuery.exec()) {
                qDebug() << "Error updating assignment with rating/feedback:" << ratingQuery.lastError().text();
                QMessageBox::critical(this, "Error", "Failed to save rating and feedback: " + ratingQuery.lastError().text());
            } else {
                QMessageBox::information(this, "Success", "Rating and feedback saved successfully!");
            }
        }

        delete ratingDialog;
    }

    // Show success message for request update
    QMessageBox::information(this, "Success", "Request updated successfully!");

    // Refresh the requests list and navigate to requestsPage
    loadAllRequests();
    ui->stackedWidget_2->setCurrentWidget(ui->stackedWidget_2->findChild<QWidget*>("requestsPage"));
}

