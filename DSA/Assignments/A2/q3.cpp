#include <iostream>
using namespace std;

struct TreeNode {
  int val;
  int height;
  TreeNode *left=NULL;
  TreeNode *right=NULL;
  TreeNode();
  TreeNode(int v) { 
    val = v; 
    height = 0;
    }
};

class AVL {
  TreeNode *root;

public:

    int getHeight(TreeNode * node){
        if(!node)
            return 0;
        else
            return node->height;
    }

    int getBF(TreeNode *node){
        if(!node)
            return -1;
        else
            return(getHeight(node->left)- getHeight(node->right));

    }

    TreeNode* RightRotate(TreeNode* node){
        TreeNode *Lchild = node->left;
        TreeNode *LRchild = Lchild->right;
        Lchild->right = node;
        node->left = LRchild;
        return Lchild;
    }


    TreeNode* LeftRotate(TreeNode* node){

        TreeNode *Rchild = node->right;
        TreeNode *RLchild = Rchild->left;
        Rchild->left = node;
        node->right = RLchild;
        return Rchild;
        
    }

    TreeNode* insert(TreeNode *root, TreeNode *new_node) {
        if(!root){
            root=new_node;
            return root;
            }
        else if(new_node->val<root->val)
            root->left=insert(root->left,new_node);
        else if(new_node->val>root->val)
            root->right=insert(root->right,new_node);
        else
            return root;
        
        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        
        int bf = getBF(root);

        if(bf > 1 && new_node->val>root->left->val){
            //LR imbalance
            root->left = LeftRotate(root->left);
            return(RightRotate(root));

        }
        else if(bf > 1 && new_node->val<root->left->val){
            //LL imbalance
            return(RightRotate(root));

        }
        else if(bf < -1 && new_node->val>root->right->val){
            //RR imbalance
            return(LeftRotate(root));

        }
        else if(bf < -1 && new_node->val<root->right->val){
            //RL imbalance
            root->right = RightRotate(root->right);
            return(LeftRotate(root));

        }
        TreeNode* Lchild = root->left;
        TreeNode* Rchild = root->right;
        if(Lchild)
            Lchild->height = 1 + max(getHeight(Lchild->left), getHeight(Lchild->right));
        if(Rchild)
            Rchild->height = 1 + max(getHeight(Rchild->left), getHeight(Rchild->right));
        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        return root;
        
  }
};
int main() { 

    AVL tree;
    TreeNode* root = new TreeNode(4);
    root=tree.insert(NULL, root);
    TreeNode* node = new TreeNode(3);   
    root=tree.insert(root,node);
    node = new TreeNode(2);   
    root=tree.insert(root,node);
    node = new TreeNode(1);   
    root=tree.insert(root,node);

    cout<<root->val;
    return 0; 
    
    }