#include<iostream>
using namespace std;


template <class T>
class Deque{

    int front_ptr,rear_ptr;
    unsigned int cur_size;
    unsigned int cur_capacity;
    T* buffer;

    public:
    //constructor
    Deque();
    // member fuctions
    void deque();
    void deque(unsigned int size, const T initial);
    void push_back(const T val);
    void push_front(const T val);
    void pop_back();
    void pop_front();
    T front();
    T back();
    T operator[](unsigned int index);
    bool empty();
    unsigned int size();
    void reserve(unsigned int cap);
    void resize(unsigned int size, const T def);
    void clear();
    void display();

};

template <class T>
Deque<T>::Deque(){
    /* Default Constructor to initialise Deque
    */
    front_ptr=-1;
    rear_ptr=-1;
    cur_size=0;
    cur_capacity=1;
    buffer =  new T[cur_capacity];

}

template <class T>
void Deque<T>::deque(){
    /* Default Fuction to initialise Deque
    */
    front_ptr=-1;
    rear_ptr=-1;
    cur_size=0;
    cur_capacity=1;
    buffer =  new T[cur_capacity];

}

template <class T>
void Deque<T>::deque(unsigned int size, const T initial){
    /* Fuction to initialise Deque with initial values and given size
    Args: 
        size(unsigned int): Size of deque
        initial(T): Value to be initialised with  
    */

    front_ptr=0;
    rear_ptr=size-1;
    cur_size=size;
    cur_capacity=size;
    buffer = new T[size];
    for (unsigned int i = 0; i < size; i++)
        buffer[i] = initial;
    
}

template <class T>
void Deque<T>::push_back(const T val){
    /* Fuction to push value at rear end of deque
    Args: 
        val(T): value to be pushed
    */
    if (front_ptr==-1){
        front_ptr++;
        rear_ptr++;
    }
    else {
        unsigned int temp=(rear_ptr+1)%cur_capacity;
        if(front_ptr==temp) {
            reserve(cur_capacity*2);
            rear_ptr++;
        }
        else rear_ptr=temp;
    }
    buffer[rear_ptr] = val;
    cur_size++;
}

template <class T>
void Deque<T>::push_front(const T val){
    /* Fuction to push value at front end of deque
    Args: 
        val(T): value to be pushed
    */
    if (front_ptr==-1){
        front_ptr++;
        rear_ptr++;
    }
    else {
        if(front_ptr==0) front_ptr=cur_capacity-1;
        else front_ptr--;
        if(front_ptr==rear_ptr) {
            front_ptr++;
            reserve(cur_capacity*2);
            front_ptr--;
            }
        }
    buffer[front_ptr] = val;
    cur_size++;
}

template <class T>
void Deque<T>::pop_back(){
    /* Fuction to pop value from the rear end of deque
    */
    if (front_ptr == -1)
    {
        cout<<"Deque is empty\n";
        return;
    }
    else if (front_ptr == rear_ptr)
        {
            front_ptr = -1;
            rear_ptr = -1;
        }
    else {
        if (rear_ptr==0) rear_ptr = cur_capacity-1;
        else rear_ptr--;
    }

    cur_size--;
}

template <class T>
void Deque<T>::pop_front(){
    /* Fuction to pop value from the front end of deque
    */
    if (front_ptr == -1)
    {
        cout<<"Deque is empty\n";
        return;
    }
    else if (front_ptr == rear_ptr)
        {
            front_ptr = -1;
            rear_ptr = -1;
        }
    else front_ptr = (front_ptr+1)%cur_capacity;
    cur_size--;
}

template <class T>
void Deque<T>::display(){
    /* Fuction to display the current queue values
    */
    if (rear_ptr == -1)
    {
        cout<<"Deque is Empty\n";
        return;
    }
    // cout<<"front_ptr="<<front_ptr<<" "<<"rear_ptr="<<rear_ptr<<" cap="<<cur_capacity<<'\n';
    if (rear_ptr >= front_ptr)
    {
        for (int i = front_ptr; i <= rear_ptr; i++)
            cout<<buffer[i]<<" ";
    }
    else
    {
        for (int i = front_ptr; i < cur_capacity; i++)
            cout<<buffer[i]<<" ";
 
        for (int i = 0; i <= rear_ptr; i++)
            cout<<buffer[i]<<" ";
    }
    cout<<'\n';
}

template <class T>
unsigned int Deque<T>::size(){
    /* Fuction that returns the current size of deque
    Returns:
        cur_size(unsigned int): Current size of deque
    */
    return cur_size;
}

