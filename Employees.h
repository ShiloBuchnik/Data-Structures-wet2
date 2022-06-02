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

        bool addEmployeeNoResize(int ID, Employee* employee) {
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
        
        if (non_empty_count == max_size) this->max_size *= 2;

        // Note: this is our variation due to resizing algorithm being inaccurate
        else if (size <= 0.25 * max_size) this->max_size = std::ceil(max_size * 0.5);
        else return;
            
        EmployeeNode** old_arr = employeeNodes;
        employeeNodes = new EmployeeNode*[max_size];

        for (int i = 0; i < max_size; i++) employeeNodes[i] = new EmployeeNode(); // Creating a dummy for every element in new array

        for (int i = 0; i < old_max; i++){ // Deleting the elements in old array
            if (!(old_arr[i]->next)){
                delete old_arr[i];
                continue;
            }

            while(list_iter->next){
                list_iter = list_iter->next;
                delete list_iter->prev;

                addEmployeeNoResize(list_iter->employee->ID, list_iter->employee);
            }
            delete list_iter;
        }
        
        delete[] old_arr;
    }


    void addEmployee(int ID, Employee* employee) {
        if (this->addEmployeeNoResize(ID, employee)) {
            this->resize();
        }
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
        */

        this->non_empty_count = 0;
        this->max_size = std::max(x1->max_size, x2->max_size);
        

        // Initialize new employeeNodes array with dummy nodes
        for (int i = 0; i < this->max_size; i++)
            this->employeeNodes[i] = new EmployeeNode();


        EmployeeHash* hash[] = { x1, x2 };

        for (int i = 0; i < 2; i++) {
            // Iterate over indices of hash
            for (int j = 0; j < hash[i]->max_size; j++) {
                EmployeeNode* list = hash[i]->employeeNodes[j];
                
                // Get head of linked list (might be empty) and delete dummy node in either case
                Employee* list_node = list->next;
                delete list;

                // Continue in case of empty linked list head at current index
                if (!list_node) continue;

                // Otherwise, add employee to the new hash and delete any allocated memory
                while (list_node) {
                    this->addEmployeeNoResize(list_node->employee->ID, list_node->employee);

                    list_node = list_node->next;
                    delete list_node->prev;
                }
            }
        } 


        // Delete old employeeNode arrays from each hash table
        delete x1->employeeNodes;
        delete x2->employeeNodes;

        this->resize();
    }

};