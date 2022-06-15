#include "EarnerRankTree.h"

EarnerTreeNode::EarnerTreeNode(const KeyType &key, const DataType &value, EarnerTreeNode *parent, EarnerTreeNode *left, EarnerTreeNode *right) :
        key(key), value(value), parent(parent), left(left), right(right), height(0), top_workers_count(1), sum_grades(value->grade) {}

void EarnerTreeNode::postOrderDeleteChildNodes()
{
    if (this->left)
    {
        this->left->postOrderDeleteChildNodes();
    }
    if (this->right)
    {
        this->right->postOrderDeleteChildNodes();
    }

    delete this;
}

EarnerTreeNode *EarnerTreeNode::findWithinSubtree(const KeyType &key)
{
    // If key is smaller, go left
    if (key < this->getKey())
    {
        if (!this->left)
        {
            return nullptr;
        }
        return this->left->findWithinSubtree(key);
    }
    // If key is bigger, go right
    else if (this->getKey() < key)
    {
        if (!this->right)
        {
            return nullptr;
        }
        return this->right->findWithinSubtree(key);
    }
    // If key is equal, we found the node we were looking for
    else
    {
        return this;
    }
}

EarnerTreeNode *EarnerTreeNode::getTreeMinimum()
{
    // Test case where such minimum does not exist
    if (this->right == nullptr)
    {
        return this;
    }

    EarnerTreeNode *current = this->right;
    while (current->left != nullptr)
    {
        current = current->left;
    }
    return current;
}

EarnerTreeNode *EarnerTreeNode::getSuccessor()
{
    if (this->right != nullptr)
    {
        return this->getTreeMinimum();
    }

    // Otherwise, look for a successor via parents

    EarnerTreeNode *current = this;
    EarnerTreeNode *parent = this->parent;

    while (parent != nullptr && current == parent->right)
    {
        current = parent;
        parent = current->parent;
    }

    return parent;
}

void EarnerTreeNode::print(const std::string &prefix)
{
    try
    {
        std::cout << prefix << "(salary: " << this->key.salary << " ID: " << this->key.EmployeeID << ") :: ";
        std::cout << (this->isRoot() ? "ROOT" : (this->isLeft() ? "LEFT" : "RIGHT"));
        std::cout << " Rank: #top_workers:# " << this->top_workers_count << " @@ #sum_grades:# " << this->sum_grades;
        std::cout << " Company ID: " << value->company->ID;
        // std::cout << ",,, PARENT: " << (this->parent ? *this->parent->key : -1);
        std::cout << std::endl;

        std::string new_prefix = prefix + "____";
        if (this->left)
        {
            this->left->print(new_prefix);
        }
        if (this->right)
        {
            this->right->print(new_prefix);
        }
    }
    catch (...)
    {
    }
}

// Returns a pointer to EarnerTreeNode of employee, else, to NULL
EarnerTreeNode *EarnerRankTree::find(const KeyType &key)
{
    // Stops if the tree is empty beforehand, or if we reached a 'nullptr' leaf
    if (this->isEmpty())
    {
        return nullptr;
    }

    return this->root->findWithinSubtree(key);
}

// We don't want to access NULL nodes
int nodeTopWorkers(EarnerTreeNode* node){
    return (!node) ? 0 : node->top_workers_count;
}

long long int nodeSumGrades(EarnerTreeNode* node){
    return (!node) ? 0 : node->sum_grades;
}

EarnerTreeNode *EarnerRankTree::LLRotate(EarnerTreeNode *node)
{
    EarnerTreeNode *target_root = node->left;
    EarnerTreeNode *current_root = node;

    // Handle rank logic
    EarnerTreeNode *B = current_root;
    EarnerTreeNode *A = current_root->left;

    B->top_workers_count = nodeTopWorkers(A->right) + nodeTopWorkers(B->right) + 1;
    A->top_workers_count = nodeTopWorkers(A->left) + nodeTopWorkers(B) + 1;

    B->sum_grades = nodeSumGrades(A->right) + nodeSumGrades(B->right) + B->value->grade;
    A->sum_grades = nodeSumGrades(A->left) + nodeSumGrades(B) + A->value->grade;

    // Handle rotation logic
    target_root->parent = current_root->parent;
    current_root->parent = target_root;

    current_root->left = target_root->right;

    if (current_root->left)
    {
        current_root->left->parent = current_root;
    }

    target_root->right = current_root;

    // Update heights of flipped root nodes
    current_root->updateHeight();
    target_root->updateHeight();

    return target_root;
}


