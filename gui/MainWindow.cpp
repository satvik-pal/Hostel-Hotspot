#include "MainWindow.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QButtonGroup>
#include <QColor>
#include <QComboBox>
#include <QCompleter>
#include <QDateTime>
#include <QFont>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>

#include <algorithm>
#include <optional>
#include <vector>

namespace
{
    QString q(const std::string& text)
    {
        return QString::fromStdString(text);
    }

    std::string str(const QString& text)
    {
        return text.trimmed().toStdString();
    }

    QLabel* makeTitle(const QString& text, int pointSize)
    {
        QLabel* label = new QLabel(text);
        QFont font = label->font();
        font.setPointSize(pointSize);
        font.setBold(true);
        label->setFont(font);
        return label;
    }

    QLabel* makeHint(const QString& text)
    {
        QLabel* label = new QLabel(text);
        label->setWordWrap(true);
        label->setStyleSheet("color: gray;");
        return label;
    }

    QTableWidget* makeTable(const QStringList& headers)
    {
        QTableWidget* table = new QTableWidget(0, headers.size());
        table->setHorizontalHeaderLabels(headers);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setAlternatingRowColors(true);
        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        table->horizontalHeader()->setStretchLastSection(true);
        return table;
    }

    QTableWidgetItem* setCell(QTableWidget* table, int row, int column, const QString& text)
    {
        QTableWidgetItem* item = new QTableWidgetItem(text);
        table->setItem(row, column, item);
        return item;
    }

    QString selectedId(QTableWidget* table)
    {
        int row = table->currentRow();
        if (row < 0 || table->item(row, 0) == nullptr)
        {
            return QString();
        }
        return table->item(row, 0)->text();
    }

    QColor priorityColor(Priority priority)
    {
        switch (priority)
        {
            case Priority::High: return QColor(198, 40, 40);
            case Priority::Medium: return QColor(214, 130, 0);
            case Priority::Low: return QColor(46, 125, 50);
        }
        return QColor(0, 0, 0);
    }

    QCompleter* makeLocationCompleter(const QStringList& names, QObject* parent)
    {
        QCompleter* completer = new QCompleter(names, parent);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setFilterMode(Qt::MatchContains);
        return completer;
    }

    // Fills a complaint table, oldest first. Empty onlyStudent = all students.
    void fillComplaints(QTableWidget* table, const HostelSystem& sys,
                        const std::string& onlyStudent, bool showStudentColumn)
    {
        std::vector<const Complaint*> list;
        sys.complaints.forEachComplaint([&](const Complaint& c)
        {
            if (onlyStudent.empty() || c.getStudentId() == onlyStudent)
            {
                list.push_back(&c);
            }
        });
        std::sort(list.begin(), list.end(), [](const Complaint* a, const Complaint* b)
        {
            return a->getArrivalOrder() < b->getArrivalOrder();
        });

        table->setRowCount(static_cast<int>(list.size()));
        for (int row = 0; row < static_cast<int>(list.size()); row++)
        {
            const Complaint* c = list[row];
            int col = 0;
            setCell(table, row, col++, q(c->getId()));
            if (showStudentColumn)
            {
                setCell(table, row, col++, q(c->getStudentId()));
            }
            setCell(table, row, col++, q(c->getCategory()));

            QTableWidgetItem* priority = setCell(table, row, col++, q(priorityToString(c->getPriority())));
            priority->setForeground(QBrush(priorityColor(c->getPriority())));
            QFont bold = priority->font();
            bold.setBold(true);
            priority->setFont(bold);

            setCell(table, row, col++, q(statusToString(c->getStatus())));
            setCell(table, row, col++, q(c->getLocation()));
            setCell(table, row, col++, q(c->getAssignedWorkerId().empty() ? "-" : c->getAssignedWorkerId()));
            setCell(table, row, col++, q(c->getDescription()));
        }
    }

    void fillCounts(QTableWidget* table, const DynamicArray<HotspotEntry>& entries)
    {
        table->setRowCount(entries.size());
        for (int i = 0; i < entries.size(); i++)
        {
            setCell(table, i, 0, q(entries[i].name));
            setCell(table, i, 1, QString::number(entries[i].count));
        }
    }
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Hostel Hotspot");

    ready = setupSystem(sys);
    for (int i = 0; i < sys.graph.getVertexCount(); i++)
    {
        locationNames << q(sys.graph.getName(i));
    }
    locationNames.sort();

