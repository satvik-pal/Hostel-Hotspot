#include "Student.h"

Student::Student()
    : studentId(""), name(""), phone(""), hostelName(""), roomNumber("")
{
}

Student::Student(const std::string& id,
                 const std::string& studentName,
                 const std::string& contact,
                 const std::string& hostel,
                 const std::string& room)
    : studentId(id),
      name(studentName),
      phone(contact),
      hostelName(hostel),
      roomNumber(room)
{
}

const std::string& Student::getStudentId() const { return studentId; }
const std::string& Student::getName() const { return name; }
const std::string& Student::getPhone() const { return phone; }
const std::string& Student::getHostelName() const { return hostelName; }
const std::string& Student::getRoomNumber() const { return roomNumber; }

void Student::updateDetails(const std::string& studentName,
                            const std::string& contact,
                            const std::string& hostel,
                            const std::string& room)
{
    name = studentName;
    phone = contact;
    hostelName = hostel;
    roomNumber = room;
}
