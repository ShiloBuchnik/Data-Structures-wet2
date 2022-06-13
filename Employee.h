#ifndef EMPLOYEE_H_
#define EMPLOYEE_H_
#include "Company.h"

class Company;

class Employee {
    public:
        int ID;
        Company* company;
        long long int salary;
        long long int grade;

        Employee(int _ID, Company* _company, long long int _salary, long long int _grade);
};
#endif