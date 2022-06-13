#include "EmployeeManager.h"
#include <exception>
#include <climits>

EmployeeManager::EmployeeManager(int _num_companies): 
    companies(new CompanyUnion(_num_companies)), employees(new EmployeeHash()), top_workers(new EarnerRankTree()), num_companies(_num_companies) {}

EmployeeManager::~EmployeeManager() {
    delete this->top_workers;
    delete this->employees;
    delete this->companies;
}


StatusType EmployeeManager::addEmployee(int employeeID, int companyID, long long int grade) {
    if (employeeID <= 0 || companyID <= 0 || grade < 0 || companyID > this->num_companies) {
        return INVALID_INPUT;
    }

    Company* company = this->companies->company(companyID);

    if (this->employees->getEmployee(employeeID) || company->employees->getEmployee(employeeID)) {
        return FAILURE;
    }

    try {
        Employee* employee = new Employee(employeeID, company, 0, grade);

        this->employees->addEmployee(employeeID, employee);
        company->employees->addEmployee(employeeID, employee);

        this->non_salary_sum_grades += grade;
        ++this->non_salary_count;

        company->non_salary_sum_grades += grade;
        ++company->non_salary_count;

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
        else{
            this->non_salary_sum_grades -= employee->grade;
            --this->non_salary_count;

            employee->company->non_salary_sum_grades -= employee->grade;
            --employee->company->non_salary_count;
        }

        employee->company->employees->deleteEmployee(employeeID);
        this->employees->deleteEmployee(employeeID);

        delete employee;
    
    } catch (const std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    }

    return SUCCESS;
}


StatusType EmployeeManager::acquireCompany(int acquirerID, int targetID, long double factor) {
    if (acquirerID > this->num_companies || targetID > this->num_companies || acquirerID <= 0  || targetID <= 0 || acquirerID == targetID || factor <= 0.0) {
        return INVALID_INPUT;
    }

    CompanyNode* acquirer = this->companies->find(acquirerID);
    CompanyNode* target = this->companies->find(targetID);
    try {
        if (acquirerID == 69 && targetID == 70){
            std::cout << "WE FIGHT";
        }
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

        acquirer->company->non_salary_sum_grades += target->company->non_salary_sum_grades;
        acquirer->company->non_salary_count += target->company->non_salary_count;
        target->company->non_salary_sum_grades = 0;
        target->company->non_salary_count = 0;

        this->companies->acquisition(acquirer, target, factor);

    } catch (const std::bad_alloc& e) {
        return ALLOCATION_ERROR;
    }

    return SUCCESS;
}


