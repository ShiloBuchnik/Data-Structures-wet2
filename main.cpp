#include <iostream>
#include "Employees.h"
#include "Employee.h"

int main() {
    Employee* arr[1000];

    EmployeeHash hash;
    
    for (int i = 999; i >= 0; i--) {
        arr[i] = new Employee(i, nullptr, 0, 0);
    }

    for (int i = 999; i >= 0; i--) {
        hash.addEmployee(arr[i]->ID, arr[i]);
    }

     for (int i = 0; i < hash.max_size; i++){
        std::cout << "Entering linked list of index " << i << std::endl;
        if (!(hash.employeeNodes[i]->next)){
            continue;
        }

        EmployeeNode* list_iter = hash.employeeNodes[i];
        while(list_iter->next){
            list_iter = list_iter->next;
            std::cout << list_iter->employee->ID << std::endl;
        }
    }

    return 0;
}