    stack = new QStackedWidget(this);
    loginPage = buildLoginPage();
    registerPage = buildRegisterPage();
    studentPage = buildStudentPage();
    adminPage = buildAdminPage();

    stack->addWidget(loginPage);
    stack->addWidget(registerPage);
    stack->addWidget(studentPage);
    stack->addWidget(adminPage);
    stack->setCurrentWidget(loginPage);
    setCentralWidget(stack);

    if (!ready)
    {
        QMessageBox::critical(this, "Hostel Hotspot", "Failed to build the campus data. Login is disabled.");
    }
}

// ---------------- Login ----------------

QWidget* MainWindow::buildLoginPage()
{
    QWidget* page = new QWidget;
    QVBoxLayout* outer = new QVBoxLayout(page);

    QGroupBox* box = new QGroupBox;
    box->setMaximumWidth(440);
    QVBoxLayout* layout = new QVBoxLayout(box);

    layout->addWidget(makeTitle("Hostel Hotspot", 22));
    layout->addWidget(makeHint("Complaint and maintenance management"));

    QFormLayout* form = new QFormLayout;
    loginId = new QLineEdit;
    loginPassword = new QLineEdit;
    loginPassword->setEchoMode(QLineEdit::Password);
    form->addRow("User ID", loginId);
    form->addRow("Password", loginPassword);
    layout->addLayout(form);

    loginStatus = new QLabel;
    loginStatus->setStyleSheet("color: #c62828;");
    layout->addWidget(loginStatus);

    QPushButton* loginButton = new QPushButton("Login");
    loginButton->setDefault(true);
    QPushButton* registerButton = new QPushButton("Create student account");
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
    layout->addWidget(makeHint("Demo logins:  admin / admin123   |   S101 / student123"));

    QHBoxLayout* centerRow = new QHBoxLayout;
    centerRow->addStretch();
    centerRow->addWidget(box);
    centerRow->addStretch();

    outer->addStretch();
    outer->addLayout(centerRow);
    outer->addStretch();

    connect(loginButton, &QPushButton::clicked, this, [this]() { doLogin(); });
    connect(loginPassword, &QLineEdit::returnPressed, this, [this]() { doLogin(); });
    connect(registerButton, &QPushButton::clicked, this, [this]()
    {
        regStatus->clear();
        stack->setCurrentWidget(registerPage);
    });

    return page;
}

void MainWindow::doLogin()
{
    if (!ready)
    {
        loginStatus->setText("System not ready.");
        return;
    }

    std::string id = str(loginId->text());
    std::string password = loginPassword->text().toStdString();

    if (!sys.auth.login(id, password))
    {
        loginStatus->setText("Invalid ID or password.");
        return;
    }

    loginStatus->clear();
    loginPassword->clear();
    currentUser = q(id);

    std::optional<Role> role = sys.auth.getRole(id);
    if (role && *role == Role::Admin)
    {
        refreshAdmin();
        stack->setCurrentWidget(adminPage);
    }
    else
    {
        refreshStudent();
        stack->setCurrentWidget(studentPage);
    }
}

void MainWindow::logout()
{
    currentUser.clear();
    stack->setCurrentWidget(loginPage);
    loginId->setFocus();
}

// ---------------- Register ----------------

