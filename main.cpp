#include <iostream>
#include "Employees.h"
#include "Employee.h"
#include "UnionFind.h"
#include "EarnerRankTree.h"
#include <random>

void testHash() {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(40,300000000);

	int magic = 3000;
    int indices_to_delete = 1123;

    std::uniform_int_distribution<std::mt19937::result_type> dist_delete(0, magic - 1); // distribution in range of hash indices

    EmployeeHash hash;
    
    Employee* arr[magic];
    int values[magic];
    int delete_indices[indices_to_delete];

    for (int i = 0; i < magic; i++) {
        do {
            values[i] = dist(rng);
        } while (hash.getEmployee(values[i]));

        arr[i] = new Employee(values[i], nullptr, 0, 0);
        hash.addEmployee(arr[i]->ID, arr[i]);
    }


    for (int i = 0; i < indices_to_delete; i++) {
        int index;
        bool in_array = true;
        do {
            int found = false;
            index = dist_delete(rng);
            for (int j = 0; j < i; j++) {
                if (delete_indices[j] == values[index]) {
                    found = true;
                    break;
                }
            }
            in_array = found;
        } while (in_array);
        delete_indices[i] = values[index];
    }

    int real_employee_size = 0;
    // std::cout << "@@@ Array before deletion of" << indices_to_delete << "random elements: @@@" << std::endl;
     for (int i = 0; i < hash.max_size; i++){
        // std::cout << "Entering linked list of index " << i << std::endl;
        if (!(hash.employeeNodes[i]->next)){
            continue;
        }

        EmployeeNode* list_iter = hash.employeeNodes[i];
        while(list_iter->next){
            list_iter = list_iter->next;
            ++real_employee_size;
            // std::cout << list_iter->employee->ID << std::endl;
        }

        // std::cout << std::endl;
    }


    std::cout << std::endl;

    int count_before = hash.employee_size;

    Employee* refetch[indices_to_delete];

    std::cout << "@@@ Indices to delete: @@@" << std::endl;
    int line_mod = 0;
    int line_length = 20;
    for (int i = 0; i < indices_to_delete; i++) {
        std::cout << (((line_mod != line_length && line_mod != 0) ? ", " : "")) << delete_indices[i];
        if (line_mod == line_length - 1) std::cout << std::endl;
        refetch[i] = hash.getEmployee(delete_indices[i]);
        hash.deleteEmployee(delete_indices[i]);
        line_mod = (line_mod + 1) % line_length;

    }

    int count_after = hash.employee_size;

    std::cout << std::endl;

    std::cout << "@@@ Sanity Check @@@" << std::endl;

    bool oddities = false;
    for (int i = 0; i < magic; i++) {
        int found = false;
        for (int j = 0; j < indices_to_delete; j++) {
            if (delete_indices[j] == values[i]) {
                found = true;
                break;
            }
        }

        if ((found && hash.getEmployee(values[i])) || (!found && !hash.getEmployee(values[i]))) {
            std::cout << ((found) ? "(X) " : "(-) ") << values[i] << " :: " << ((!hash.getEmployee(values[i])) ? "XXXXX" : "-----") << std::endl;
            oddities = true;
        }
    }

    if (!oddities) {
        std::cout << "All elements were deleted/persisted according to logic." << std::endl;
    }

    std::cout << std::endl;


    // std::cout << "@@@ Indices after deletion: @@@" << std::endl;
    //  for (int i = 0; i < hash.max_size; i++){
    //     std::cout << "Entering linked list of index " << i << std::endl;
    //     if (!(hash.employeeNodes[i]->next)){
    //         continue;
    //     }

    //     EmployeeNode* list_iter = hash.employeeNodes[i];
    //     while(list_iter->next){
    //         list_iter = list_iter->next;
    //         std::cout << list_iter->employee->ID << std::endl;
    //     }
    //     std::cout << std::endl;
    // }

    int set_issue = false;
    int real_non_empty_count = 0;
    for (int i = 0; i < hash.max_size; i++) {
        EmployeeNode* list = hash.employeeNodes[i];
        if ((!list->next && list->is_set) || (list->next && !list->is_set)) {
            set_issue = true;
            std::cout << "Wrong 'is_set' flag on empty linked list with ID [" << i << "]" << std::endl;
        }

        else if (list->next) ++real_non_empty_count;
    }

    if (!set_issue) {
        std::cout << "All 'is_set' flags are correct." << std::endl;
    }

    std::cout << "Hash non_empty_count: (" << hash.non_empty_count << ") -- Real non_empty_count: (" << real_non_empty_count << ")" << std::endl;

    std::cout << "Hash employee_size: " << count_before << " -- Real employee_size: " << real_employee_size << std::endl;
    std::cout << "Size before: (" << count_before << ") -- Deleted: (" << (count_before - count_after) << ") -- Size after: (" << count_after << ")" << std::endl;

    std::cout << std::endl;

    for (int i = 0; i < indices_to_delete; i++) {
        hash.addEmployee(refetch[i]->ID, refetch[i]);
    }

    std::cout << "Size after reading: " << hash.employee_size << std::endl;
}

