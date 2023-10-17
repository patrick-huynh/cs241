#include <stdexcept>
#include <ostream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

/* A binary tree node has data, pointer to left child
and a pointer to right child */
class node {
public:
    std::string value;
    std::string type;
    std::vector<node*> children
};
 
// A utility function to create a node
node* newNode(std::string value)
{
    node* temp = new node();
 
    temp->value = value;
    temp->type = "";
    temp->children = std::vector<node*>;
 
    return temp;
}
 
// A recursive function to construct Full from pre[].
// preIndex is used to keep track of index in pre[].
node* constructTreeUtil(int pre[], int* preIndex, int low,
                        int high, int size)
{
    // Base case
    if (*preIndex >= size || low > high)
        return NULL;
 
    // The first node in preorder traversal is root. So take
    // the node at preIndex from pre[] and make it root, and
    // increment preIndex
    node* root = newNode(pre[*preIndex]);
    *preIndex = *preIndex + 1;
 
    // If the current subarray has only one element, no need
    // to recur
    if (low == high)
        return root;
 
    // Search for the first element greater than root
    int i;
    for (i = low; i <= high; ++i)
        if (pre[i] > root->data)
            break;
 
    // Use the index of element found in preorder to divide
    // preorder array in two parts. Left subtree and right
    // subtree
    root->left = constructTreeUtil(pre, preIndex, *preIndex,
                                   i - 1, size);
    root->right
        = constructTreeUtil(pre, preIndex, i, high, size);
 
    return root;
}
 
// The main function to construct BST from given preorder
// traversal. This function mainly uses constructTreeUtil()
node* constructTree(int pre[], int size)
{
    int preIndex = 0;
    return constructTreeUtil(pre, &preIndex, 0, size - 1,
                             size);
}
 
// A utility function to print inorder traversal of a Binary
// Tree
void printInorder(node* node)
{
    if (node == NULL)
        return;
    printInorder(node->left);
    cout << node->data << " ";
    printInorder(node->right);
}

node readTree(int numChildren, std::istream &in) {
    std::string s;
    std::getline(in, s);
    std::string::difference_type newNumChildren = std::count(s.begin(), s.end(), ' ');
    std::vector<node> children;
    children.push_back(readTree())
    node n = {s, "", children};
    return n;
}   

int main() {
    std::istream &in = std::cin;
    node tree = readTree(3, in);

    try {
        
    } catch (const std::invalid_argument &e) {
        std::cerr << "ERROR" << std::endl;
    }
    return 0;
}