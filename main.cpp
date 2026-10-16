#include <iostream>
#include <string>
#include "StudentManager.h"

int main() {
    StudentManager manager;
    std::string choice;

    while (true) {
        std::cout << "\n========== STUDENT MANAGEMENT ==========\n"
                  << "1. Register Student\n"
                  << "2. Search Student\n"
                  << "3. Update Student\n"
                  << "4. Remove Student\n"
                  << "5. Display All Students\n"
                  << "6. Total Students\n"
                  << "0. Exit\n"
                  << "Enter your choice: ";

        std::getline(std::cin, choice);

        if (choice == "1") {
            std::string id, name, phone, hostel, room;

            std::cout << "Enter Student ID: ";
            std::getline(std::cin, id);
            std::cout << "Enter Name: ";
            std::getline(std::cin, name);
            std::cout << "Enter Phone: ";
            std::getline(std::cin, phone);
            std::cout << "Enter Hostel Name: ";
            std::getline(std::cin, hostel);
            std::cout << "Enter Room Number: ";
            std::getline(std::cin, room);

            if (id.empty() || name.empty()) {
                std::cout << "Student ID and name are required.\n";
                continue;
            }

            manager.addStudent(Student(id, name, phone, hostel, room));
        } else if (choice == "2") {
            std::string id;
            std::cout << "Enter Student ID to search: ";
            std::getline(std::cin, id);

            const Student* student = manager.findStudent(id);
            if (student != nullptr) {
                student->display();
            } else {
                std::cout << "Student not found.\n";
            }
        } else if (choice == "3") {
            std::string id, name, phone, hostel, room;
            std::cout << "Enter Student ID to update: ";
            std::getline(std::cin, id);

            if (manager.findStudent(id) == nullptr) {
                std::cout << "Student not found.\n";
                continue;
            }

            std::cout << "Enter New Name: ";
            std::getline(std::cin, name);
            std::cout << "Enter New Phone: ";
            std::getline(std::cin, phone);
            std::cout << "Enter New Hostel: ";
            std::getline(std::cin, hostel);
            std::cout << "Enter New Room Number: ";
            std::getline(std::cin, room);

            manager.updateStudent(id, name, phone, hostel, room);
        } else if (choice == "4") {
            std::string id;
            std::cout << "Enter Student ID to remove: ";
            std::getline(std::cin, id);
            manager.removeStudent(id);
        } else if (choice == "5") {
            manager.displayAllStudents();
        } else if (choice == "6") {
            std::cout << "Total Students: " << manager.getTotalStudents() << '\n';
        } else if (choice == "0") {
            std::cout << "Exiting Student Management System.\n";
            break;
        } else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
