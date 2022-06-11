#include "Company.h"

Company::Company(int _ID){
    ID = _ID;
    value = _ID;
    employees = new EmployeeHash();
    owner = nullptr;
    top_workers = new EarnerRankTree();
}

Company::~Company() {
    delete employees;
}