template <class T>
void Deque<T>::reserve(unsigned int cap){
     /* Fuction that reserves deque with the given size
        It is expected to have the given size more than
        the current size
    Args:
        cap(unsigned int): New capacity of deque
    */
    T* new_buffer = new T[cap];
    if (rear_ptr >= front_ptr)
    {
        for (unsigned int i = front_ptr; i <= rear_ptr; i++)
            new_buffer[i] = buffer[i];
        
    }
    else
    {   unsigned int i;
        for (i = 1; i < cur_capacity-front_ptr+1 ; i++)
            new_buffer[cap-i] = buffer[cur_capacity-i];

        front_ptr=cap-i+1;
        for (i = 0; i <= rear_ptr; i++)
            new_buffer[i] = buffer[i];
    }
    
    cur_capacity = cap;
    
    delete[] buffer;
    buffer=new_buffer;
}

template <class T>
void Deque<T>::clear(){
    /* Fuction to clear the values of the deque
    */
    front_ptr=-1;
    rear_ptr=-1;
    cur_size=0;
    cur_capacity=1;
    T* new_buffer= new T;
    delete[] buffer;
    buffer = new_buffer;
}

template <class T>
T Deque<T>::front(){
    /* Fuction that returns the current front vlaue of deque
    Returns:
        front_value(T): Current front value
    */
    return buffer[front_ptr];
}
template <class T>
T Deque<T>::back(){
    /* Fuction that returns the current rear vlaue of deque
    Returns:
        rear_value(T): Current rear value
    */
    return buffer[rear_ptr];
}

template <class T>
T Deque<T>::operator[](unsigned int index){
    /* Fuction that returns the current vlaue of deque at
       the given index
    Args:
        index(unsigned int): Index value
    Returns:
        deque_val(T): Current value at given index
    */
    return buffer[(front_ptr+index)%cur_capacity];
}

template <class T>
bool Deque<T>::empty(){
    /* Fuction to check if the given array is empty
    Returns:
        bool: True if deque is empty
    */
    return front_ptr==-1;
}

template<class T>
void Deque<T>::resize(unsigned int size, const T def){
    /* Fuction to resize the deque with given size
       and defualt value
    Args:
        size(unsigned int): New deque size
        def(T): Default value for deque
    */
    if(cur_size==0){
        deque(size,def);
        return;
    }

    if(size>=cur_size){
        reserve(size);
        if (rear_ptr >= front_ptr)
        {
            for (unsigned int i = rear_ptr+1; i < cur_capacity; i++)
                buffer[i] = def;
            for (unsigned int i = 0; i < front_ptr; i++)
                buffer[i] = def;
            rear_ptr=front_ptr==0?cur_capacity-1:front_ptr-1;
        }   
        else
        {
            for (unsigned int i = rear_ptr+1; i < front_ptr; i++)
                buffer[i] = def;
            rear_ptr=front_ptr-1;
        }

    }
    else{
        
        if (rear_ptr >= front_ptr){
            reserve(size);
            rear_ptr=size-1;
            }
        else{
            T* new_buffer = new T[size];
            unsigned int i,t,remaining;
            //copying the size number of values to new buffer
            for(i=front_ptr,t=0; i<cur_capacity && t<size;i++,t++)
                new_buffer[t]=buffer[i];

            remaining=size-t;

            for(i=0; i<remaining;i++)
                new_buffer[t+i]=buffer[i];
            front_ptr=0;
            rear_ptr=size-1;
            cur_capacity=size;
            delete[] buffer;
            buffer=new_buffer;
        }
        
    }
    cur_size=size;
}

int main(){
    int choice, flag = 1, n;
    Deque<int> DQ; // or Deque<float> DQ;
    int val; // or float val;
    while(flag)
     {
        cout << "-------------------------\n";
        cout << "choice: ";
        cin >> choice;
        switch(choice) {
            case 0:
                flag = 0;
                break;
            case 1:
                DQ.deque();
                break;
            case 2:
                cin >> n >> val;
                DQ.deque(n, val);
                break;
            case 3:
                cin >> val;
                DQ.push_back(val);
                break;
            case 4:
                DQ.pop_back();
                break;
            case 5:
                cin >> val;
                DQ.push_front(val);
                break;
            case 6:
                DQ.pop_front();
                break;
            case 7:
                cout << DQ.front() << endl;
                break;
            case 8:
                cout << DQ.back() << endl;
                break;
            case 9:
                cout << ((DQ.empty()) ? "true" : "false") << endl;
                break;
            case 10:
                cout << DQ.size() << endl;
                break;
            case 11:
                cin >> n >> val;
                DQ.resize(n, val);
                break;
            case 12:
                DQ.clear();
                break;
            case 13:
                cin >> n;
                cout << DQ[n] << endl;
                break;
            case 14:
                // For Debugging
                DQ.display();
                break;
            default:
                cout << "Enter correct choice\n";
        }
    }
    return 0;
}