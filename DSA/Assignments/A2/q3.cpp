#include <iostream>
using namespace std;


template <class T>
struct TreeNode {
  T val;
  int height;
  int count;
  int l_nodes;
  int r_nodes;
  TreeNode<T> *left=NULL;
  TreeNode<T> *right=NULL;
  TreeNode();
  TreeNode(T v) { 
    val = v; 
    height = 0;
    count = 1;
    l_nodes = 0;
    r_nodes = 0;
    }
};

template <class T>
class AVL {
  TreeNode<T> *root;

public:
    AVL(){
        root = NULL;
    }

    int getHeight(TreeNode<T> * node){
        /* Helper fuction that returns the  height of current node
        Args:
            node(TreeNode<T>*): Tree node
        Returns:
            height(int): Height of the node
        */
        if(!node)
            return -1;
        else
            return node->height ;
    }

    int getLcount(TreeNode<T> * node){
        /* Helper fuction that returns the  left node count of the  node
        Args:
            node(TreeNode<T>*): Tree node
        Returns:
            count(int): Left node count
        */
        if(!node)
            return 0;
        else
            return node->l_nodes + node->count - 1;
    }

    int getRcount(TreeNode<T> * node){
        /* Helper fuction that returns the  right node count of the  node
        Args:
            node(TreeNode<T>*): Tree node
        Returns:
            count(int): Right node count
        */
        if(!node)
            return 0;
        else
            return node->r_nodes;
    }

    int getBF(TreeNode<T> *node){
        /* Helper fuction that returns the  balance factor of the  node
        Args:
            node(TreeNode<T>*): Tree node
        Returns:
            balancefactor(int): Balance factor of the node
        */
        if(!node)
            return 0;
        else
            return(getHeight(node->left)- getHeight(node->right));

    }


    TreeNode<T>* RightRotate(TreeNode<T>* node){
        /* Fuction that does a right rotaion on the given node
        Args:
            node(TreeNode<T>*): Tree node
        Returns:
            node(TreeNode<T>*): Balanced node
        */
        TreeNode<T> *Lchild = node->left;
        TreeNode<T> *LRchild = Lchild->right;
        Lchild->right = node;
        node->left = LRchild;
        // if(LRchild)
        //     LRchild->height = 1 + max(getHeight(LRchild->left), getHeight(LRchild->right));
        if(LRchild)
            node->l_nodes =  1 + getLcount(LRchild) + getRcount(LRchild);
        else
            node->l_nodes = 0;

        Lchild->r_nodes = 1 + getLcount(node) + getRcount(node);
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        return Lchild;
    }

    TreeNode<T>* LeftRotate(TreeNode<T>* node){

        TreeNode<T> *Rchild = node->right;
        TreeNode<T> *RLchild = Rchild->left;
        Rchild->left = node;
        node->right = RLchild;
        // if(RLchild)
        //     RLchild->height = 1 + max(getHeight(RLchild->left), getHeight(RLchild->right));

        if(RLchild)
            node->r_nodes =  1 + getLcount(RLchild) + getRcount(RLchild);
        else
            node->r_nodes = 0;
        Rchild->l_nodes = 1 + getLcount(node) + getRcount(node);

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        return Rchild;
        
    }

    TreeNode<T>* insert(TreeNode<T> *root, TreeNode<T> *new_node) {
        if(!root){
            root=new_node;
            return root;
            }
        else if(new_node->val<root->val)
            root->left=insert(root->left,new_node);
        else if(new_node->val>root->val)
            root->right=insert(root->right,new_node);
        else{
            root->count++;
            return root;
            }
        
        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        TreeNode<T>* Lchild = root->left;
        TreeNode<T>* Rchild = root->right;
        if(Lchild)
            root->l_nodes = 1 + getLcount(Lchild) + getRcount(Lchild);
        else
            root->l_nodes = 0;
        if(Rchild)
            root->r_nodes = 1 + getLcount(Rchild) + getRcount(Rchild);
        else
            root->r_nodes = 0;
        
        
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

        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        return root;
        
  }

    void insert(T key){
        TreeNode<T>* new_node = new TreeNode<T>(key);
        root = insert(root,new_node);
    }

    TreeNode<T>* remove(TreeNode<T> *root, T key){
        if(root==NULL)
            return root;
        if(root->val>key)
            root->left = remove(root->left,key);
        else if(root->val<key)
            root->right = remove(root->right,key);
        else{
            TreeNode<T>* temp = root;
            if(!root->left&&!root->right){
                //leaf node
                root = NULL;
                delete temp;
                return root;
            }
            else if(!root->left||!root->right){
                //one child
                if(root->left){
                    root = root->left;
                    delete temp;
                    return root;
                }
                else{
                    root = root->right;
                    delete temp;
                    return root;
                }
            }
            else{
                //two child
                //get max val in left subtree
                TreeNode<T>* l_max = root->left;
                while(l_max->right)
                    l_max = l_max->right;
                
                root->val = l_max->val;

                root->left = remove(root->left, l_max->val);


            }
        }
        

            if (!root)
                return root;
            TreeNode<T>* Lchild = root->left;
            TreeNode<T>* Rchild = root->right;
            if(Lchild)
                root->l_nodes = 1 + getLcount(Lchild) + getRcount(Lchild);
            else
                root->l_nodes = 0;
            if(Rchild)
                root->r_nodes = 1 + getLcount(Rchild) + getRcount(Rchild);
            else
                root->r_nodes = 0;
        

            int bf = getBF(root);

            if(bf > 1){
                if (getBF(root->left)>=0){
                    //LL imbalance
                    return(RightRotate(root));
                    }
                else{
                    //LR imbalance
                    root->left = LeftRotate(root->left);
                    return(RightRotate(root));
                }
            }
            else if(bf < -1){

                if(getBF(root->right)<=0){
                    //RR imbalance
                    return(LeftRotate(root));
                }
                else{
                    //RL imbalance
                    root->right = RightRotate(root->right);
                    return(LeftRotate(root));
                }

            }
            root->height = 1 + max(getHeight(root->left), getHeight(root->right));
            return root;

        }

