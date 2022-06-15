#ifndef EMPLOYEES_H_
#define EMPLOYEES_H_

#include <cmath>
#include <algorithm>
#include <iostream>

// size indicates number of set indices
class Employee;

struct EmployeeNode {
    Employee* employee;
    EmployeeNode* prev;
    EmployeeNode* next;
    bool is_set; // Only matters for first node in each linked list

    EmployeeNode(Employee* _employee = nullptr, EmployeeNode* _prev = nullptr, EmployeeNode* _next = nullptr);
};

class EmployeeHash {
    public:
        EmployeeNode** employeeNodes;
        int max_size; // Number of overall elements in array
        int non_empty_count;
        int employee_size;
        bool delete_employees;

        int hashFunction(int id){
            return id % max_size;
        }

        bool addEmployeeNoResize(int ID, Employee* employee);

        EmployeeNode* getEmployeeNode(int ID);

    // Default c'tor, for creating object with 0 Employees
    EmployeeHash();

    Employee* getEmployee(int ID);

    void resize();

    void addEmployee(int ID, Employee* employee);
    
    /**
    * C'tor for merging two employee hash tables
    * Assume employeeNode properties of x1,x2 are deleted during the course of this function
    */
    EmployeeHash(EmployeeHash* x1, EmployeeHash* x2);

    void deleteEmployee(int ID);

    ~EmployeeHash();
};

#endif