QWidget* MainWindow::buildRegisterPage()
{
    QWidget* page = new QWidget;
    QVBoxLayout* outer = new QVBoxLayout(page);

    QGroupBox* box = new QGroupBox;
    box->setMaximumWidth(480);
    QVBoxLayout* layout = new QVBoxLayout(box);
    layout->addWidget(makeTitle("Student Registration", 18));

    QFormLayout* form = new QFormLayout;
    regId = new QLineEdit;
    regName = new QLineEdit;
    regPhone = new QLineEdit;
    regHostel = new QComboBox;
    for (int i = 0; i < Campus::HOSTEL_COUNT; i++)
    {
        regHostel->addItem(q(Campus::HOSTEL_NAMES[i]));
    }
    regRoom = new QLineEdit;
    regRoom->setPlaceholderText("e.g. 204  (floor 1-5, room 01-10)");
    regPassword = new QLineEdit;
    regPassword->setEchoMode(QLineEdit::Password);
    regPassword->setPlaceholderText("at least 6 characters");

    form->addRow("Student ID", regId);
    form->addRow("Name", regName);
    form->addRow("Phone", regPhone);
    form->addRow("Hostel", regHostel);
    form->addRow("Room number", regRoom);
    form->addRow("Password", regPassword);
    layout->addLayout(form);

    regStatus = new QLabel;
    regStatus->setStyleSheet("color: #c62828;");
    regStatus->setWordWrap(true);
    layout->addWidget(regStatus);

    QHBoxLayout* buttons = new QHBoxLayout;
    QPushButton* backButton = new QPushButton("Back to login");
    QPushButton* registerButton = new QPushButton("Register");
    registerButton->setDefault(true);
    buttons->addWidget(backButton);
    buttons->addStretch();
    buttons->addWidget(registerButton);
    layout->addLayout(buttons);

    QHBoxLayout* centerRow = new QHBoxLayout;
    centerRow->addStretch();
    centerRow->addWidget(box);
    centerRow->addStretch();

    outer->addStretch();
    outer->addLayout(centerRow);
    outer->addStretch();

    connect(registerButton, &QPushButton::clicked, this, [this]() { doRegister(); });
    connect(backButton, &QPushButton::clicked, this, [this]() { stack->setCurrentWidget(loginPage); });

    return page;
}

void MainWindow::doRegister()
{
    ActionResult result = registerStudent(sys,
                                          str(regId->text()),
                                          str(regName->text()),
                                          str(regPhone->text()),
                                          str(regHostel->currentText()),
                                          str(regRoom->text()),
                                          regPassword->text().toStdString());
    if (!result.success)
    {
        regStatus->setText(q(result.message));
        return;
    }

    QMessageBox::information(this, "Registered", q(result.message));
    loginId->setText(regId->text().trimmed());
    loginPassword->clear();
    regId->clear();
    regName->clear();
    regPhone->clear();
    regRoom->clear();
    regPassword->clear();
    regStatus->clear();
    stack->setCurrentWidget(loginPage);
    loginPassword->setFocus();
}

// ---------------- Student dashboard ----------------

QWidget* MainWindow::buildStudentPage()
{
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);

    QHBoxLayout* top = new QHBoxLayout;
    studentHeader = makeTitle("", 16);
    QPushButton* logoutButton = new QPushButton("Logout");
    top->addWidget(studentHeader);
    top->addStretch();
    top->addWidget(logoutButton);
    layout->addLayout(top);

    QTabWidget* tabs = new QTabWidget;
    layout->addWidget(tabs);

    // Raise complaint
    QWidget* raiseTab = new QWidget;
    QFormLayout* form = new QFormLayout(raiseTab);

    complaintCategory = new QComboBox;
    for (int i = 0; i < PriorityRules::CATEGORY_COUNT; i++)
    {
        complaintCategory->addItem(q(PriorityRules::CATEGORIES[i]));
    }

    complaintDescription = new QTextEdit;
    complaintDescription->setPlaceholderText("e.g. sparking from switch board");
    complaintDescription->setMaximumHeight(120);

    myRoomOption = new QRadioButton("My room");
    otherLocationOption = new QRadioButton("Another campus location:");
    myRoomOption->setChecked(true);
    QButtonGroup* locationGroup = new QButtonGroup(raiseTab);
    locationGroup->addButton(myRoomOption);
    locationGroup->addButton(otherLocationOption);

    otherLocation = new QLineEdit;
    otherLocation->setPlaceholderText("e.g. Mess, Atlantis Hostel 310");
    otherLocation->setEnabled(false);
    otherLocation->setCompleter(makeLocationCompleter(locationNames, this));

    QVBoxLayout* locationBox = new QVBoxLayout;
    locationBox->addWidget(myRoomOption);
    QHBoxLayout* otherRow = new QHBoxLayout;
    otherRow->addWidget(otherLocationOption);
    otherRow->addWidget(otherLocation);
    locationBox->addLayout(otherRow);

    QPushButton* submitButton = new QPushButton("Submit complaint");

    form->addRow("Category", complaintCategory);
    form->addRow("Description", complaintDescription);
    form->addRow("Location", locationBox);
    form->addRow("", submitButton);
    form->addRow("", makeHint("Priority is set automatically from the description "
                              "(e.g. fire, sparking, gas leak, flood = High)."));
    tabs->addTab(raiseTab, "Raise complaint");

    // My complaints
    QWidget* listTab = new QWidget;
    QVBoxLayout* listLayout = new QVBoxLayout(listTab);
    myComplaints = makeTable({"ID", "Category", "Priority", "Status", "Location", "Worker", "Description"});
    QPushButton* refreshButton = new QPushButton("Refresh");
    listLayout->addWidget(myComplaints);
    listLayout->addWidget(refreshButton, 0, Qt::AlignRight);
    tabs->addTab(listTab, "My complaints");

    // Profile
    QWidget* profileTab = new QWidget;
    QVBoxLayout* profileLayout = new QVBoxLayout(profileTab);
    profileLabel = new QLabel;
    profileLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    profileLayout->addWidget(profileLabel);
    profileLayout->addStretch();
    tabs->addTab(profileTab, "Profile");

    connect(logoutButton, &QPushButton::clicked, this, [this]() { logout(); });
    connect(submitButton, &QPushButton::clicked, this, [this]() { submitComplaint(); });
    connect(refreshButton, &QPushButton::clicked, this, [this]() { refreshStudent(); });
    connect(otherLocationOption, &QRadioButton::toggled, otherLocation, &QLineEdit::setEnabled);
    connect(tabs, &QTabWidget::currentChanged, this, [this](int) { refreshStudent(); });

    return page;
}

