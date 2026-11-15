#ifndef STUDENT_H
#define STUDENT_H

#include <string>

class Student
{
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

    const std::string& getStudentId() const;
    const std::string& getName() const;
    const std::string& getPhone() const;
    const std::string& getHostelName() const;
    const std::string& getRoomNumber() const;

    void updateDetails(const std::string& studentName,
                       const std::string& contact,
                       const std::string& hostel,
                       const std::string& room);
};

#endif
