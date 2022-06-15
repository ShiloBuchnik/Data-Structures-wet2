#include "EmployeeHash.h"
#include "Employee.h"

EmployeeNode::EmployeeNode(Employee* _employee, EmployeeNode* _prev, EmployeeNode* _next) {
    employee = _employee;
    next = _next;
    prev = _prev;
    is_set = false;
}

// Default c'tor, for creating object with 0 Employees
EmployeeHash::EmployeeHash(){
    employeeNodes = new EmployeeNode*[1];
    non_empty_count = 0;
    employee_size = 0;
    max_size = 1;
    employeeNodes[0] = new EmployeeNode(nullptr);
    delete_employees = false;
}

 bool EmployeeHash::addEmployeeNoResize(int ID, Employee* employee) {
     if (this->getEmployee(ID)) {
        return false;
    }

    int target_index = this->hashFunction(ID);
    EmployeeNode* dummy = this->employeeNodes[target_index];

    // Define target_index as set and increment non-empty count property
    if (!dummy->is_set) {
        ++this->non_empty_count;
        dummy->is_set = true;
    }

    /*
    * Create new employee node
    * Note: old_head night be a nullptr, this is fine
    */
    EmployeeNode* old_head = dummy->next;
    EmployeeNode* new_head = new EmployeeNode(employee, dummy, old_head);

    // Add new node to appropriate linked list, change links between nodes appropriately
    dummy->next = new_head;

    if (old_head) {
        old_head->prev = new_head;
    }

    return true;
}

EmployeeNode* EmployeeHash::getEmployeeNode(int ID) {
    EmployeeNode* node = employeeNodes[hashFunction(ID)]->next;

    if (!node) return nullptr;

    while (node){
        if (node->employee->ID == ID){
            return node;
        }

        node = node->next;
    }

    return nullptr;
}

Employee* EmployeeHash::getEmployee(int ID){
    EmployeeNode* node = this->getEmployeeNode(ID);
    if (!node) return nullptr;

    return node->employee;
}

void EmployeeHash::resize(){
    int old_max = max_size;
    
    if (non_empty_count == max_size) this->max_size *= 2;

    // Note: this is our variation due to resizing algorithm being inaccurate
    else if (non_empty_count == floor(0.25 * max_size)) this->max_size = std::ceil(max_size * 0.5);
    else return;

    non_empty_count = 0;
    EmployeeNode** old_arr = employeeNodes;
    employeeNodes = new EmployeeNode*[max_size];

    for (int i = 0; i < max_size; i++) employeeNodes[i] = new EmployeeNode(nullptr); // Creating a dummy for every element in new array

    EmployeeNode* list_iter = nullptr;
    for (int i = 0; i < old_max; i++){ // Deleting the elements in old array
        if (!(old_arr[i]->next)){
            delete old_arr[i];
            continue;
        }

        list_iter = old_arr[i];
        while(list_iter->next){
            list_iter = list_iter->next;
            delete list_iter->prev;
            list_iter->prev = nullptr;

            addEmployeeNoResize(list_iter->employee->ID, list_iter->employee);
        }
        delete list_iter;
    }
    
    delete[] old_arr;
}

 void EmployeeHash::addEmployee(int ID, Employee* employee) {
    this->resize();
    if (this->addEmployeeNoResize(ID, employee)) ++this->employee_size;
}

EmployeeHash::EmployeeHash(EmployeeHash* x1, EmployeeHash* x2) {
    this->non_empty_count = 0;
    this->employee_size = 0;
    this->max_size = std::max(x1->max_size, x2->max_size);
    this->delete_employees = false;

    this->employeeNodes = new EmployeeNode*[this->max_size];

    // Initialize new employeeNodes array with dummy nodes
    for (int i = 0; i < this->max_size; i++)
        this->employeeNodes[i] = new EmployeeNode(nullptr);

    EmployeeHash* hash[] = { x1, x2 };

    for (int i = 0; i < 2; i++) {
        // Iterate over indices of hash
        for (int j = 0; j < hash[i]->max_size; j++) {
            EmployeeNode* list = hash[i]->employeeNodes[j];

            // Get head of linked list (might be empty) and delete dummy node in either case
            EmployeeNode* list_node = list->next;

            // Continue in case of empty linked list head at current index
            if (!list_node) continue;

            // Otherwise, add employee to the new hash and delete any allocated memory
            while (list_node) {
                this->addEmployeeNoResize(list_node->employee->ID, list_node->employee);
                ++this->employee_size;

                list_node = list_node->next;
            }

        }
    }

    this->resize();
}

void EmployeeHash::deleteEmployee(int ID) {
    EmployeeNode* target_node = this->getEmployeeNode(ID);

    // A target node not being found means this employee does not exist within hash table
    if (!target_node) return;

    // Otherwise, remove the employee node from its respective linked list and alter the table's properties accordingly
    EmployeeNode* previous_node = target_node->prev;
    EmployeeNode* next_node = target_node->next;

    // Change previous node's successive node according to whether target node had one or not
    previous_node->next = next_node;

    if (next_node) {
        next_node->prev = previous_node;
    }

    // Set linked list to 'unset' if it's empty after deletion
    EmployeeNode* target_list = this->employeeNodes[this->hashFunction(ID)];
    if (!target_list->next) {
        target_list->is_set = false;
        --this->non_empty_count;
    }

    // Finally, delete target node
    --this->employee_size;
    delete target_node;

    this->resize();
}

EmployeeHash::~EmployeeHash(){
    for (int i = 0; i < max_size; i++){
        if (!(employeeNodes[i]->next)){
            delete employeeNodes[i];
            continue;
        }

        EmployeeNode* list_iter = employeeNodes[i];
        while(list_iter->next){
            if (this->delete_employees){
                delete list_iter->employee;
            }
            list_iter = list_iter->next;
            delete list_iter->prev;
        }

        if (this->delete_employees){
            delete list_iter->employee;
        }
        delete list_iter;
    }

    delete[] this->employeeNodes;
}