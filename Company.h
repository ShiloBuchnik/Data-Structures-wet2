#ifndef COMPANY_H_
#define COMPANY_H_

#include "EmployeeHash.h"
#include "EarnerRankTree.h"

class EarnerRankTree;

class Company {
    public:
        EmployeeHash* employees;
        Company* owner;
        EarnerRankTree* top_workers;
        long double value;
        int ID;
        long long int non_salary_sum_grades;
        int non_salary_count;

        Company(int _ID);
        ~Company();
};

#endif