void MainWindow::submitComplaint()
{
    const std::string studentId = currentUser.toStdString();
    const Student* student = sys.students.findStudent(studentId);
    if (student == nullptr)
    {
        QMessageBox::warning(this, "Error", "Student record not found.");
        return;
    }

    QString description = complaintDescription->toPlainText().trimmed();
    if (description.isEmpty())
    {
        QMessageBox::warning(this, "Missing description", "Please describe the problem.");
        return;
    }

    std::string location = myRoomOption->isChecked()
                               ? student->getHostelName() + " " + student->getRoomNumber()
                               : str(otherLocation->text());

    ActionResult result = raiseComplaint(sys, studentId,
                                         str(complaintCategory->currentText()),
                                         description.toStdString(),
                                         location);
    if (!result.success)
    {
        QMessageBox::warning(this, "Complaint not registered", q(result.message));
        return;
    }

    QMessageBox::information(this, "Complaint registered", q(result.message));
    complaintDescription->clear();
    otherLocation->clear();
    myRoomOption->setChecked(true);
    refreshStudent();
}

void MainWindow::refreshStudent()
{
    const std::string id = currentUser.toStdString();
    const Student* s = sys.students.findStudent(id);
    if (s == nullptr)
    {
        return;
    }

    QString room = q(s->getHostelName() + " " + s->getRoomNumber());
    studentHeader->setText("Welcome, " + q(s->getName()));
    myRoomOption->setText("My room (" + room + ")");

    profileLabel->setText(
        "<b>Student ID:</b> " + q(s->getStudentId()).toHtmlEscaped() + "<br>" +
        "<b>Name:</b> " + q(s->getName()).toHtmlEscaped() + "<br>" +
        "<b>Phone:</b> " + q(s->getPhone()).toHtmlEscaped() + "<br>" +
        "<b>Hostel:</b> " + q(s->getHostelName()).toHtmlEscaped() + "<br>" +
        "<b>Room:</b> " + q(s->getRoomNumber()).toHtmlEscaped());

    fillComplaints(myComplaints, sys, id, false);
}

// ---------------- Admin dashboard ----------------