EarnerTreeNode *EarnerRankTree::RRRotate(EarnerTreeNode *node)
{
    EarnerTreeNode *target_root = node->right;
    EarnerTreeNode *current_root = node;

    // Handle rank logic
    EarnerTreeNode *A = current_root;
    EarnerTreeNode *B = current_root->right;

    A->top_workers_count = nodeTopWorkers(A->left) + nodeTopWorkers(B->left) + 1;
    B->top_workers_count = nodeTopWorkers(A) + nodeTopWorkers(B->right) + 1;

    A->sum_grades = nodeSumGrades(A->left) + nodeSumGrades(B->left) + A->value->grade;
    B->sum_grades = nodeSumGrades(A) + nodeSumGrades(B->right) + B->value->grade;

    target_root->parent = current_root->parent;
    current_root->parent = target_root;

    current_root->right = target_root->left;

    if (current_root->right)
    {
        current_root->right->parent = current_root;
    }

    target_root->left = current_root;

    // Update heights of flipped root nodes
    current_root->updateHeight();
    target_root->updateHeight();

    return target_root;
}

EarnerTreeNode *EarnerRankTree::balance(EarnerTreeNode *node)
{
    // Do nothing in case of a balanced node
    if (node->isBalanced())
    {
        return node;
    }

    EarnerTreeNode **ptr;

    bool is_root = node->isRoot();
    if (is_root)
    {
        ptr = &(this->root);
    }
    else
    {
        ptr = (node->isLeft() ? &(node->parent->left) : &(node->parent->right));
    }

    switch (node->balanceFactor())
    {
    case 2:
        // LR Rotation
        if (node->left->balanceFactor() == -1)
        {
            node->left = this->RRRotate(node->left);
            *ptr = this->LLRotate(node);

            // LL Rotation
        }
        else if (node->left->balanceFactor() >= 0)
        {
            *ptr = this->LLRotate(node);
        }
        break;

    case -2:
        // RR Rotation
        if (node->right->balanceFactor() <= 0)
        {
            *ptr = this->RRRotate(node);

            // RL Rotation
        }
        else if (node->right->balanceFactor() == 1)
        {
            node->right = this->LLRotate(node->right);
            *ptr = this->RRRotate(node);
        }
        break;
    default:
        throw "Non-AVL tree behavior occurred while attempting rotation during insertion to AVL tree.";
    }

    return *ptr;
}

// Allocates a EarnerTreeNode, and insert it

EarnerTreeNode *EarnerRankTree::insert(const KeyType &key, const DataType &employee)
{
    // track insertion route
    // go back to root until parent is null
    // trigger imbalance logic if balance factor of a parent is invalid

    EarnerTreeNode *previous = this->root;
    EarnerTreeNode **current = &previous;

    if (this->isEmpty())
    {
        current = &this->root;
    }
    else
    {
        while (*current)
        {
            ++(*current)->top_workers_count;
            (*current)->sum_grades += employee->grade;
            previous = *current;

            if (key < previous->key)
            {
                current = &previous->left;
            }
            else if (previous->key < key)
            {
                current = &previous->right;
            }
            else
            {
                throw KeyExists(std::string("Identical values cannot exist in a binary search tree of type: ") + typeid(DataType).name());
            }
        }
    }

    // perform insertion with @previous as parent, testing occurs down below

    EarnerTreeNode *inserted_node = new EarnerTreeNode(key, employee, previous);

    *current = inserted_node;

    // Increment number of nodes in AVL Tree
    ++this->nodes;

    // 'previous' now holds the designated insertion spot parent
    EarnerTreeNode *imbalance = previous;

    // search for minimal imbalanced node, update all balanced nodes below it

    while (imbalance && imbalance->isBalanced())
    {
        imbalance->updateHeight();
        imbalance = imbalance->parent;
    }

    // no imbalance node has been found
    if (!imbalance)
    {
        return inserted_node;
    }

    // imbalance occurred, pinpoint type of rotation needed

    imbalance = this->balance(imbalance);

    // continue updating heights, no further imbalances will occur as shown during class.
    while (imbalance)
    {
        imbalance->updateHeight();
        imbalance = imbalance->parent;
    }

    return inserted_node;
}

