#ifndef UNIONFIND_H_
#define UNIONFIND_H_

#include "Company.h"
#include <memory>

#define NO_SIZE -1

struct CompanyNode{
    Company* company;
    CompanyNode* parent;
    CompanyNode* label;

    int size;
    double r; // Helper field, as seen in boxes problem


    CompanyNode() : company(nullptr), parent(nullptr), label(nullptr), size(NO_SIZE), r(0)  {}

    CompanyNode(Company* _company, CompanyNode* _parent = nullptr, int _size = 1) :
         company(_company), parent(_parent), label(nullptr), size(_size)  {
             r = _company->ID;
    }

    bool isRoot() {
        return !this->parent;
    }
};

class CompanyUnion{
    public:
        CompanyNode* companyNodes;
        int numberOfCompanies;

    public:
        CompanyUnion(int k) {
            numberOfCompanies = k;
            this->companyNodes = new CompanyNode[k];

            // Note: there will be a label (dummy) CompanyNode that stores owner of company tree
            for (int ID = 0; ID < k; ID++) {
                this->companyNodes[ID] = CompanyNode(new Company(ID + 1));
                this->companyNodes[ID].label = &this->companyNodes[ID]; // Creating the labels
            }
        }

        // Union function
        void acquisition(CompanyNode* acquirer, CompanyNode* target, double factor) {
            if (acquirer == target || !acquirer->isRoot() || !target->isRoot()) return;

            CompanyNode* acquirer_label = acquirer->label;
            CompanyNode*  target_label = target->label;

            double added_value = factor * this->companyValue(target_label->company->ID);

            int unified_group_size = acquirer_label->size + target_label->size;

            target->company->owner = acquirer_label->company;

            EmployeeHash* acquirer_hash = acquirer_label->company->employees;
            EmployeeHash* target_hash = target_label->company->employees;

            target_label->company->employees = nullptr;
            acquirer_label->company->employees = new EmployeeHash(acquirer_hash, target_hash);

            delete acquirer_hash;
            delete target_hash;

            CompanyNode* greater = (acquirer_label->size >= target_label->size) ? acquirer : target;
            CompanyNode* smaller = (greater == acquirer) ? target : acquirer;

            acquirer->r += added_value;

            if (acquirer == greater)
                target->r -= acquirer->r;
            else
                acquirer->r -= target->r;

            smaller->parent = greater;
            greater->parent = nullptr;

            acquirer_label->size = unified_group_size;
            target_label->size = NO_SIZE;

            smaller->label = nullptr;
            greater->label = acquirer_label;
        }

        CompanyNode* find(int CompanyID) {
            double r_sum = 0.0;

            if (CompanyID > numberOfCompanies || CompanyID <= 0) {
                return nullptr;
            }

            CompanyNode* node = &this->companyNodes[CompanyID - 1];
            while (node->parent) {
                r_sum += node->r;
                node = node->parent;
            }

            CompanyNode* root = node;

            // 'root' now stores the root of target company
            node = &this->companyNodes[CompanyID - 1];
            CompanyNode* temp_parent;

            while (node != root) {
				double node_r = node->r;
				node->r = r_sum;
				r_sum -= node_r;

                temp_parent = node->parent;
                node->parent = root;
                node = temp_parent;
            }

            return root;
        }

        Company* company(int ID) {
            CompanyNode* root = this->find(ID);

            if (!root) return nullptr;

            return root->label->company;
        }

        double companyValue(int ID) {

			CompanyNode* node = &companyNodes[ID - 1];
			double sum = node->r;

			while (node->parent) {
				node = node->parent;
				sum += node->r;
			}

			return sum;
        }

        ~CompanyUnion(){
            for(int i = 0; i < numberOfCompanies; i++) {
                delete companyNodes[i].company;
            }

            delete[] companyNodes;
        }
};

#endif
