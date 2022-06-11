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
        double value;
        int ID;

    public:
        Company(int _ID);

        ~Company();
};

#endif