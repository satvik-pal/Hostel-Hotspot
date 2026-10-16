#ifndef STUDENT_H
#define STUDENT_H

#include <string>

class Student {
private:
    std::string studentId;
    std::string name;
    std::string phone;
    std::string hostelName;
    std::string roomNumber;

public:
    Student();
    Student(const std::string& id,
            const std::string& studentName,
            const std::string& contact,
            const std::string& hostel,
            const std::string& room);

    std::string getStudentId() const;
    std::string getName() const;
    std::string getPhone() const;
    std::string getHostelName() const;
    std::string getRoomNumber() const;

    void updateDetails(const std::string& studentName,
                       const std::string& contact,
                       const std::string& hostel,
                       const std::string& room);

    void display() const;
};

#endif
