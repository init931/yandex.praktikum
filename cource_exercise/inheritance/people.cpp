#include "people.h"

#include <stdexcept>

using namespace std;

const std::string& Person::GetName() const {
    return name_;
}

int Person::GetAge() const {
    return age_;
}

Gender Person::GetGender() const {
    return gender_;
}

void Programmer::AddProgrammingLanguage(ProgrammingLanguage language) {
    // Заглушка, реализуйте метод самостоятельно
    langs_.insert(language);
}

bool Programmer::CanProgram(ProgrammingLanguage language) const {
    // Заглушка, реализуйте метод самостоятельно
    //return false;
    return langs_.count(language) > 0;
}

void Worker::AddSpeciality(WorkerSpeciality speciality) {
    // Заглушка, реализуйте метод самостоятельно
    ws_.insert(speciality);
}

bool Worker::HasSpeciality(WorkerSpeciality speciality) const {
    // Заглушка, реализуйте метод самостоятельно
    //return false;
    return ws_.count(speciality) > 0;
}
