#include "EmployeeManager.h"
#include <exception>

EmployeeManager::EmployeeManager(int _num_companies): 
    companies(new CompanyUnion(_num_companies)), employees(new EmployeeHash()), top_workers(new EarnerRankTree()), num_companies(_num_companies) {}

EmployeeManager::~EmployeeManager() {
    delete this->top_workers;
    delete this->employees;
}


StatusType EmployeeManager::addEmployee(int employeeID, int companyID, int grade) {
    if (employeeID <= 0 || companyID <= 0 || grade < 0 || companyID > this->num_companies) {
        return INVALID_INPUT;
    }

    Company* company = this->companies->find(companyID)->company;

    if (this->employees->getEmployee(employeeID) || company->employees->getEmployee(employeeID)) {
        return FAILURE;
    }

    try {
        Employee* employee = new Employee(employeeID, company, 0, grade);

        this->employees->addEmployee(employeeID, employee);
        company->employees->addEmployee(employeeID, employee);

    } catch (const std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    } 

    return SUCCESS;
}

StatusType EmployeeManager::removeEmployee(int employeeID) {
    Employee* employee = this->employees->getEmployee(employeeID);

    if (!employee) {
        return FAILURE;
    }

    // We put in a try block, because we allocate in EmployeeHash, in case of a resize
    try {
        SalaryKey key = { employee->salary, employee->ID };
        if (employee->salary > 0) {
            employee->company->top_workers->remove(key);
            this->top_workers->remove(key);
        }

        employee->company->employees->deleteEmployee(employeeID);
        this->employees->deleteEmployee(employeeID);

        delete employee;
    
    } catch (const std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    }

    return SUCCESS;
}


StatusType EmployeeManager::acquireCompany(int acquirerID, int targetID, double factor) {
    if (acquirerID > this->num_companies || targetID > this->num_companies || acquirerID <= 0  || targetID <= 0 || acquirerID == targetID || factor <= 0.0) {
        return INVALID_INPUT;
    }

    CompanyNode* acquirer = this->companies->find(acquirerID);
    CompanyNode* target = this->companies->find(targetID);
    try {
        acquirer->company->top_workers = MergeEarnerRankTrees(acquirer->company->top_workers, target->company->top_workers);
        delete target->company->top_workers;

        EmployeeHash* target_employees = target->company->employees;

        // Raw iteration over elements of hash table, ideally this would be implemented as an iterator of sorts
        for (int i = 0; i < target_employees->max_size; i++) {
            EmployeeNode* list = target_employees->employeeNodes[i];

            if (!list) continue;
            EmployeeNode* node = list->next;

            while (node) {
                node->employee->company = acquirer->company;
                node = node->next;
            }
        }

        this->companies->acquisition(acquirer, target, factor);

    } catch (const std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    }

    return SUCCESS;
}


StatusType EmployeeManager::employeeSalaryIncrease(int employeeID, int salaryIncrease) {
    if (employeeID <= 0 || salaryIncrease <= 0) {
        return INVALID_INPUT;
    }

    Employee* employee = this->employees->getEmployee(employeeID);

    if (!employee) {
        return FAILURE;
    }

    Company* company = employee->company;

    if (employee->salary > 0) {
        SalaryKey old_key = { employee->salary, employee->ID };
        company->top_workers->remove(old_key);
        this->top_workers->remove(old_key);
    }

    employee->salary += salaryIncrease;

    SalaryKey new_key = { employee->salary, employee->ID };
    try {
        company->top_workers->insert(new_key, employee);
        this->top_workers->insert(new_key, employee);
    
    } catch (const std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    }

    return SUCCESS;
}