    void remove(T key){
        root = remove(root,key);
    }

    bool search(TreeNode<T> *root,T key){
        if(root==NULL)
            return root;
        if(root->val>key)
            return search(root->left,key);
        if(root->val<key)
            return search(root->right,key);
        return true;
    }

    bool search(T key){
        return search(root,key);
    }

    int count_occurence(TreeNode<T> *root,T key){
        if(root==NULL)
            return 0;
        if(root->val>key)
            return count_occurence(root->left,key);
        if(root->val<key)
            return count_occurence(root->right,key);
        return root->count;
        }
    
    int count_occurence(T key){
        return count_occurence(root, key);
    }
    
    T upper_bound(TreeNode<T> *root,T key){
        //returns number greater than key
        if(root==NULL)
            return T();
        if(root->val>key){
            if(root->left&&root->left->val<=key)
                return root->val;
            return upper_bound(root->left,key);
            }
        else if(root->val<key){
            if(root->right&&root->right->val>key)
                return root->right->val;
            return upper_bound(root->right,key);
            }
        else {
            if(root->right)
                return root->right->val;
            return root->val;
        };
    }

    T upper_bound(T key){
        return upper_bound(root,key);
    }

    T lower_bound(TreeNode<T> *root,T key){
        if(root==NULL)
            return T();
        if(root->val>key){
            if(root->left&&root->left->val<=key)
                return root->val;
            return lower_bound(root->left,key);
            }
        else if(root->val<key){
            if(root->right&&root->right->val>key)
                return root->right->val;
            return lower_bound(root->right,key);
            }
        else 
            return root->val;
    }

    T lower_bound(T key){
        return lower_bound(root,key);
    }

    T abs_lower_bound(TreeNode<T> *root,T key){
        //returns number greater than key
        if(root==NULL)
            return T();
        if(root->val>key){
            if(root->left&&root->left->val<key)
                return root->left->val;
            return abs_lower_bound(root->left,key);
            }
        else if(root->val<key){
            if(root->right&&root->right->val>key)
                return root->val;
            return abs_lower_bound(root->right,key);
            }
        else {
            if(root->left)
                return root->left->val;
            return root->val;
        };
    }

    T abs_lower_bound(T key){

        return abs_lower_bound(root,key);
    }

    T closest_element(TreeNode<T> *root, T key){
        T u_bound = upper_bound(root, key);
        T l_bound = abs_lower_bound(root, key);
        if(u_bound-key<key-l_bound)
            return u_bound;
        return l_bound;
    }

    T closest_element(T key){
        if(root&&key>root->val){
            TreeNode<T>* treeMax = root;
            while(treeMax->right)
                treeMax = treeMax->right;
            if (key>treeMax->val)
                return treeMax->val;
        }
        else if(root&&key<root->val){
            TreeNode<T>* treeMin = root;
            while(treeMin->left)
                treeMin = treeMin->left;
            if(key<treeMin->val)
                return treeMin->val;
        }
        return closest_element(root,key);
    }

    T Kth_largest(TreeNode<T> *root, int k){
        if(k>=root->l_nodes+1 && k<=root->l_nodes+root->count)
            return root->val;
        if(k<root->l_nodes+1)
            return Kth_largest(root->left,k);

        // if(k>root->l_nodes+root->count)
        return Kth_largest(root->right,k-(root->l_nodes+1));
    }

    T Kth_largest(int k){
        if(k<1||k>root->l_nodes+root->count+root->r_nodes)
            return -1;
        return Kth_largest(root,k);
    }

    int count_range(TreeNode<T> *root, T eLeft, T eRight){
        if(!root)
            return 0;
        if(root->val>=eLeft && root->val<=eRight)
            return root->count + count_range(root->left, eLeft, eRight) + count_range(root->right, eLeft, eRight);
        if(root->val<eLeft)
            return count_range(root->left, eLeft, eRight);
        return count_range(root->right, eLeft, eRight);    
    }

    int count_range(T eLeft, T eRight){
        return count_range(root,eLeft,eRight);
    }
    // T Kth_largest(int k)


    void printBT(const std::string& prefix, const TreeNode<T>* node, bool isRight){
    if( node != nullptr ){
        cout << prefix;
        cout << (isRight ? "├──" : "└──" );

        cout << node->val<<"("<<node->height<<','<<node->count<<','<<node->l_nodes<<','<<node->r_nodes<<")" << endl;

        printBT( prefix + (isRight ? "│   " : "    "), node->right, true);
        printBT( prefix + (isRight ? "│   " : "    "), node->left, false);
    }
    }

    void printBT(){
        printBT("", root, false);    
    }
};
int main() { 

    AVL<float> tree;
    tree.insert(50); 
    tree.insert(20); 
    tree.insert(60); 
    tree.insert(10);
    tree.insert(8);  
    tree.insert(15); 
    tree.insert(32);  
    tree.insert(46); 
    tree.insert(11);
    tree.insert(48);
    // tree.remove(46);                                     


    tree.printBT();
    cout<<tree.count_range(8,60);
    return 0; 
    
    }