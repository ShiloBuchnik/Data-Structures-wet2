#ifndef EMPLOYEEMANAGER_H_
#define EMPLOYEEMANAGER_H_

#include "Employee.h"
#include "Company.h"
#include "EmployeeHash.h"
#include "CompanyUnion.h"
#include "EarnerRankTree.h"
#include "library2.h"

class EmployeeManager {
public:
    CompanyUnion* companies;
    EmployeeHash* employees;
    EarnerRankTree* top_workers;
    int num_companies;

public:
    EmployeeManager(int _num_companies);

    ~EmployeeManager();

    StatusType addEmployee(int employeeID, int companyID, int grade);

    StatusType removeEmployee(int employeeID);

    StatusType acquireCompany(int acquirerID, int targetID, double factor);
    
    StatusType employeeSalaryIncrease(int employeeID, int salaryIncrease);

    StatusType promoteEmployee(int employeeID, int bumpGrade);

    StatusType sumOfBumpGradeBetweenTopWorkersByGroup(int companyID, int m, void* sumBumpGrades);

    StatusType averageBumpGradeBetweenSalaryByGroup(int companyID, int lowerSalary, int higherSalary, void* averageBumpGrade);
};

#endif