QWidget* MainWindow::buildAdminPage()
{
    QWidget* page = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(page);

    QHBoxLayout* top = new QHBoxLayout;
    QPushButton* logoutButton = new QPushButton("Logout");
    pendingLabel = new QLabel;
    top->addWidget(makeTitle("Admin Dashboard", 16));
    top->addSpacing(20);
    top->addWidget(pendingLabel);
    top->addStretch();
    top->addWidget(logoutButton);
    layout->addLayout(top);

    QTabWidget* tabs = new QTabWidget;
    layout->addWidget(tabs);

    // Complaints
    QWidget* complaintsTab = new QWidget;
    QVBoxLayout* complaintsLayout = new QVBoxLayout(complaintsTab);

    QHBoxLayout* actions = new QHBoxLayout;
    QPushButton* assignNextButton = new QPushButton("Assign highest priority");
    QPushButton* assignSelectedButton = new QPushButton("Assign selected");
    QPushButton* resolveButton = new QPushButton("Mark selected resolved");
    QPushButton* refreshButton = new QPushButton("Refresh");
    actions->addWidget(assignNextButton);
    actions->addWidget(assignSelectedButton);
    actions->addWidget(resolveButton);
    actions->addStretch();
    actions->addWidget(refreshButton);
    complaintsLayout->addLayout(actions);

    allComplaints = makeTable({"ID", "Student", "Category", "Priority", "Status", "Location", "Worker", "Description"});
    complaintsLayout->addWidget(allComplaints, 3);

    complaintsLayout->addWidget(new QLabel("Activity log"));
    activityLog = new QPlainTextEdit;
    activityLog->setReadOnly(true);
    complaintsLayout->addWidget(activityLog, 1);
    tabs->addTab(complaintsTab, "Complaints");

    // Workers
    QWidget* workersTab = new QWidget;
    QVBoxLayout* workersLayout = new QVBoxLayout(workersTab);
    workersTable = makeTable({"ID", "Name", "Skill", "Location", "Workload", "Duty"});
    QPushButton* toggleButton = new QPushButton("Toggle duty of selected worker");
    workersLayout->addWidget(workersTable);
    workersLayout->addWidget(toggleButton, 0, Qt::AlignRight);

    QGroupBox* addBox = new QGroupBox("Add worker");
    QFormLayout* addForm = new QFormLayout(addBox);
    newWorkerName = new QLineEdit;
    newWorkerSkill = new QComboBox;
    for (int i = 0; i < PriorityRules::CATEGORY_COUNT; i++)
    {
        newWorkerSkill->addItem(q(PriorityRules::CATEGORIES[i]));
    }
    newWorkerLocation = new QLineEdit;
    newWorkerLocation->setPlaceholderText("e.g. Maintenance Office");
    newWorkerLocation->setCompleter(makeLocationCompleter(locationNames, this));
    QPushButton* addButton = new QPushButton("Add worker");
    addForm->addRow("Name", newWorkerName);
    addForm->addRow("Skill", newWorkerSkill);
    addForm->addRow("Location", newWorkerLocation);
    addForm->addRow("", addButton);
    workersLayout->addWidget(addBox);
    tabs->addTab(workersTab, "Workers");

    // Students
    QWidget* studentsTab = new QWidget;
    QVBoxLayout* studentsLayout = new QVBoxLayout(studentsTab);
    studentsTable = makeTable({"ID", "Name", "Phone", "Hostel", "Room"});
    studentsLayout->addWidget(studentsTable);
    tabs->addTab(studentsTab, "Students");

    // Hotspots
    QWidget* hotspotTab = new QWidget;
    QVBoxLayout* hotspotLayout = new QVBoxLayout(hotspotTab);
    hotTotal = makeTitle("", 13);
    hotspotLayout->addWidget(hotTotal);

    QHBoxLayout* hotRow = new QHBoxLayout;
    QGroupBox* locBox = new QGroupBox("By location");
    QGroupBox* hostelBox = new QGroupBox("By hostel");
    QGroupBox* catBox = new QGroupBox("By category");
    hotLocations = makeTable({"Location", "Complaints"});
    hotHostels = makeTable({"Hostel", "Complaints"});
    hotCategories = makeTable({"Category", "Complaints"});
    (new QVBoxLayout(locBox))->addWidget(hotLocations);
    (new QVBoxLayout(hostelBox))->addWidget(hotHostels);
    (new QVBoxLayout(catBox))->addWidget(hotCategories);
    hotRow->addWidget(locBox);
    hotRow->addWidget(hostelBox);
    hotRow->addWidget(catBox);
    hotspotLayout->addLayout(hotRow);
    tabs->addTab(hotspotTab, "Hotspots");

    connect(logoutButton, &QPushButton::clicked, this, [this]() { logout(); });
    connect(refreshButton, &QPushButton::clicked, this, [this]() { refreshAdmin(); });
    connect(tabs, &QTabWidget::currentChanged, this, [this](int) { refreshAdmin(); });

    connect(assignNextButton, &QPushButton::clicked, this, [this]()
    {
        report(assignNextPending(sys));
    });
    connect(assignSelectedButton, &QPushButton::clicked, this, [this]()
    {
        QString id = selectedId(allComplaints);
        if (id.isEmpty())
        {
            log("Select a complaint in the table first.");
            return;
        }
        report(assignComplaint(sys, id.toStdString()));
    });
    connect(resolveButton, &QPushButton::clicked, this, [this]()
    {
        QString id = selectedId(allComplaints);
        if (id.isEmpty())
        {
            log("Select a complaint in the table first.");
            return;
        }
        report(resolveComplaint(sys, id.toStdString()));
    });
    connect(toggleButton, &QPushButton::clicked, this, [this]()
    {
        QString id = selectedId(workersTable);
        if (id.isEmpty())
        {
            QMessageBox::information(this, "Workers", "Select a worker first.");
            return;
        }
        report(toggleWorkerDuty(sys, id.toStdString()));
    });
    connect(addButton, &QPushButton::clicked, this, [this]()
    {
        ActionResult result = addWorker(sys, str(newWorkerName->text()),
                                        str(newWorkerSkill->currentText()),
                                        str(newWorkerLocation->text()));
        if (result.success)
        {
            newWorkerName->clear();
            newWorkerLocation->clear();
        }
        else
        {
            QMessageBox::warning(this, "Add worker", q(result.message));
        }
        report(result);
    });

    return page;
}

