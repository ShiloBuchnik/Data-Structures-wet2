#include "Company.h"

// This is a hash table, i hate myself (the band)

struct EmployeeNode{
    Employee* employee;
    EmployeeNode* prev;
    EmployeeNode* next;

    EmployeeNode(Employee* _employee = nullptr, EmployeeNode* _prev = nullptr, EmployeeNode* _next = nullptr){
        employee = _employee;
        prev = _prev;
        next = _next;
    }
}


class Employees{
    private:
        EmployeeNode** employeeNodes;
        int size;

        int hashFunction(int id){
            return id % size;
        }

    public:

    Employee* getEmployee(int id){
        EmployeeNode* node = employeeNodes[hashFunction(id)]->next;

        if (!node) return nullptr;

        while (node){
            if (node->employee->id == node->employee->id){
                return node->employee;
            }

            node = node->next;
        }

        return nullptr;
    }

    // Default c'tor, for creating object with 0 Employees
    Employees(){
        employeeNodes = new EmployeeNode*[1];
        size = 1;
        employeeNodes[0] = new EmployeeNode();
    }

    // C'tor for merging two employee hash tables
    Employees(Employees* x1, Employees x2){

    }








};