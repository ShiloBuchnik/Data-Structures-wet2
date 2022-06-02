#ifndef EMPLOYEE_H_
#define EMPLOYEE_H_
#include "Company.h"

class Company;

class Employee {
    public:
        int ID;
        Company* company;
        int salary;
        int grade;

        Employee(int _ID, Company* _company, int _salary, int _grade): ID(_ID), company(_company), salary(_salary), grade(_grade) {}
};
#endif