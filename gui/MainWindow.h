#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>

#include "AppCore.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QRadioButton;
class QStackedWidget;
class QTableWidget;
class QTextEdit;

// One window with four pages: Login, Register, Student dashboard, Admin dashboard.
class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    HostelSystem sys;
    bool ready = false;
    QString currentUser;
    QStringList locationNames;

    QStackedWidget* stack = nullptr;
    QWidget* loginPage = nullptr;
    QWidget* registerPage = nullptr;
    QWidget* studentPage = nullptr;
    QWidget* adminPage = nullptr;

    // Login page
    QLineEdit* loginId = nullptr;
    QLineEdit* loginPassword = nullptr;
    QLabel* loginStatus = nullptr;

    // Register page
    QLineEdit* regId = nullptr;
    QLineEdit* regName = nullptr;
    QLineEdit* regPhone = nullptr;
    QComboBox* regHostel = nullptr;
    QLineEdit* regRoom = nullptr;
    QLineEdit* regPassword = nullptr;
    QLabel* regStatus = nullptr;

    // Student page
    QLabel* studentHeader = nullptr;
    QComboBox* complaintCategory = nullptr;
    QTextEdit* complaintDescription = nullptr;
    QRadioButton* myRoomOption = nullptr;
    QRadioButton* otherLocationOption = nullptr;
    QLineEdit* otherLocation = nullptr;
    QTableWidget* myComplaints = nullptr;
    QLabel* profileLabel = nullptr;

    // Admin page
    QLabel* pendingLabel = nullptr;
    QTableWidget* allComplaints = nullptr;
    QPlainTextEdit* activityLog = nullptr;
    QTableWidget* workersTable = nullptr;
    QLineEdit* newWorkerName = nullptr;
    QComboBox* newWorkerSkill = nullptr;
    QLineEdit* newWorkerLocation = nullptr;
    QTableWidget* studentsTable = nullptr;
    QLabel* hotTotal = nullptr;
    QTableWidget* hotLocations = nullptr;
    QTableWidget* hotHostels = nullptr;
    QTableWidget* hotCategories = nullptr;

    QWidget* buildLoginPage();
    QWidget* buildRegisterPage();
    QWidget* buildStudentPage();
    QWidget* buildAdminPage();

    void doLogin();
    void doRegister();
    void logout();

    void submitComplaint();
    void refreshStudent();

    void refreshAdmin();
    void refreshWorkers();
    void refreshStudents();
    void refreshHotspots();
    void report(const ActionResult& result);
    void log(const QString& text);
};

#endif