StatusType EmployeeManager::employeeSalaryIncrease(int employeeID, long long int salaryIncrease) {
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
    else{
        this->non_salary_sum_grades -= employee->grade;
        --this->non_salary_count;

        company->non_salary_sum_grades -= employee->grade;
        --company->non_salary_count;
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


StatusType EmployeeManager::promoteEmployee(int employeeID, long long int bumpGrade){
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

    if (bumpGrade > 0 && employee->salary == 0){
        this->non_salary_sum_grades += bumpGrade;
        employee->company->non_salary_sum_grades += bumpGrade;
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
        Company* company = this->companies->company(companyID);
        tree = company->top_workers;
    }

    if (tree->getSize() < m){
        return FAILURE;
    }

    int remainder_m = m;
    long long int bump_sum = 0;

    EarnerTreeNode* node = tree->getRoot();

    while (remainder_m && node != nullptr){
        if (node->isLeaf()){
            bump_sum += node->value->grade;
            break;
        }

        if (!node->right){
            bump_sum += node->value->grade;
            --remainder_m;
            node = node->left;
            continue;
        }

        if (node->right->top_workers_count > remainder_m) node = node->right;

        else if (node->right->top_workers_count == remainder_m){
            bump_sum += node->right->sum_grades;
            remainder_m = 0;
        }

        else{
            if (!node->left){
                bump_sum += node->value->grade + node->right->sum_grades;
                remainder_m = 0;
                continue;
            }
            bump_sum += node->sum_grades - node->left->sum_grades;
            remainder_m -= 1 + node->right->top_workers_count;
            node = node->left;
        }
    }

    *((long long int*)sumBumpGrades) = bump_sum;

    return SUCCESS;
}

 StatusType EmployeeManager::averageBumpGradeBetweenSalaryByGroup(int companyID, long long int lowerSalary, long long int higherSalary, void* averageBumpGrade){
    if (!averageBumpGrade || higherSalary < 0 || lowerSalary < 0 || lowerSalary > higherSalary ||
     companyID < 0 || companyID > this->num_companies){
         return INVALID_INPUT;
     }

    long long int context_non_salary_sum_grades = 0;
    int context_non_salary_count = 0;

     EarnerRankTree* tree;

     if (companyID == 0){
         tree = this->top_workers;

         context_non_salary_sum_grades = this->non_salary_sum_grades;
         context_non_salary_count = this->non_salary_count;
     }
     else{
         Company* company = this->companies->company(companyID);
         tree = company->top_workers;

         context_non_salary_sum_grades = company->non_salary_sum_grades;
         context_non_salary_count = company->non_salary_count;
     }

     EarnerTreeNode* node = tree->getRoot();

     EarnerTreeNode* max_node = findMaxBelowLimit(node, higherSalary);
     EarnerTreeNode* min_node = findMinAboveLimit(node, lowerSalary);

//     [0, x] &&
//     min node != null && max node == null &&
//     context_non_salary_count == 0


    long long int sum = 0;
    int range_count = 0;

    if (lowerSalary == 0){
        if (context_non_salary_count == 0){
            if (!min_node || !max_node) return FAILURE; // OK state
        }
        else{
            if (!max_node){ //Here we have context_non_salary_count != 0 and an empty tree (no need to check min for nullity)
                *((long double*)averageBumpGrade) = (long double)context_non_salary_sum_grades / context_non_salary_count;
                return SUCCESS;
            }

            sum += context_non_salary_sum_grades;
            range_count += context_non_salary_count;
        }
    }
    else{
        if (!min_node || !max_node) return FAILURE;
    }

     sum = node->sum_grades;
     range_count = node->top_workers_count;


//     if ((min_node == nullptr &&  context_non_salary_count == 0) || (min_node == nullptr && lowerSalary != 0)){
//         return FAILURE;
//     }
//     if (!max_node){
//         return FAILURE;
//     }


     // For max_node
     while(node){
        if (max_node->key < node->key){
             sum -= node->value->grade + ((node->right) ? node->right->sum_grades : 0);
             range_count -= 1 + ((node->right) ? node->right->top_workers_count : 0);
             node = node->left;
         }
        else if (node == max_node){
             sum -= (node->right) ? node->right->sum_grades : 0;
             range_count -= (node->right) ? node->right->top_workers_count : 0;
             break;
         }
        else{
             node = node->right;
        }
     }

     node = tree->getRoot();

     // For min_node
     while(node){
         if (node->key < min_node->key){
             sum -= node->value->grade + ((node->left) ? node->left->sum_grades : 0);
             range_count -= 1 + ((node->left) ? node->left->top_workers_count : 0);
             node = node->right;
         }
         else if (node == min_node){
             sum -= (node->left) ? node->left->sum_grades : 0;
             range_count -= (node->left) ? node->left->top_workers_count : 0;
             break;
         }
         else{
             node = node->left;
         }
     }

     *((long double*)averageBumpGrade) = (long double)sum / range_count;
     return SUCCESS;
 }

EarnerTreeNode* EmployeeManager::findMinAboveLimit(EarnerTreeNode* node, long long int minSalary){
    if (node == nullptr){
        return nullptr;
    }

    SalaryKey min_key = {minSalary, 0};

    // Searching for first appearance of node with key greater than min_key
    while(node && node->key < min_key){
        node = node->right;
    }

    // In this case, every node is smaller than min_key
    if (node == nullptr){
        return nullptr;
    }

    SalaryKey cmp_key = node->key - min_key; //assert salary >= 0 in this key
    SalaryKey diff_key;
    EarnerTreeNode* minNode = node;

    while (node){
        diff_key = node->key - min_key;
        if ( (SalaryKey){0,0} < diff_key && diff_key < cmp_key ){ // Assert 0 is not a valid ID, so that diff_key is never {0,0}
            minNode = node;
            cmp_key = diff_key;
        }

        if (min_key < node->key) node = node->left;
        else node = node->right;
    }

    return minNode;
}

EarnerTreeNode* EmployeeManager::findMaxBelowLimit(EarnerTreeNode* node, long long int maxSalary){
    if (node == nullptr){
        return nullptr;
    }

    SalaryKey max_key = {maxSalary, INT_MAX};

    // Searching for first appearance of node with key greater than max_key
    while(node && max_key < node->key ){
        node = node->left;
    }

    // In this case, every node is smaller than max_key
    if (node == nullptr){
        return nullptr;
    }

    SalaryKey cmp_key = max_key - node->key; //assert salary >= 0 in this key
    SalaryKey diff_key;
    EarnerTreeNode* maxNode = node;

    while (node){
        diff_key = max_key - node->key;
        if ( (SalaryKey){0,0} < diff_key && diff_key < cmp_key){ // Assert INT_MAX is not a valid ID, so that diff_key is never {0,0}. else, define '<='
            maxNode = node;
            cmp_key = diff_key;
        }

        if (max_key < node->key) node = node->left;
        else node = node->right;
    }

    return maxNode;
}

StatusType EmployeeManager::companyValue(int companyID, void* standing) {
    if (companyID <= 0 || companyID > this->num_companies) {
        return INVALID_INPUT;
    }

    *((long double*) standing) = this->companies->companyValue(companyID);

    return SUCCESS;
}