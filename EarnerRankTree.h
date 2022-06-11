#ifndef EarnerRankTree_H_
#define EarnerRankTree_H_

#include "Employee.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <exception>


typedef struct salary_key_t
{
    int salary;
    int EmployeeID;
} SalaryKey;

inline bool operator<(const SalaryKey &a, const SalaryKey &b)
{
    if (a.salary < b.salary)
    {
        return true;
    }
    if (b.salary < a.salary)
    {
        return false;
    }

    return a.EmployeeID < b.EmployeeID;
}

typedef SalaryKey KeyType;
typedef Employee* DataType;

class EarnerTreeNode
{
public:
    KeyType key;
    DataType value;

    EarnerTreeNode *parent;
    EarnerTreeNode *left;
    EarnerTreeNode *right;

    int height;

    int top_workers_count;
    int sum_grades;

     void postOrderDeleteChildNodes();

    void print(const std::string &prefix = "");

    friend class EarnerRankTree;

    // This is now a default destructor since nothing is being dynamically allocated
    ~EarnerTreeNode() = default;

    // Key and value constructors now assume existence of copy constructor, nothing is being dynamically allocated
    EarnerTreeNode(const KeyType &key, const DataType &value, EarnerTreeNode *parent = nullptr, EarnerTreeNode *left = nullptr, EarnerTreeNode *right = nullptr);

    EarnerTreeNode *getSuccessor();
    EarnerTreeNode *getTreeMinimum();

    EarnerTreeNode *findWithinSubtree(const KeyType &key);

    EarnerTreeNode *getLeft() const
    {
        return this->left;
    }
    EarnerTreeNode *getRight() const
    {
        return this->right;
    }

    int getHeight() const
    {
        return this->height;
    }

    const KeyType &getKey()
    {
        return this->key;
    }

    DataType &getValue()
    {
        return this->value;
    }

    bool isRoot() const
    {
        return (this->parent == nullptr);
    }

    bool isLeft() const
    {
        return (!this->isRoot() && (this->parent->left == this));
    }

    void setParent(EarnerTreeNode *new_parent)
    {
        this->parent = new_parent;
    }

    void setLeft(EarnerTreeNode *new_left)
    {
        this->left = new_left;
    }

    void setRight(EarnerTreeNode *new_right)
    {
        this->right = new_right;
    }

    /*

    This is no longer needed
    Change this in functions

    bool outOfBounds() const {
        return (this->parent == nullptr);
    }

    */

    // This function relies on the existence of a dummy node. If removed, update to 'this'

    bool isLeaf() const
    {
        return (this->left == nullptr && this->right == nullptr);
    }

    bool isSingleParent() const
    {
        return (this->left == nullptr && this->right != nullptr) || (this->left != nullptr && this->right == nullptr);
    }

    void updateHeight()
    {
        this->height = std::max(this->leftHeight(), this->rightHeight()) + 1;
    }

    int leftHeight() const
    {
        return (this->left) ? left->height : -1;
    }

    /**
     * Getter of right AVL tree height.
     * Define NULL right AVL tree height to be -1
     */
    int rightHeight() const
    {
        return (this->right) ? right->height : -1;
    }
    /**
     * Getter of AVL tree balance factor.
     */
    int balanceFactor() const
    {
        // Note: height of a childless singleton is still 0
        return this->leftHeight() - this->rightHeight();
    }

    bool isBalanced() const
    {
        int bf = this->balanceFactor();

        return bf >= -1 && bf <= 1;
    }
};

class EarnerRankTree
{
private:
    EarnerTreeNode *root;
    int nodes;

    EarnerTreeNode *removeAux(const KeyType &key, EarnerTreeNode *node, int* diff);
    EarnerTreeNode *removeTargetNode(EarnerTreeNode *remove_target, int* diff);

    void swapNodes(EarnerTreeNode *node1, EarnerTreeNode *node2);

public:
    EarnerRankTree()
    {
        this->root = nullptr;
        this->nodes = 0;
    }

    ~EarnerRankTree()
    {
        if (this->root)
        {
            this->root->postOrderDeleteChildNodes();
        }
    }

    // Exceptions
    class Exception : public std::exception
    {
    public:
        explicit Exception(const char *message)
            : msg_(message) {}

        explicit Exception(const std::string &message)
            : msg_(message) {}

        virtual ~Exception() noexcept {}

        virtual const char *what() const noexcept override
        {
            return msg_.c_str();
        }

    protected:
        std::string msg_;
    };

    class KeyExists : public Exception
    {
        using Exception::Exception;
    };
    
    bool isEmpty() const
    {
        return (this->root == nullptr);
    }

    EarnerTreeNode *getRoot()
    {
        return this->root;
    }

    EarnerTreeNode *balance(EarnerTreeNode *node);
    EarnerTreeNode *LLRotate(EarnerTreeNode *node);
    EarnerTreeNode *RRRotate(EarnerTreeNode *node);

    EarnerTreeNode *find(const KeyType &key);
    EarnerTreeNode *insert(const KeyType &key, const DataType &value);


    EarnerTreeNode *min()
    {
        if (this->isEmpty())
        {
            return nullptr;
        }

        EarnerTreeNode *current = this->root;

        while (current->left)
        {
            current = current->left;
        }

        return current;
    }

    EarnerTreeNode *max()
    {

        if (this->isEmpty())
        {
            return nullptr;
        }

        EarnerTreeNode *current = this->root;

        while (current->right)
        {
            current = current->right;
        }

        return current;
    }

    void remove(const KeyType &key);

    void print()
    {
        this->root->print();
    }

    int getHeight()
    {
        return this->root->height;
    }

    int getSize()
    {
        return this->nodes;
    }

    void setSize(int new_size)
    {
        this->nodes = new_size;
    }

    // Therse are  advanced auxillary function that alter the tree's root
    void setRoot(EarnerTreeNode *root)
    {
        this->root = root;
    }

    void stem()
    {
        this->root = nullptr;
    }
};

void SortedNodeArrayFromTree(EarnerTreeNode *node, EarnerTreeNode **array, int *index);

EarnerTreeNode **MergeSortedNodeArrays(EarnerTreeNode **arr1, EarnerTreeNode **arr2, int len1, int len2);

EarnerTreeNode* ConvertSortedNodeArrayToAVL(EarnerTreeNode** array, int left, int right,
                                            EarnerTreeNode* parent);

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

EarnerRankTree* MergeEarnerRankTrees(EarnerRankTree* tree1, EarnerRankTree* tree2);

#endif