#include <iostream>
#include <string>

#include "AuthManager.h"
#include "StudentManager.h"

namespace
{
    void displayStudent(const Student& student)
    {
        std::cout << "\n----- Student Details -----\n"
                  << "Student ID : " << student.getStudentId() << '\n'
                  << "Name       : " << student.getName() << '\n'
                  << "Phone      : " << student.getPhone() << '\n'
                  << "Hostel     : " << student.getHostelName() << '\n'
                  << "Room       : " << student.getRoomNumber() << '\n';
    }

    const char* roleName(Role role)
    {
        return role == Role::Admin ? "Admin" : "Student";
    }
}

int main()
{
    StudentManager manager;
    AuthManager auth;

    // Fake demo-only account. Change this password during a live demonstration.
    auth.createAccount("ADMIN", "change-me", Role::Admin);

    std::string choice;
    while (true)
    {
        std::cout << "\n========== HOSTELPULSE STUDENT MANAGEMENT ==========\n"
                  << "1. Register Student\n"
                  << "2. Search Student\n"
                  << "3. Update Student\n"
                  << "4. Remove Student\n"
                  << "5. Display All Students\n"
                  << "6. Total Students\n"
                  << "7. Login\n"
                  << "0. Exit\n"
                  << "Enter your choice: ";
        if (!std::getline(std::cin, choice)) break;

        if (choice == "1")
        {
            std::string id, name, phone, hostel, room, password;
            std::cout << "Enter Student ID: "; std::getline(std::cin, id);
            std::cout << "Enter Name: "; std::getline(std::cin, name);
            std::cout << "Enter Phone: "; std::getline(std::cin, phone);
            std::cout << "Enter Hostel Name: "; std::getline(std::cin, hostel);
            std::cout << "Enter Room Number: "; std::getline(std::cin, room);
            std::cout << "Create Password (minimum 6 characters): "; std::getline(std::cin, password);

            if (id.empty() || name.empty())
            {
                std::cout << "Student ID and name are required.\n";
            }
            else if (password.length() < AuthManager::MIN_PASSWORD_LENGTH)
            {
                std::cout << "Password must contain at least 6 characters.\n";
            }
            else if (manager.addStudent(Student(id, name, phone, hostel, room)))
            {
                if (auth.createAccount(id, password, Role::Student))
                {
                    std::cout << "Student registered and account created successfully.\n";
                }
                else
                {
                    manager.removeStudent(id);
                    std::cout << "Account creation failed; student registration rolled back.\n";
                }
            }
            else
            {
                std::cout << "Error: Student ID already exists or is invalid.\n";
            }
        }
        else if (choice == "2")
        {
            std::string id;
            std::cout << "Enter Student ID to search: "; std::getline(std::cin, id);
            const Student* student = manager.findStudent(id);
            if (student != nullptr) displayStudent(*student);
            else std::cout << "Student not found.\n";
        }
        else if (choice == "3")
        {
            std::string id, name, phone, hostel, room;
            std::cout << "Enter Student ID to update: "; std::getline(std::cin, id);
            std::cout << "Enter New Name: "; std::getline(std::cin, name);
            std::cout << "Enter New Phone: "; std::getline(std::cin, phone);
            std::cout << "Enter New Hostel: "; std::getline(std::cin, hostel);
            std::cout << "Enter New Room Number: "; std::getline(std::cin, room);
            if (manager.updateStudent(id, name, phone, hostel, room))
                std::cout << "Student details updated successfully.\n";
            else
                std::cout << "Student not found or name empty.\n";
        }
        else if (choice == "4")
        {
            std::string id;
            std::cout << "Enter Student ID to remove: "; std::getline(std::cin, id);
            if (manager.removeStudent(id))
            {
                auth.removeAccount(id);
                std::cout << "Student and associated login account removed.\n";
            }
            else std::cout << "Student not found.\n";
        }
        else if (choice == "5")
        {
            if (manager.getTotalStudents() == 0) std::cout << "No student records found.\n";
            else manager.forEachStudent(displayStudent);
        }
        else if (choice == "6")
        {
            std::cout << "Total Students: " << manager.getTotalStudents() << '\n';
        }
        else if (choice == "7")
        {
            std::string id, password;
            std::cout << "User ID: "; std::getline(std::cin, id);
            std::cout << "Password: "; std::getline(std::cin, password);
            if (auth.login(id, password))
            {
                const std::optional<Role> role = auth.getRole(id);
                std::cout << "Login successful (role: " << roleName(*role) << ").\n";
            }
            else std::cout << "Invalid ID or password.\n";
        }
        else if (choice == "0")
        {
            std::cout << "Exiting HostelPulse Student Management.\n";
            break;
        }
        else std::cout << "Invalid choice. Please try again.\n";
    }
    return 0;
}
