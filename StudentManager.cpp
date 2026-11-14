#include "StudentManager.h"

bool StudentManager::addStudent(const Student& student)
{
    const std::string& id = student.getStudentId();
    if (id.empty())
    {
        return false;
    }
    return students.insert(id, student);
}

Student* StudentManager::findStudent(const std::string& id)
{
    return students.find(id);
}

const Student* StudentManager::findStudent(const std::string& id) const
{
    return students.find(id);
}

bool StudentManager::updateStudent(const std::string& id,
                                   const std::string& name,
                                   const std::string& phone,
                                   const std::string& hostel,
                                   const std::string& room)
{
    Student* student = findStudent(id);
    if (student == nullptr || name.empty())
    {
        return false;
    }
    student->updateDetails(name, phone, hostel, room);
    return true;
}

bool StudentManager::removeStudent(const std::string& id)
{
    return students.erase(id);
}

int StudentManager::getTotalStudents() const
{
    return students.size();
}
