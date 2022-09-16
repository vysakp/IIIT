#include<iostream>
#include<unordered_map>
using namespace std;


struct Node{
        int val;
        int key;
        Node *prev=NULL;
        Node *next=NULL;
    };
class LRUCache {
    Node *head, *tail;
    int cur_cap;
    int cur_len=0;
    unordered_map<int, Node*> hmap;
public:
    LRUCache(int capacity) {
        cur_cap=capacity;
    }
    
    int get(int key) {
        
        if(hmap.find(key)!=hmap.end()){
            
            Node *prev_node, *cur_node, *next_node;
            cur_node = hmap[key];
            if(head==cur_node)
                return head->val;
            prev_node = cur_node->prev;
            next_node = cur_node->next;
            if(prev_node)
                prev_node->next = next_node;
            
            if(next_node)
                next_node->prev = prev_node;
            //this means it is tail node 
            else
                tail = prev_node;
            cur_node->next = head;
            cur_node->prev = NULL;
            head->prev = cur_node;
            head = cur_node;
            return head->val;
        }
        else
            return -1;
        
    }
    
    void put(int key, int value) {
        if(hmap.find(key)!=hmap.end()){
            //Node already present
            
            Node *prev_node, *cur_node, *next_node;
            cur_node = hmap[key];
            if(head==cur_node){
                head->val = value;
                return;
            }
            prev_node = cur_node->prev;
            next_node = cur_node->next;
            if(prev_node)
                prev_node->next = next_node;
            if(next_node)
                next_node->prev = prev_node;
            //this means it is tail node 
            else
                tail= prev_node;
            cur_node->next = head;
            cur_node->prev = NULL;
            cur_node->val = value;
            head->prev = cur_node;
            head = cur_node;
        }
        else{
            
            //Node not present
            if(cur_len<cur_cap){
                //add new node 
                if(cur_len==0){
                    head = new Node;
                    tail = head;
                    head->val = value;
                    head->key = key;
                    hmap[key] = head;
                }
                else{
                    Node *new_node = new Node;
                    head->prev = new_node;
                    new_node->next = head;
                    new_node->val = value;
                    new_node->key = key;
                    head = new_node;
                    hmap[key] = head;
                }
                
                cur_len++;
            }
            else{
                //remove the LRU node
                Node *new_node = new Node;
                Node *prev_node;
                new_node->val = value;
                new_node->key = key;
                prev_node = tail->prev;
                if(prev_node){
                    prev_node->next = NULL;
                    tail->prev = NULL;
                    hmap.erase(tail->key);
                    delete(tail);
                    tail = prev_node;
                    new_node->next = head;
                    head->prev = new_node;
                    head = new_node;
                }
                else{
                    //single node
                    hmap.erase(tail->key);
                    delete(tail);
                    tail = new_node;
                    head = new_node;
                }
                hmap[key] = head;
            }
                        
        }
    }
};


int main(){
    
    return 0;
}

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */