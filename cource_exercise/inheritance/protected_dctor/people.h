#pragma once

#include <string>

class Person;

// Наблюдатель за состоянием человека.
class PersonObserver {
public:
    // Этот метод вызывается, когда меняется состояние удовлетворённости человека
    virtual void OnSatisfactionChanged(Person& /*person*/, int /*old_value*/, int /*new_value*/) {
        // Реализация метода базового класса ничего не делает
    }

protected:
    // Класс PersonObserver не предназначен для удаления напрямую
    ~PersonObserver() = default;
};

/*
    Человек.
    При изменении уровня удовлетворённости уведомляет
    связанного с ним наблюдателя
*/
class Person {
public:
    Person(const std::string& name, int age)
        : name_(name), age_(age), dance_count_(0), satisfaction_base_(1) {
        
        // Напишите реализацию самостоятельно
    }

    int GetSatisfaction() const {
        return satisfaction_;
    }

    const std::string& GetName() const {
        return name_;
    }

    // «Привязывает» наблюдателя к человеку. Привязанный наблюдатель
    // уведомляется об изменении уровня удовлетворённости человека
    // Новый наблюдатель заменяет собой ранее привязанного
    // Если передать nullptr в качестве наблюдателя, это эквивалентно отсутствию наблюдателя
    void SetObserver(PersonObserver* observer) {
        // Напишите реализацию самостоятельно
        observer_ = observer;
    }

    int GetAge() const {
        return age_;
    }

    // Увеличивает на 1 количество походов на танцы
    // Увеличивает удовлетворённость на 1
    virtual void Dance() {
        // Напишите тело метода самостоятельно
        ++dance_count_;
        SetSatisfaction(satisfaction_ + satisfaction_base_);
    }

    int GetDanceCount() const {
        // Заглушка. Напишите реализацию самостоятельно
        //return 0;
        return dance_count_;
    }

    // Прожить день. Реализация в базовом классе ничего не делает
    virtual void LiveADay() {
        // Подклассы могут переопределить этот метод
    }

    void SetSatisfactionBase(int val) {
        satisfaction_base_ = val;
    }

    virtual ~Person() = default;

protected:
    void SetSatisfaction(int satisfaction) {
        int old = satisfaction_;
        satisfaction_ = satisfaction;
        
        if (observer_) {
            observer_->OnSatisfactionChanged(*this, old, satisfaction_);
        }
    }

private:
    std::string name_;
    PersonObserver* observer_ = nullptr;
    int satisfaction_ = 100;
    int age_;
    int dance_count_;
    int satisfaction_base_;
};

// Рабочий.
// День рабочего проходит за работой
class Worker: public Person {
public:
    Worker(const std::string& name, int age) 
        : Person(name, age) {

        // Напишите недостающий код
    }

    // Рабочий старше 30 лет и младше 40 за танец получает 2 единицы удовлетворённости вместо 1
    void Dance() override {
        if (GetAge() > 30 && GetAge() < 40) {
            SetSatisfactionBase(2);
        }
        Person::Dance();
    }

    // День рабочего проходит за работой
    void LiveADay() override {
        Work();
    }

    // Увеличивает счётчик сделанной работы на 1, уменьшает удовлетворённость на 5
    void Work() {
        // Напишите тело метода самостоятельно
        SetSatisfaction(GetSatisfaction() - 5);
        ++work_point_;
    }

    // Возвращает значение счётчика сделанной работы
    int GetWorkDone() const {
        // Заглушка. Напишите реализацию самостоятельно
        //return 0;
        return work_point_;
    }

private:
    int work_point_ = 0;
};

// Студент.
// День студента проходит за учёбой
class Student: public Person {
public:
    Student(const std::string& name, int age) 
        : Person(name, age) {

        // Напишите недостающий код
    }

    void Dance() override {
        Person::Dance();
    }

    // День студента проходит за учёбой
    void LiveADay() override {
        Study();
    }

    // Учёба увеличивает уровень знаний на 1, уменьшает уровень удовлетворённости на 3
    void Study() {
        // Напишите реализацию самостоятельно
        SetSatisfaction(GetSatisfaction() - 3);
        ++study_point_;
    }

    // Возвращает уровень знаний
    int GetKnowledgeLevel() const {
        // Заглушка, напишите реализацию самостоятельно
        //return 0;
        return study_point_;
    }

private:
    int study_point_ = 0;
};
