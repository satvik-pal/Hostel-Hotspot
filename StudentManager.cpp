#include <iostream>
#include "StudentManager.h"

bool StudentManager::addStudent(const Student& student) {
    const std::string id = student.getStudentId();

    if (id.empty()) {
        std::cout << "Error: Student ID cannot be empty.\n";
        return false;
    }

    const auto result = students.emplace(id, student);
    if (!result.second) {
        std::cout << "Error: Student ID already exists.\n";
        return false;
    }

    std::cout << "Student registered successfully.\n";
    return true;
}

Student* StudentManager::findStudent(const std::string& id) {
    const auto it = students.find(id);
    if (it == students.end()) {
        return nullptr;
    }
    return &(it->second);
}

bool StudentManager::updateStudent(const std::string& id,
                                   const std::string& name,
                                   const std::string& phone,
                                   const std::string& hostel,
                                   const std::string& room) {
    Student* student = findStudent(id);
    if (student == nullptr) {
        std::cout << "Student not found.\n";
        return false;
    }

    if (name.empty()) {
        std::cout << "Error: Name cannot be empty.\n";
        return false;
    }

    student->updateDetails(name, phone, hostel, room);
    std::cout << "Student details updated successfully.\n";
    return true;
}

bool StudentManager::removeStudent(const std::string& id) {
    const std::size_t removed = students.erase(id);
    if (removed == 0) {
        std::cout << "Student not found.\n";
        return false;
    }

    std::cout << "Student removed successfully.\n";
    return true;
}

void StudentManager::displayAllStudents() const {
    if (students.empty()) {
        std::cout << "No student records found.\n";
        return;
    }

    std::cout << "\n===== All Registered Students =====\n";
    for (const auto& entry : students) {
        entry.second.display();
    }
}

std::size_t StudentManager::getTotalStudents() const {
    return students.size();
}
