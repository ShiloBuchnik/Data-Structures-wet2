#ifndef UNIONFIND_H_
#define UNIONFIND_H_

#include "Company.h"

#define NO_SIZE -1

struct CompanyNode{
    Company* company;
    CompanyNode* parent;
    int size;
    double r; // Helper field, as seen in boxes problem


    CompanyNode(Company* _company, CompanyNode* _parent = nullptr, int _size = 1) :
         company(_company), parent(_parent), size(_size) {
             r = _company->value;
         }

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
        void acquisition(CompanyNode* acquirer, CompanyNode* target, double factor) {
            if (acquirer == nullptr || target == nullptr) throw "i hate myself (the band)";
            if (acquirer == target || !acquirer->isRoot() || !target->isRoot()) return;
            
            CompanyNode* acquirer_label = acquirer->parent;
            CompanyNode* target_label = target->parent;

            double added_value = factor * target_label->company->value;

            int unified_group_size = acquirer_label->size + target_label->size;

            target->company->owner = acquirer_label->company;

            EmployeeHash* acquirer_hash = acquirer_label->company->employees;
            EmployeeHash* target_hash = target_label->company->employees;

            target_label->company->employees = nullptr;
            acquirer_label->company->employees = new EmployeeHash(acquirer_hash, target_hash);

            delete acquirer_hash;
            delete target_hash;

            CompanyNode* greater = (acquirer_label->size >= target_label->size) ? acquirer : target; // Potential bug
            CompanyNode* smaller = (greater == acquirer) ? target : acquirer;

            acquirer->r += added_value;

            if (target == greater)
                acquirer->r -= target->r;
            else
                target->r -= acquirer->r;

            // The target label is no longer needed from this point on
            delete target_label;

            smaller->parent = greater;
            acquirer_label->size = unified_group_size;

            greater->parent = acquirer_label;
        }

        CompanyNode* find(int CompanyID) {
            double r_sum = 0.0;

            if (CompanyID > numberOfCompanies || CompanyID <= 0) {
                return nullptr;
            }

            CompanyNode* node = this->companyNodes[CompanyID - 1];
            while (node->parent->parent) {
                r_sum += node->r; // Moral compass
                node = node->parent;
            }

            CompanyNode* root = node;

            double r_subtract = 0;
            // $root now stores the root of target company
            node = this->companyNodes[CompanyID - 1];
            CompanyNode* temp_parent;
            while (node != root) {
                r_subtract += node->r;
                node->r = (r_sum - r_subtract) + node->r;
                node->company->value = node->r + root->r;

                temp_parent = node->parent;
                node->parent = root;
                node = temp_parent;
            }

            r_subtract += node->r;
            node->company->value = (r_sum - r_subtract) + 2 * node->r;

            return root;
        }

        Company* company(int ID) {
            CompanyNode* root = this->find(ID);

            if (!root) return nullptr;

            return root->parent->company;
        }

        double companyValue(int ID) {
            this->find(ID);
            return companyNodes[ID - 1]->company->value;
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