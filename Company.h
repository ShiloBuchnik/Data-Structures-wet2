#ifndef COMPANY_H_
#define COMPANY_H_

#include "Employees.h"

class EmployeeHash;

class Company {
    public:
        EmployeeHash* employees;
        Company* owner;
        double value;
        int id;

    public:
        Company(int _id);

        ~Company();
};

#endif