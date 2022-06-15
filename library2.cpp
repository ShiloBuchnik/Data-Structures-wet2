#include "library2.h"
#include "EmployeeManager.h"

void *Init(int k) {
    EmployeeManager* DS;
    try {
        DS = new EmployeeManager(k);
    
    } catch (std::bad_alloc& e) {
		return NULL;
	}

    return (void*) DS;
}

StatusType AddEmployee(void *DS, int employeeID, int companyID, int grade) {
    if (DS == NULL) {
        return INVALID_INPUT;
    }

    return ((EmployeeManager*)DS)->addEmployee(employeeID, companyID, grade);
}

StatusType RemoveEmployee(void *DS, int employeeID) {
    if (DS == NULL || employeeID <= 0) {
        return INVALID_INPUT;
    }

    return ((EmployeeManager*)DS)->removeEmployee(employeeID);
}

StatusType AcquireCompany(void *DS, int companyID1, int companyID2, double factor) {
    if (DS == NULL) {
        return INVALID_INPUT;
    }

    return ((EmployeeManager*)(DS))->acquireCompany(companyID1, companyID2, factor);
}

StatusType EmployeeSalaryIncrease(void *DS, int employeeID, int salaryIncrease) {
    if (DS == NULL) {
        return INVALID_INPUT;
    }

    return ((EmployeeManager*)(DS))->employeeSalaryIncrease(employeeID, salaryIncrease);
}

StatusType PromoteEmployee(void *DS, int employeeID, int bumpGrade){
    if (DS == NULL) {
        return INVALID_INPUT;
    }

    return ((EmployeeManager*)(DS))->promoteEmployee(employeeID, bumpGrade);
}

StatusType SumOfBumpGradeBetweenTopWorkersByGroup(void *DS, int companyID, int m){
    if (DS == NULL) {
        return INVALID_INPUT;
    }

    long long int sumBumpGrade = 0;

    StatusType res = ((EmployeeManager*)(DS))->sumOfBumpGradeBetweenTopWorkersByGroup(companyID, m, &sumBumpGrade);

    if (res != SUCCESS){
        return res;
    }

    printf("SumOfBumpGradeBetweenTopWorkersByGroup: %lld\n", sumBumpGrade);

    return SUCCESS;
}

 StatusType AverageBumpGradeBetweenSalaryByGroup(void *DS, int companyID, int lowerSalary, int higherSalary){
     if (DS == NULL) {
         return INVALID_INPUT;
     }

    double averageBumpGrade = 0;

    StatusType res = ((EmployeeManager*)(DS))->averageBumpGradeBetweenSalaryByGroup(companyID, lowerSalary, higherSalary, &averageBumpGrade);

    if (res != SUCCESS){
        return res;
    }
     
     printf("AverageBumpGradeBetweenSalaryByGroup: %.1lf\n", floor(10 * averageBumpGrade + 0.5f) / (double)10);

     return SUCCESS;
 }

StatusType CompanyValue(void *DS, int companyID) {
    if (!DS) {
        return INVALID_INPUT;
    }

    double standing = 0;

    StatusType res = ((EmployeeManager*)(DS))->companyValue(companyID, &standing);

    if (res != SUCCESS){
        return res;
    }

    printf("CompanyValue: %.1lf\n", floor(10 * standing + 0.5f) / (double)10);

    return SUCCESS;
}

void Quit(void** DS) 
{
    delete (EmployeeManager*)*DS;
    *(DS) = NULL;
}
