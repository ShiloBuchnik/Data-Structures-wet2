#include "Company.h"

Company::Company(int _id){
    id = _id;
    value = _id;
    employees = new EmployeeHash();
    owner = nullptr;
}

Company::~Company() {
    delete employees;
}