// Deallocates an EarnerTreeNode

void EarnerRankTree::remove(const KeyType &key)
{
    if (this->isEmpty())
    {
        return;
    }

    EarnerTreeNode* removed_employee_node = this->find(key);
    if (!removed_employee_node)
        return;

    bool successor_existed = (!removed_employee_node->isLeaf() && !removed_employee_node->isSingleParent());

    Employee* employee = removed_employee_node->value;
    int diff = 0;

    EarnerTreeNode *v = this->removeAux(key, this->root, &diff);

    EarnerTreeNode *successor = removed_employee_node;
    bool successor_path = successor_existed && (successor != nullptr);

    // No such key exists
    if (!v)
    {
        // Decrement number of nodes in AVL Tree
        --this->nodes;
        return;
    }

    while (v)
    {
        // Follow path from deleted target upto root
        v->updateHeight();

        // Update rank of node
        if (v == successor) {
            successor_path = false;
        }

        --v->top_workers_count;
        v->sum_grades -= (successor_path) ? successor->value->grade : employee->grade;

        // Automatically checks if balanced and rotates accordingly.
        v = this->balance(v);

        v = v->parent;
    }

    // Decrement number of nodes in AVL Tree
    --this->nodes;
}

EarnerTreeNode *EarnerRankTree::removeTargetNode(EarnerTreeNode *remove_target, int* diff)
{
    EarnerTreeNode *parent = remove_target->parent;
    EarnerTreeNode **parent_ptr;

    // This logic is only relevant to non-root nodes
    if (remove_target->isLeft())
    {
        parent_ptr = &(parent->left);
    }
    else
    {
        parent_ptr = &(parent->right);
    }

    if (remove_target->isLeaf())
    {
        if (!remove_target->isRoot())
        {
            *parent_ptr = nullptr;
            parent->updateHeight();
        }
        else
        {
            this->root = nullptr;
        }

        delete remove_target;

        return parent;
    }

    if (remove_target->isSingleParent())
    {
        EarnerTreeNode *left = remove_target->left;
        EarnerTreeNode *right = remove_target->right;

        EarnerTreeNode *child = (left) ? left : right;

        // Point remove target node child and its parent to each other
        if (!remove_target->isRoot())
        {
            *parent_ptr = child;
            child->parent = parent;
        }
        else
        {
            this->root = child;
            child->parent = nullptr;
        }

        remove_target->left = nullptr;
        remove_target->right = nullptr;

        delete remove_target;

        if (parent)
        {
            parent->updateHeight();
        }
        else
        {
            child->updateHeight();
        }

        return parent;
    }

    // Otherwise, remove target has two children, proceed accordingly by removing the successor
    EarnerTreeNode *successor = remove_target->getSuccessor();

    // Swap between remove_target and successor locations
    this->swapNodes(remove_target, successor);

    return this->removeTargetNode(successor, diff);
}

void EarnerRankTree::swapNodes(EarnerTreeNode *node1, EarnerTreeNode *node2)
{
    KeyType key = node1->key;
    DataType value = node1->value;

    node1->key = node2->key;
    node1->value = node2->value;

    node2->key = key;
    node2->value = value;
}

EarnerTreeNode *EarnerRankTree::removeAux(const KeyType &key, EarnerTreeNode *node, int* diff)
{
    if (!node)
    {
        return nullptr;
    }

    EarnerTreeNode *removal_point;

    if (key < node->getKey())
    {
        removal_point = this->removeAux(key, node->left, diff);
    }
    else if (node->getKey() < key)
    {
        removal_point = this->removeAux(key, node->right, diff);
    }
    else
    {
        // Otherwise, we have found our target node to delete.
        removal_point = this->removeTargetNode(node, diff);
    }
    return removal_point;
}

