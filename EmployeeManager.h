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
    long long int non_salary_sum_grades;
    int non_salary_count;

public:
    EmployeeManager(int _num_companies);

    ~EmployeeManager();

    StatusType addEmployee(int employeeID, int companyID, long long int grade);

    StatusType removeEmployee(int employeeID);

    StatusType acquireCompany(int acquirerID, int targetID, long double factor);
    
    StatusType employeeSalaryIncrease(int employeeID, long long int salaryIncrease);

    StatusType promoteEmployee(int employeeID, long long int bumpGrade);

    StatusType sumOfBumpGradeBetweenTopWorkersByGroup(int companyID, int m, void* sumBumpGrades);

    StatusType averageBumpGradeBetweenSalaryByGroup(int companyID, long long int lowerSalary, long long int higherSalary, void* averageBumpGrade);

    StatusType companyValue(int companyID, void* standing);

    EarnerTreeNode* findMinAboveLimit(EarnerTreeNode* node, long long int minSalary);

    EarnerTreeNode* findMaxBelowLimit(EarnerTreeNode* node, long long int maxSalary);
};

#endif