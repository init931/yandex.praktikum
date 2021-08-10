#include <cassert>

template <typename T>
struct TreeNode {
    T value;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

template <typename T>
void DeleteTree(TreeNode<T>* node) {
    if (!node) {
        return;
    }
    DeleteTree(node->left);
    DeleteTree(node->right);
    delete node;
}

template <typename T>
bool CheckTreeProperty(const TreeNode<T>* node) {
    // реализуйте функцию
    if (node->left && (node->value < node->left->value 
            || !CheckTreeProperty(node->left))) {

        return false;
    }
    if (node->right && (node->value > node->right->value 
            || !CheckTreeProperty(node->right))) {

        return false;
    }
    return true;
}

int main() {
    using T = TreeNode<int>;
    T* root1 = new T{6, 
        new T{4, new T{3}, new T{5}}, new T{7}};
    assert(CheckTreeProperty(root1));

    T* root2 = new T{6, 
        new T{4, new T{3}, new T{5}}, new T{7, new T{8}}};
    assert(!CheckTreeProperty(root2));

    DeleteTree(root1);
    DeleteTree(root2);
}
