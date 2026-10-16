#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include <string>
#include <unordered_map>
#include "Student.h"

class StudentManager {
private:
    // Student ID is the key; the Student object is the value.
    std::unordered_map<std::string, Student> students;

public:
    bool addStudent(const Student& student);
    Student* findStudent(const std::string& id);

    bool updateStudent(const std::string& id,
                       const std::string& name,
                       const std::string& phone,
                       const std::string& hostel,
                       const std::string& room);

    bool removeStudent(const std::string& id);
    void displayAllStudents() const;
    std::size_t getTotalStudents() const;
};

#endif