void MainWindow::log(const QString& text)
{
    activityLog->appendPlainText(QDateTime::currentDateTime().toString("hh:mm:ss") + "   " + text);
}

void MainWindow::report(const ActionResult& result)
{
    log((result.success ? "OK    " : "FAIL  ") + q(result.message));
    refreshAdmin();
}

void MainWindow::refreshAdmin()
{
    pendingLabel->setText(QString("Pending complaints: %1").arg(sys.complaints.getPendingCount()));
    fillComplaints(allComplaints, sys, "", true);
    refreshWorkers();
    refreshStudents();
    refreshHotspots();
}

void MainWindow::refreshWorkers()
{
    std::vector<const MaintenanceWorker*> list;
    sys.maintenance.forEachWorker([&](const MaintenanceWorker& w) { list.push_back(&w); });
    std::sort(list.begin(), list.end(), [](const MaintenanceWorker* a, const MaintenanceWorker* b)
    {
        return a->getWorkerId() < b->getWorkerId();
    });

    workersTable->setRowCount(static_cast<int>(list.size()));
    for (int row = 0; row < static_cast<int>(list.size()); row++)
    {
        const MaintenanceWorker* w = list[row];
        setCell(workersTable, row, 0, q(w->getWorkerId()));
        setCell(workersTable, row, 1, q(w->getName()));
        setCell(workersTable, row, 2, q(w->getSkill()));
        setCell(workersTable, row, 3, q(w->getLocation()));
        setCell(workersTable, row, 4, QString("%1 / %2").arg(w->getWorkload()).arg(MaintenanceWorker::MAX_WORKLOAD));
        QTableWidgetItem* duty = setCell(workersTable, row, 5, w->isOnDuty() ? "On duty" : "Off duty");
        duty->setForeground(QBrush(w->isOnDuty() ? QColor(46, 125, 50) : QColor(198, 40, 40)));
    }
}

void MainWindow::refreshStudents()
{
    std::vector<const Student*> list;
    sys.students.forEachStudent([&](const Student& s) { list.push_back(&s); });
    std::sort(list.begin(), list.end(), [](const Student* a, const Student* b)
    {
        return a->getStudentId() < b->getStudentId();
    });

    studentsTable->setRowCount(static_cast<int>(list.size()));
    for (int row = 0; row < static_cast<int>(list.size()); row++)
    {
        const Student* s = list[row];
        setCell(studentsTable, row, 0, q(s->getStudentId()));
        setCell(studentsTable, row, 1, q(s->getName()));
        setCell(studentsTable, row, 2, q(s->getPhone()));
        setCell(studentsTable, row, 3, q(s->getHostelName()));
        setCell(studentsTable, row, 4, q(s->getRoomNumber()));
    }
}

void MainWindow::refreshHotspots()
{
    HotspotReport hotspots = buildHotspotReport(sys);
    hotTotal->setText(QString("Total complaints: %1").arg(hotspots.total));
    fillCounts(hotLocations, hotspots.locations);
    fillCounts(hotHostels, hotspots.hostels);
    fillCounts(hotCategories, hotspots.categories);
}

