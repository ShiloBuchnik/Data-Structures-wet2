#include <algorithm>

#include "Company.h"
#include "Employee.h"

// This is a hash table, i hate myself (the band)

// size indicates number of set indices

struct EmployeeNode{
    Employee* employee;
    EmployeeNode* prev;
    EmployeeNode* next;
    bool is_set; // Only matters for first node in each linked list

    EmployeeNode(Employee* _employee = nullptr, EmployeeNode* _prev = nullptr, EmployeeNode* _next = nullptr){
        employee = _employee;
        prev = _prev;
        is_set = false;
    }
};

class EmployeeHash {
    private:
        EmployeeNode** employeeNodes;
        int max_size; // Number of overall elements in array
        int non_empty_count;
        int employee_size;

        int hashFunction(int id){
            return id % max_size;
        }

    public:

    Employee* getEmployee(int id){
        EmployeeNode* node = employeeNodes[hashFunction(id)]->next;

        if (!node) return nullptr;

        while (node){
            if (node->employee->ID == node->employee->ID){
                return node->employee;
            }

            node = node->next;
        }

        return nullptr;
    }

    // Default c'tor, for creating object with 0 Employees
    EmployeeHash(){
        employeeNodes = new EmployeeNode*[1];
        non_empty_count = 0;
        employee_size = 0;
        max_size = 1;
        employeeNodes[0] = new EmployeeNode();
    }


    void resize(){
        int old_max = max_size;

        if (non_empty_count == max_size) this->max_size *= 2{
            EmployeeNode** tempArr = new EmployeeNode*[max_size];
            for (int i = 0; i < old_max; i++) tempArr[i] = EmployeeNodes[i];
            for (int i = old_max; i < max_size; i++) tempArr[i] = new EmployeeNode();
        }

        // Note: this is our variation due to resizing algorithm being inaccurate
        else if (size <= 0.25 * max_size) this->max_size = std::ceil(max_size * 0.5) {
            EmployeeNode** old_arr = employeeNodes;
            employeeNodes = new EmployeeNode*[max_size];

            for (int i = 0; i < max_size; i++) employeeNodes[i] = new EmployeeNode(); // Creating a dummy for every element

            for (int i = 0; i < max_size; i++){ // Deleting elements in old_arr
                if (!(old_arr[i]->next)){
                    delete old_arr[i];
                    continue;
                }

                EmployeeNode* list_iter = old_arr[i]->next;
                delete list_iter->prev; // Deleting old dummy
                while(list_iter){
                    addEmployee(list_iter->employee->id, list_iter->employee);
                    if (!(list_iter->next)){ // If next doesn't exist, we stop
                        delete list_iter;
                        list_iter = nullptr;
                    }
                    else{
                        list_iter = list_iter->next;
                        delete list_iter->prev;
                    } 
                }

                delete[] old_arr;
            }
        }

        else return;
    }

    // Remember to set is_set flag
    void addEmployee(int ID, Employee* employee) {
        if (this->getEmployee(ID)) {
            return;
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

        this->resize();
    }
    
    /**
    * C'tor for merging two employee hash tables
    * Assume employeeNode properties of x1,x2 are deleted during the course of this function
    */
    EmployeeHash(EmployeeHash* x1, EmployeeHash* x2){
        
        /*
        int minimal_size = x1->size + x2->size;
        int maximal_size = std::max(x1->max_size, x2->max_size);

        if (minimal_size == maximal_size) {
            this->max_size = 2*minimal_size;
        }

        else if (minimal_size == 0.25 * maximal_size) {
            this->max_size = 0.5 * maximal_size;
        }

        else this->max_size = maximal_size;


        this->size = minimal_size;
        this->employeeNodes = new EmployeeNode*[this->max_size];
        */

        this->non_empty_count = 0;
        int maximal_size = std::max(x1->max_size, x2->max_size);
        
        this->employee_size = x1->employee_size + x2->employee_size;
        this->max_size = maximal_size;

        this->employeeNodes = new EmployeeNode*[this->max_size];

        // Initialize entries of new employeeNodes property
        // Running on the new array, and concatenating the respective linked lists

        int minimal_max_size = std::min(x1->max_size, x2->max_size);

        for (int i = 0; i < minimal_max_size; i++) {
            this->employeeNodes[i] = x1->employeeNodes[i];

            if (x2->employeeNodes[i]->next) {
                EmployeeNode* x1_last = x1->employeeNodes[i]->next;

                // Get to last element of x2 linked list
                while (x1_last->next) x1_last = x1_last->next;

                x1_last->next = x2->employeeNodes[i]->next;
                x2->employeeNodes[i]->next->prev = x1_last;

                // Delete old x2 dummy as it's no longer needed
                delete x2->employeeNodes[i];
            }

            if (this->employeeNodes[i]->next) ++this->non_empty_count;
        }

        for (int i = minimal_max_size; i < x1->max_size; i++){
            this->employeeNodes[i] = x1->employeeNodes[i];
            if (this->employeeNodes[i]->next) ++this->non_empty_count;
        }


        for (int i = minimal_max_size; i < x2->max_size; i++){
            this->employeeNodes[i] = x2->employeeNodes[i];
            if (this->employeeNodes[i]->next) ++this->non_empty_count;
        }

        // Delete old employeeNode arrays from each hash table
        delete x1->employeeNodes;
        delete x2->employeeNodes;

        this->resize();
    }
        
};