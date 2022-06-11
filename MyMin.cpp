inline SalaryKey operator - (const SalaryKey &a, const SalaryKey &b)
{
    SalaryKey difference = {a.salary - b.salary, a.EmployeeID - b.EmployeeID};

    return difference;
}

EarnerTreeNode* EmployeeManager::findMinAboveLimit(EarnerTreeNode* node, int minSalary){
    if (node == NULL){
        return NULL;
    }

    SalaryKey min_key = {minSalary, 0};

    // Searching for first appearance of node with key greater than min_key
    while(node && node->key < min_key){
        node = node->right;
    }

    // In this case, every node is smaller than min_key
    if (node == NULL){
        return NULL;
    }

    SalaryKey cmp_key = node->key - min_key; //assert salary >= 0 in this key
    SalaryKey diff_key;
    EarnerTreeNode* minNode = node;

    while (node){
        diff_key = node->key - min_key;
        if ( (SalaryKey){0,0} < diff_key && diff_key < cmp_key){
            minNode = node;
            cmp_key = node->key;
        }

        if (min_key < node->key) node = node->left;
        else node = node->right;
    }

    return minNode;
}
