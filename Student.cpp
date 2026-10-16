#include <iostream>
#include "Student.h"

Student::Student()
    : studentId(""), name(""), phone(""), hostelName(""), roomNumber("") {}

Student::Student(const std::string& id,
                 const std::string& studentName,
                 const std::string& contact,
                 const std::string& hostel,
                 const std::string& room)
    : studentId(id),
      name(studentName),
      phone(contact),
      hostelName(hostel),
      roomNumber(room) {}

std::string Student::getStudentId() const { return studentId; }
std::string Student::getName() const { return name; }
std::string Student::getPhone() const { return phone; }
std::string Student::getHostelName() const { return hostelName; }
std::string Student::getRoomNumber() const { return roomNumber; }

void Student::updateDetails(const std::string& studentName,
                            const std::string& contact,
                            const std::string& hostel,
                            const std::string& room) {
    name = studentName;
    phone = contact;
    hostelName = hostel;
    roomNumber = room;
}

void Student::display() const {
    std::cout << "\n----- Student Details -----\n"
              << "Student ID : " << studentId << '\n'
              << "Name       : " << name << '\n'
              << "Phone      : " << phone << '\n'
              << "Hostel     : " << hostelName << '\n'
              << "Room       : " << roomNumber << '\n';
}
