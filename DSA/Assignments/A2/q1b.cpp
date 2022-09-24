#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
using namespace std;

struct Node {
    int key;
    int val;
    int freq;
    Node* prev=NULL;
    Node* next=NULL;
    Node(int k, int v){
        key = k;
        val = v;
        freq = 1;
    }
};

struct List{
    int len;
    Node* head;
    Node* tail;
    List(){
        head = new Node(0,0);
        tail = new Node(0,0);
        head->next = tail;
        tail->prev = head;
        len = 0;
    }
};


class LFUCache {
    int cur_cap;
    int cur_least_freq;
    int cache_len;
    //to keep track of length at each frequency;
    unordered_map<int,Node*> node_hmap;
    //storing head and tail
    unordered_map<int,List*> freq_hmap;
    public:
    LFUCache(int capacity) {
        cur_cap=capacity;
        cur_least_freq=1; 
        cache_len=0;
    }

    void insert(Node* node, List* f_list){
        // node_hmap[node->key] = node;
        Node* nhead = f_list->head->next;
        node->next = nhead;
        node->prev = f_list->head;
        f_list->head->next = node;
        nhead->prev = node;
        f_list->len++;
    }
    
    void remove(Node* node, List* f_list){
         Node* prev_node = node->prev;
         Node* next_node = node->next;
         prev_node->next = next_node;
         next_node->prev = prev_node;
        //  node_hmap.erase(node->key);
         f_list->len--;

    }
    
    int get(int key){
        if(node_hmap.find(key)!=node_hmap.end()){
            //Node present in the cache
            Node *cur_node = node_hmap[key];
            int val = cur_node->val;

            node_hmap.erase(cur_node->key);
            remove(cur_node, freq_hmap[cur_node->freq]);
            if(freq_hmap[cur_node->freq]->len==0 && cur_node->freq==cur_least_freq)
                cur_least_freq++;

            List* new_list = new List();
            cur_node->freq++;
            if(freq_hmap.find(cur_node->freq)!=freq_hmap.end())
                new_list=freq_hmap[cur_node->freq];
            // else 
            //     freq_hmap[cur_least_freq] = new_list;
            insert(cur_node, new_list);
            freq_hmap[cur_node->freq] = new_list;
            node_hmap[cur_node->key] = cur_node;

            return val;
        }
        else
            return -1;
    }
    
    void put(int key, int value) {

        if(node_hmap.find(key)!=node_hmap.end()){
            //Node present in the cache
            Node *cur_node = node_hmap[key];
            cur_node->val = value;
            node_hmap.erase(cur_node->key);
            remove(cur_node, freq_hmap[cur_node->freq]);
            if(freq_hmap[cur_node->freq]->len==0 && cur_node->freq==cur_least_freq)
                cur_least_freq++;

            List* new_list = new List();
            cur_node->freq++;
            if(freq_hmap.find(cur_node->freq)!=freq_hmap.end())
                new_list=freq_hmap[cur_node->freq];
            insert(cur_node, new_list);
            freq_hmap[cur_node->freq] = new_list;
            node_hmap[cur_node->key] = cur_node;
        }
        else{
            if (cur_cap==0)
                return;
            if(cache_len == cur_cap){
                List* cur_list = freq_hmap[cur_least_freq];
                int rm_key = cur_list->tail->prev->key;
                node_hmap.erase(rm_key);
                remove(cur_list->tail->prev, cur_list);
                cache_len--;  
            }
            cache_len++;
            cur_least_freq=1;
            List* list = new List;
            if(freq_hmap.find(cur_least_freq)!=freq_hmap.end()){
                list = freq_hmap[cur_least_freq];
            }
            Node* new_node = new Node(key,value);
            insert(new_node, list);
            node_hmap[new_node->key] = new_node;
            freq_hmap[cur_least_freq] = list;
        }
    }
};

int main(){
    int cap, q, opr, key, val;
    cin>>cap>>q;
    LFUCache cache(cap);
    for(int i=0;i<q;i++){
        cin>>opr;
        if(opr==1){
            //get operation
            cin>>key;
            cout<<cache.get(key)<<'\n';
        }
        else if(opr==2){
            //set operation
            cin>>key>>val;
            cache.put(key,val);
        }
        else{
            cout<<"Enter a valid operation";
        }
    }
    return 0;
}
/**
 * Your LFUCache object will be instantiated and called as such:
 * LFUCache* obj = new LFUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */