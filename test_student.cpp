#include <iostream>
#include <string>

#include "AuthManager.h"
#include "StudentManager.h"

namespace
{
    int passed = 0;
    int failed = 0;

    void check(bool condition, const std::string& description)
    {
        if (condition)
        {
            ++passed;
            std::cout << "[PASS] " << description << '\n';
        }
        else
        {
            ++failed;
            std::cout << "[FAIL] " << description << '\n';
        }
    }

    void section(const std::string& name)
    {
        std::cout << "\n--- " << name << " ---\n";
    }
}

int main()
{
    section("Student");
    {
        Student student("S1", "Asha", "111", "Hostel A", "101");
        check(student.getStudentId() == "S1" && student.getName() == "Asha" &&
              student.getPhone() == "111" && student.getHostelName() == "Hostel A" &&
              student.getRoomNumber() == "101", "Constructor and getters preserve all fields");
        student.updateDetails("Asha K", "222", "Hostel B", "202");
        check(student.getStudentId() == "S1" && student.getName() == "Asha K" &&
              student.getPhone() == "222" && student.getHostelName() == "Hostel B" &&
              student.getRoomNumber() == "202", "updateDetails changes details but not ID");
        Student empty;
        check(empty.getStudentId().empty(), "Default constructor gives empty ID");
    }

    section("StudentManager");
    {
        StudentManager manager;
        check(manager.getTotalStudents() == 0, "New manager has zero students");
        check(manager.addStudent(Student("S1", "Asha", "111", "Hostel A", "101")) &&
              manager.getTotalStudents() == 1, "Add new student and count becomes one");
        check(!manager.addStudent(Student("S1", "Changed", "999", "X", "9")) &&
              manager.findStudent("S1")->getName() == "Asha", "Duplicate ID rejected; original unchanged");
        check(!manager.addStudent(Student("", "No ID", "", "", "")), "Empty ID rejected");
        check(manager.findStudent("S1") != nullptr && manager.findStudent("missing") == nullptr,
              "Find existing and missing students");
        const StudentManager& constManager = manager;
        check(constManager.findStudent("S1") != nullptr && constManager.findStudent("missing") == nullptr,
              "Const findStudent overload works");
        check(manager.updateStudent("S1", "Asha Updated", "333", "Hostel C", "303") &&
              manager.findStudent("S1")->getName() == "Asha Updated", "Update existing student");
        check(!manager.updateStudent("S1", "", "444", "Hostel D", "404") &&
              manager.findStudent("S1")->getName() == "Asha Updated", "Empty name rejected without changes");
        check(!manager.updateStudent("missing", "Name", "", "", ""), "Update missing ID rejected");
        check(manager.addStudent(Student("S2", "Bala", "", "", "")), "Add second student for removal test");
        check(manager.removeStudent("S1") && !manager.removeStudent("S1") &&
              manager.findStudent("S2") != nullptr, "Remove existing once; other student unaffected");
        int visited = 0;
        manager.forEachStudent([&](const Student&) { ++visited; });
        check(visited == manager.getTotalStudents(), "forEachStudent visits every student exactly once");
    }
    {
        StudentManager largeManager;
        bool allAdded = true;
        for (int i = 0; i < 1000; ++i)
        {
            const std::string id = "S" + std::to_string(i);
            if (!largeManager.addStudent(Student(id, "Student", "", "", ""))) allAdded = false;
        }
        bool allFound = true;
        for (int i = 0; i < 1000; ++i)
        {
            const std::string id = "S" + std::to_string(i);
            if (largeManager.findStudent(id) == nullptr) allFound = false;
        }
        check(allAdded && allFound && largeManager.getTotalStudents() == 1000,
              "1000 students insert/find successfully across HashMap resizing");
    }

    section("AuthManager");
    {
        AuthManager auth;
        check(auth.createAccount("U1", "secret1", Role::Student) && auth.getAccountCount() == 1,
              "Create valid account and count it");
        check(!auth.createAccount("U1", "secret2", Role::Admin) &&
              !auth.createAccount("", "secret2", Role::Student) &&
              !auth.createAccount("U2", "12345", Role::Student),
              "Reject duplicate ID, empty ID and short password");
        check(auth.login("U1", "secret1") && !auth.login("U1", "wrongpw") &&
              !auth.login("unknown", "secret1"), "Login accepts correct and rejects wrong/unknown credentials");
        const std::optional<Role> studentRole = auth.getRole("U1");
        check(studentRole.has_value() && *studentRole == Role::Student && !auth.getRole("unknown").has_value(),
              "getRole returns role or empty optional");
        const std::string hash = AuthManager::hashPassword("secret1", "U1");
        check(hash.length() == 16 && hash != "secret1", "Password hash is 16 hex characters and not plaintext");
        check(AuthManager::hashPassword("secret1", "U1") != AuthManager::hashPassword("secret1", "U2"),
              "Different salts produce different hashes");
        check(AuthManager::hashPassword("secret1", "U1") == AuthManager::hashPassword("secret1", "U1"),
              "Same password and salt produce deterministic hash");
        check(!auth.changePassword("U1", "wrongpw", "newpass") &&
              auth.changePassword("U1", "secret1", "newpass"), "Password change validates old password");
        check(!auth.login("U1", "secret1") && auth.login("U1", "newpass"),
              "After password change only new password works");
        check(auth.createAccount("ADMIN2", "adminpw", Role::Admin), "Create Admin account");
        const std::optional<Role> adminRole = auth.getRole("ADMIN2");
        check(adminRole.has_value() && *adminRole == Role::Admin, "Admin role is returned correctly");
        check(auth.removeAccount("U1") && !auth.login("U1", "newpass") && !auth.removeAccount("U1"),
              "Remove account disables login and second removal fails");
    }

    std::cout << "\nResult: " << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
