#ifndef UNIONFIND_H_
#define UNIONFIND_H_

#include "Company.h"

#define NO_SIZE -1

struct CompanyNode{
    Company* company;
    CompanyNode* parent;
    int size;

    CompanyNode(Company* _company, CompanyNode* _parent = nullptr, int _size = 1) :
         company(_company), parent(_parent), size(_size) {}

    bool isRoot() { return this->parent->parent == nullptr; }
};

class CompanyUnion{
    public:
        CompanyNode** companyNodes;
        int numberOfCompanies;

    public:
        CompanyUnion(int k) {
            numberOfCompanies = k;
            this->companyNodes = new CompanyNode*[k];

            // Note: there will be a label (dummy) CompanyNode that stores owner of company tree
            for (int ID = 0; ID < k; ID++) {
                this->companyNodes[ID] = new CompanyNode(new Company(ID + 1));
                this->companyNodes[ID]->parent = new CompanyNode(this->companyNodes[ID]->company); // Creating the labels
            }
        }

        // Union function
        void acquisition(CompanyNode* acquirer, CompanyNode* target, int factor) {
            if (acquirer == nullptr || target == nullptr) throw "i hate myself (the band)";

            if (acquirer == target || !acquirer->isRoot() || !target->isRoot()) return;
            
            CompanyNode* acquirer_label = acquirer->parent;
            CompanyNode* target_label = target->parent;

            int unified_group_size = acquirer_label->size + target_label->size;

            target->company->owner = acquirer_label->company;

            EmployeeHash* acquirer_hash = acquirer_label->company->employees;
            EmployeeHash* target_hash = target_label->company->employees;

            target_label->company->employees = nullptr;
            acquirer_label->company->employees = new EmployeeHash(acquirer_hash, target_hash);

            delete acquirer_hash;
            delete target_hash;

            CompanyNode* greater = (acquirer_label->size < target_label->size) ? acquirer : target;
            CompanyNode* smaller = (greater == acquirer) ? target : acquirer;

            // The target label is no longer needed from this point on
            delete target_label;

            smaller->parent = greater;
            acquirer_label->size = unified_group_size;

            greater->parent = acquirer_label;
        }

        CompanyNode* find(int CompanyID) {
            if (CompanyID > numberOfCompanies || CompanyID <= 0) {
                return nullptr;
            }

            CompanyNode* root = this->companyNodes[CompanyID - 1];
            while (root->parent->parent) {
                root = root->parent;
            }

            // $root now stores the root of target company
            CompanyNode* node = this->companyNodes[CompanyID - 1];
            CompanyNode* temp_parent;
            while (node != root) {
                temp_parent = node->parent;
                node->parent = root;
                node = temp_parent;
            }

            return root;
        }

        Company* getCompany(int ID) {
            CompanyNode* root = this->find(ID);

            if (!root) return nullptr;

            return root->parent->company;
        }

        ~CompanyUnion(){
            for(int i = 0; i < numberOfCompanies; i++) {
                if (this->companyNodes[i]->isRoot()){
                    delete companyNodes[i]->parent;
                }
                delete this->companyNodes[i]->company;
                delete this->companyNodes[i];
            }

            delete this->companyNodes;
        }
};


#endif