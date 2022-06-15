#include "Company.h"

Company::Company(int _ID){
    ID = _ID;
    employees = new EmployeeHash();
    owner = nullptr;
    top_workers = new EarnerRankTree();
    non_salary_sum_grades = 0;
    non_salary_count = 0;
}

Company::~Company() {
    delete employees;
    delete top_workers;
}