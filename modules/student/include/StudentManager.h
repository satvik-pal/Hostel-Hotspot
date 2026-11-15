#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include <string>
#include "HashMap.h"
#include "Student.h"

class StudentManager
{
private:
    HashMap<Student> students;

public:
    bool addStudent(const Student& student);
    Student* findStudent(const std::string& id);
    const Student* findStudent(const std::string& id) const;

    bool updateStudent(const std::string& id,
                       const std::string& name,
                       const std::string& phone,
                       const std::string& hostel,
                       const std::string& room);

    bool removeStudent(const std::string& id);
    int getTotalStudents() const;

    template <typename Action>
    void forEachStudent(Action action) const
    {
        students.forEach([&](const std::string&, const Student& student)
        {
            action(student);
        });
    }
};

#endif
