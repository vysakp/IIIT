#include <iostream>
#include<algorithm>
#include<queue>
#include<vector>

using namespace std;


struct Node{
    long long value;
    int index;
    Node(){}
    Node(long long _val, int _ind){
        value = _val;
        index = _ind;
    }
};

class MinHeap{
    int heap_size;
    int capacity;
    Node* min_heap;
    public:
    MinHeap();
    // void build_heap(Node * arr);
    void heapify( int i);
    void push(Node node);
    void pop();
    Node top();
    bool empty();
};

MinHeap::MinHeap(){
    heap_size=0;
    capacity=3;
    min_heap = new Node[3];
}

void MinHeap::heapify(int i){
    Node cur_node = min_heap[i];
    int min_index = i;
    if(2*i<heap_size&&min_heap[2*i].value<min_heap[min_index].value)
        min_index = 2*i;
    if(2*i+1<heap_size&&min_heap[2*i+1].value<min_heap[min_index].value)
        min_index = 2*i+1;
    if (min_index==i)
        return;
    min_heap[i]=min_heap[min_index];
    min_heap[min_index] = cur_node;
    heapify(min_index); 
}

void MinHeap::push(Node node){
    heap_size++;
    if(heap_size>capacity){
        capacity*=2;
        Node* new_heap = new Node[capacity];
        for(int i=0;i<heap_size-1;i++)
            new_heap[i]=min_heap[i];
        min_heap = new_heap;
    }
    min_heap[heap_size-1] = node;
    int chld_ind = heap_size-1;
    int par_ind = chld_ind/2;
    while(chld_ind!=0 && min_heap[par_ind].value>min_heap[chld_ind].value){
        Node temp = min_heap[chld_ind];
        min_heap[chld_ind] = min_heap[par_ind];
        min_heap[par_ind] = temp;
        chld_ind = par_ind;
        par_ind = chld_ind/2;
    }  
}

Node MinHeap::top(){
    return min_heap[0];
}

void MinHeap::pop(){
    if(heap_size==0)
        return;
    min_heap[0] = min_heap[heap_size-1];
    heap_size--;
    heapify(0);
}

bool MinHeap::empty(){
    return heap_size==0;
}


void sort_arr(int* arr, int n){

    MinHeap sort_heap;

    for(int i = 0; i<n; i++){
        Node* new_node = new Node(arr[i],0);
        sort_heap.push(*new_node);
    }


    for(int i=0;i<n;i++){
        arr[i] = sort_heap.top().value;
        sort_heap.pop();
    }

}

int main(){

    int n,k;
    cin>>n>>k;
    int* popularity = new int[n];
    for(int i=0;i<n;i++){
        cin>>popularity[i];
    }

    int max_sum=0;
    for(int i=0; i<n;i++){
        if(popularity[i]>0)
            max_sum+=popularity[i];
        else
            popularity[i]*=-1;
    }

    //sorting the popularity using heap sort

    sort_arr(popularity,n);

    MinHeap ratings;
    Node root = Node(popularity[0] ,0);
    ratings.push(root);

    //for storing sum of rating in susequence
    long long sum_ratings[k];
    int req_values = 0;

    while(!ratings.empty()&&req_values<k-1){
        Node cur_rating = ratings.top();
        ratings.pop();
        long long value = cur_rating.value;
        int index = cur_rating.index;
        sum_ratings[req_values++] = value;
        if(index<n-1){
            Node with_prev = Node(value+popularity[index+1],index+1);
            Node with_out_prev = Node(value+popularity[index+1]-popularity[index], index+1);
            ratings.push(with_prev);
            ratings.push(with_out_prev);
        }
    }

    long long best_combos[k];
    best_combos[0] = max_sum;

    for(int i=1;i<k;i++)
        best_combos[i] = max_sum-sum_ratings[i-1];
    
    for(int i=0;i<k;i++)
     cout<<best_combos[i]<<" ";

    return 0;
}