void SortedNodeArrayFromTree(EarnerTreeNode *node, EarnerTreeNode **array, int *index)
{
    if (!node)
    {
        return;
    }

    SortedNodeArrayFromTree(node->getLeft(), array, index);

    array[*index] = node;
    ++(*index);

    node->sum_grades = node->getValue()->grade;
    node->top_workers_count = 1;

    SortedNodeArrayFromTree(node->getRight(), array, index);
}

EarnerTreeNode **MergeSortedNodeArrays(EarnerTreeNode **arr1, EarnerTreeNode **arr2, int len1, int len2)
{
    EarnerTreeNode **merged_array = new EarnerTreeNode *[len1 + len2];

    int i1 = 0;
    int i2 = 0;

    int i3 = 0;

    while (i1 < len1 && i2 < len2)
    {
        if (arr1[i1]->getKey() < arr2[i2]->getKey())
        {
            merged_array[i3] = arr1[i1];
            ++i1;
        }
        else
        {
            merged_array[i3] = arr2[i2];
            ++i2;
        }
        ++i3;
    }

    while (i1 < len1)
    {
        merged_array[i3] = arr1[i1];
        ++i1;
        ++i3;
    }

    while (i2 < len2)
    {
        merged_array[i3] = arr2[i2];
        ++i2;
        ++i3;
    }

    return merged_array;
}

EarnerTreeNode* ConvertSortedNodeArrayToAVL(EarnerTreeNode** array, int left, int right,
                                            EarnerTreeNode* parent)
{
    if (right < left)
    {
        return nullptr;
    }

    int mid = (left + right) / 2;

    EarnerTreeNode *node = array[mid];

    node->setParent(parent);

    node->setLeft(ConvertSortedNodeArrayToAVL(array, left, mid - 1, node));

    node->setRight(ConvertSortedNodeArrayToAVL(array, mid + 1, right, node));

    // Moral compass
    if (node->left){
        node->top_workers_count += node->left->top_workers_count;
        node->sum_grades += node->left->sum_grades;
    }
    if (node->right){
        node->top_workers_count += node->right->top_workers_count;
        node->sum_grades += node->right->sum_grades;
    }

    node->updateHeight();

    return node;
}

/**
 * @brief Merges two AVL trees into one.
 *
 * Stems both trees and they are undefined afterwards
 *
 * Takes O(tree1->getSize() + tree2->getSize()) time
 *
 * @tparam KeyType
 * @tparam DataType
 * @param tree1
 * @param tree2
 * @return EarnerTreeNode*
 */

EarnerRankTree* MergeEarnerRankTrees(EarnerRankTree* tree1, EarnerRankTree* tree2)
{
    EarnerTreeNode** tree1_array = new EarnerTreeNode *[tree1->getSize()];
    EarnerTreeNode** tree2_array = new EarnerTreeNode *[tree2->getSize()];

    int len1 = tree1->getSize();
    int len2 = tree2->getSize();

    int new_size = len1 + len2;

    int i1 = 0;
    int i2 = 0;

    SortedNodeArrayFromTree(tree1->getRoot(), tree1_array, &i1);
    SortedNodeArrayFromTree(tree2->getRoot(), tree2_array, &i2);

    // This action takes O(tree1->getSize() + tree2->getSize()) actions.
    EarnerTreeNode** merged_array = MergeSortedNodeArrays(tree1_array, tree2_array, len1, len2);

    delete[] tree1_array;
    delete[] tree2_array;

    EarnerTreeNode* root = ConvertSortedNodeArrayToAVL(merged_array, 0, len1 + len2 - 1, nullptr);

    EarnerRankTree* tree = new EarnerRankTree();

    tree->setRoot(root);
    tree->setSize(new_size);

    // Stem both trees
    tree1->stem();
    tree2->stem();

    delete[] merged_array;

    return tree;
}