StatusType EmployeeManager::promoteEmployee(int employeeID, int bumpGrade){
    if (employeeID <= 0) {
        return INVALID_INPUT;
    }

    Employee* employee = this->employees->getEmployee(employeeID);

    if (!employee) {
        return FAILURE;
    }

    if (bumpGrade > 0){
        employee->grade += bumpGrade;
    }
    
    // Update 'sum_grade' property of relevant rank trees if the employee has a non-zero salary
    if (bumpGrade > 0 && employee->salary > 0) {
        EarnerRankTree* tree[] = { this->top_workers, employee->company->top_workers };
        
        for (int i = 0; i < 2; i++) {
            EarnerTreeNode* node = tree[i]->find({ employee->salary, employee->ID });
            
            // Note that 'node' cannot be empty due to manner in which the earner trees are maintained
            while (node) {
                node->sum_grades += bumpGrade;
                node = node->parent;
            }
        }
    }

    return SUCCESS;
}

StatusType EmployeeManager::sumOfBumpGradeBetweenTopWorkersByGroup(int companyID, int m, void* sumBumpGrades) {
    if (!sumBumpGrades || companyID < 0 || companyID > this->num_companies || m <= 0){
        return INVALID_INPUT;
    }

    EarnerRankTree* tree;

    if (companyID == 0){ // Global tree
        tree = this->top_workers;
    }
    else{ // Company tree
        Company* company = this->companies->find(companyID)->company;
        tree = company->top_workers;
    }

    if (tree->getSize() < m){
        return FAILURE;
    }

    int remainder_m = m, bump_sum = 0;
    EarnerTreeNode* node = tree->getRoot();

    while (remainder_m && node != nullptr){
        if (node->isLeaf()){
            bump_sum += node->value->grade;
            break;
        }

        if (node->right->top_workers_count > remainder_m) node = node->right;

        else if (node->right->top_workers_count == remainder_m){
            bump_sum += node->right->sum_grades;
            remainder_m = 0;
        }

        else{
            bump_sum += node->sum_grades - node->left->sum_grades;
            remainder_m -= 1 + node->right->top_workers_count;
            node = node->left;
        }
    }

    *((int*)sumBumpGrades) = bump_sum;

    return SUCCESS;
}

/*
// Moral compass
remainder_m = m

bump_sum = 0

while remainder_m > 0 and node != null:

    if node->right->highestEarnerCount > remainder_m:
        node = node->right

    elif (node->right->highestEarnerCount == remainder_m):
        sum += node->right->sumHighestEmployeeGrades
        remainder_m = 0

    else:
        sum += node->sumHighestEmployeeGrades - node->left->sumHighestEmployeeGrades
        remainder_m -= 1 + node->right->highestEarnerCount
        node = node->left
*/

StatusType EmployeeManager::averageBumpGradeBetweenSalaryByGroup(int companyID, int lowerSalary, int higherSalary, void* averageBumpGrade){
    if (!averageBumpGrade || higherSalary < 0 || lowerSalary < 0 || lowerSalary > higherSalary ||
    companyID < 0 || companyID > this->num_companies){
        return INVALID_INPUT;
    }

    EarnerRankTree* tree;

    if (companyID == 0){
        tree = this->top_workers;
    }
    else{
        Company* company = this->companies->find(companyID)->company;
        tree = company->top_workers;
    }

    EarnerTreeNode* node = tree->getRoot();

}



/*
void EmployeeManager::createBoundedRangeEmployee(int min, int max, Employee** array, int* index, EmployeeTree* tree) {
    createBoundedRangeEmployeeAux(min, max, array, index, tree->getRoot());

}

void EmployeeManager::createBoundedRangeEmployeeAux(int min, int max, Employee** array, int* index, EmployeeNode* node) {
    if (!node) {
        return;
    }

    int key = node->getKey();
    bool key_in_range = key >= min && key <= max;

    if (key < min || key_in_range) {
        this->createBoundedRangeEmployeeAux(min, max, array, index, node->getRight());
    }

    if (key_in_range) {
        array[*index] = node->getValue();
        ++(*index);
    }

    if (key > max || key_in_range) {
        this->createBoundedRangeEmployeeAux(min, max, array, index, node->getLeft());
    }
} */