void testUnion() {
    int k = 50;
    CompanyUnion uf(k);

    for(int i = 0; i <= 9; i++){
        for (int j = 2; j <= 5; j++){
            uf.acquisition(uf.find(5*i + 1), uf.find(5*i+j), 3);
        }
    }

    uf.acquisition(uf.find(1), uf.find(6), 3);

//    uf.acquisition(uf.find(23), uf.find(23), 3);
//    uf.acquisition(uf.find(7), uf.find(12), 3);
//    uf.acquisition(uf.find(2), uf.find(38), 3);
//    uf.acquisition(uf.find(6), uf.find(1), 3);
//    uf.acquisition(uf.find(41), uf.find(46), 3);
//    uf.acquisition(uf.find(21), uf.find(16), 3);
//    uf.acquisition(uf.find(21), uf.find(41), 3);
//    uf.acquisition(uf.find(21), uf.find(31), 3);
//    uf.acquisition(uf.find(26), uf.find(21), 3);
//    uf.acquisition(uf.find(26), uf.find(6), 3);


    for (int i = 1; i <= 50; i++) {
        std::cout << "Company no. " << i << " is owned by company no. " << uf.find(i)->parent->company->id << " with size " << uf.find(i)->parent->size
        << " with value: " << uf.companyValue(i) << std::endl;
    }

}

void testTree(){
    EarnerRankTree* tree1 = new EarnerRankTree();
    EarnerRankTree* tree2 = new EarnerRankTree();

    Company* company = new Company(3);

    int magic = 40;

    Employee* arr[3*magic];
    for (int i = 0; i < 2*magic; i++) {
        arr[i] = new Employee(i + 1, company, i + 1, i + 1);
    }

    for (int i = 0; i < magic; i++) {
        tree1->insert({arr[i]->ID, arr[i]->salary}, arr[i]);
    }
    for (int i = magic; i < 2*magic; i++) {
        tree2->insert({arr[i]->ID, arr[i]->salary}, arr[i]);
    }

    tree1->print();
    std::cout << std::endl << std::endl;
    tree2->print();

    std::cout << std::endl << std::endl;
//
//
//    EarnerRankTree* tree3 =  MergeEarnerRankTrees(tree1, tree2);
//
//    tree3->print();

//     for (int i = 0; i < magic; i+= 2) {
//         tree1->remove({ i+1, i+1 });
//    }
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0,magic*2);

    int values[magic];

    for (int i = 0; i < magic; i++) {
        int index;
        bool in_array = true;
        do {
            int found = false;
            index = dist(rng);
            for (int j = 0; j < i; j++) {
                if (values[j] == index + 1) {
                    found = true;
                    break;
                }
            }
            in_array = found;
        } while (in_array);
        values[i] = index + 1;
    }

    for (int i = 0; i < magic; i++){
        tree1-> remove({values[i], values[i]});
    }

     std::cout << std::endl << std::endl;


     tree1->print();
}

int main() {
    testTree();
    return 0;
}