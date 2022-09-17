#include<iostream>
using namespace std;

// template <class T>

// template <class T>
// SMLinkeList<T> SMLinkeList<T>::operator+(SMLinkeList a){

//     SMLinkeList<T> Res;
//     Res.row = row;
//     Res.col = col;
//     Node<T>* p1 = head;
//     Node<T>* p2 = a.head;
//     while(p1&&p2){
//         if(p1)
//     }


//     return Res;
// }


template <class T>
class SMArray{
    public:
    int row,col;
    unsigned int cur_size;
    unsigned int cur_capacity;
    T** matrix = new T*[3];

    SMArray();
    SMArray<T> transpose();

    void push_back( T vals[3]);

    void push_front( T vals[3]);

    void display();

    SMArray<T>  operator+( SMArray a);
    SMArray<T>  operator*( SMArray a);




};
template<class T>
SMArray<T>::SMArray(){
    //default constuctor
    cur_size = 0;
    cur_capacity=3;
    for(int i=0;i<3;i++)
        matrix[i] = new T[cur_capacity];
}

template<class T>
void SMArray<T>::push_back(T vals[]){
    T r = vals[0];
    T c = vals[1];
    T v = vals[2];
    if(cur_size<cur_capacity){
        matrix[0][cur_size] = r;
        matrix[1][cur_size] = c;
        matrix[2][cur_size] = v;
        cur_size++;
        
    }
    else{
        cur_capacity*=2;
        T** new_matrix = new T*[3];
        for(int i=0;i<3;i++)
            new_matrix[i] = new T[cur_capacity];
        for(int i=0;i<cur_size;i++){
            new_matrix[0][i] = matrix[0][i];
            new_matrix[1][i] = matrix[1][i];
            new_matrix[2][i] = matrix[2][i];
        }
        matrix = new_matrix;
        matrix[0][cur_size] = r;
        matrix[1][cur_size] = c;
        matrix[2][cur_size] = v;
        cur_size++;
    }
}

template<class T>
void SMArray<T>::push_front(T vals[]){
    if(cur_size==0){
            push_back(vals);
            return;
            }
    bool added = false;
    for(int i=0;i<cur_size;i++){
        
        if(matrix[0][i]==vals[0]){
            if(matrix[1][i]==vals[1]){
                matrix[0][i] = vals[0];
                matrix[1][i] = vals[1];
                matrix[2][i] += vals[2];
                added = true;
                break;
                }
            else if(matrix[1][i]>vals[1]&&matrix[0][i]>=vals[0]){
                //if sizes are equal add a dummy value 
                bool increased = false;
                if (cur_size==cur_capacity){
                    T new_val[3] = {-1,-1,-1};
                    push_back(new_val);
                    increased = true;
                    }
                

                for(int j=cur_size+1;j>i;j--){
                    matrix[0][j] = matrix[0][j-1];
                    matrix[1][j] = matrix[1][j-1];
                    matrix[2][j] = matrix[2][j-1];
                }
                matrix[0][i] = vals[0];
                matrix[1][i] = vals[1];
                matrix[2][i] = vals[2];
                if(!increased)
                    cur_size++;
                added= true;
                break;
            }
            

        }
        
        // else if(vals)

    }

    if(!added)
        push_back(vals);
}

template <class T>
SMArray<T> SMArray<T>::operator+(SMArray a){

    SMArray<T> Res;
    Res.row = row;
    Res.col = col;

    int i=0,j=0,k=0;
    while(i<cur_size&&j<a.cur_size){
        if(matrix[0][i]<a.matrix[0][j]){
            T new_val[3] = {matrix[0][i],matrix[1][i],matrix[2][i]};
            Res.push_back(new_val);
            i++;
        }
        else if(matrix[0][i]>a.matrix[0][j]){
            T new_val[3] = {a.matrix[0][j],a.matrix[1][j],a.matrix[2][j]};
            Res.push_back(new_val);
            j++;
        }
        else{
            if(matrix[1][i]<a.matrix[1][j]){
            T new_val[3] = {matrix[0][i],matrix[1][i],matrix[2][i]};
            Res.push_back(new_val);
            i++;
            }
            else if(matrix[1][i]>a.matrix[1][j]){
                T new_val[3] = {a.matrix[0][j],a.matrix[1][j],a.matrix[2][j]};
                Res.push_back(new_val);
                j++;
            }
            else {

                //  cout<<k<<" ";
                T new_val[3] = {a.matrix[0][j],a.matrix[1][j],a.matrix[2][j]+matrix[2][i]};
                Res.push_back(new_val);
                j++;
                i++;
            }
        }

         k++;
    }
    while(i<cur_size){
        T new_val[3] = {matrix[0][i],matrix[1][i],matrix[2][i]};
            Res.push_back(new_val);
            i++;
    }
    // cout<<" "<<i<<" "<<j<<" "<<a.cur_size<<" "<<Res.cur_size<<'\n';
    while(j<a.cur_size){
         T new_val[3] = {a.matrix[0][j],a.matrix[1][j],a.matrix[2][j]};
        Res.push_back(new_val);
        j++;
}


    return Res;
}

template<class T>
SMArray<T> SMArray<T>::transpose(){
    SMArray<T> Res;
    for(int i=0;i<col;i++){
        for(int j=0;j<cur_size;j++){
            if(matrix[1][j]==i){
                T new_val[3] = {matrix[1][j],matrix[0][j],matrix[2][j]};
                Res.push_back(new_val);
            }
        }
    }
    Res.row = col;
    Res.col = row;
    return Res;
}

template <class T>
SMArray<T> SMArray<T>::operator*(SMArray a){
    if(col!=a.row){
        SMArray<T> null;
        return null;
        }
    SMArray<T> Res;
    Res.row = row;
    Res.col = col;
    a = a.transpose();
    
    
    for(int i=0;i<cur_size;i++){
        for(int j=0;j<a.cur_size;j++){
            if(matrix[1][i]==a.matrix[1][j]){
                T new_val[3] = {matrix[0][i],a.matrix[0][j],matrix[2][i]*a.matrix[2][j]};
                Res.push_back(new_val);
            }
        }
    }
    
   

    
    
    //Creating a new result object that will avoid the negative values
    SMArray<T> FRes;
    FRes.row = row;
    FRes.col = a.row; //since we did a transpose
    for(int i=0;i<Res.cur_size;i++){
        T new_val[3] = {Res.matrix[0][i],Res.matrix[1][i],Res.matrix[2][i]};
        FRes.push_front(new_val);
    }

    

    return FRes;
}

template <class T>
void SMArray<T>::display(){

    int k=0;
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            if(i==matrix[0][k]&&j==matrix[1][k]){
                cout<<matrix[2][k]<<" ";
                k++;
                }
            else
                cout<<0<<" ";
                }
            cout<<'\n';
            
    }
}


template <class T>
struct Node{
        T r;
        T c;
        T val;
        Node* next = NULL;
        Node(){}
        Node(T _r, T _c, T _val){
            r = _r;
            c = _c;
            val = _val;
        }
    };

template <class T>
class SMLinkeList{
    unsigned int cur_size;
    unsigned int cur_capacity;
    Node<T>* tail;
    Node<T>* head;

    public:
    int row,col;
    
    SMLinkeList();
    
    void push_back( T vals[3]);

    void push_front( T vals[3]);

    SMLinkeList<T> transpose();
    // void push_front( T vals[3]);

    void display();

    SMLinkeList<T>  operator+( SMLinkeList a);

    SMLinkeList<T>  operator*( SMLinkeList a);
};
template<class T>
SMLinkeList<T>::SMLinkeList(){
    head = NULL;
    tail = NULL;
}


template<class T>
void SMLinkeList<T>::push_back(T vals[3]){
    // cout<<vals[2]<<" ";
    if(!head){
        head = new Node<T>(vals[0], vals[1], vals[2]);
        tail = head;
        return;
    }
    Node<T>* new_node = new Node<T>(vals[0], vals[1], vals[2]);
    tail->next = new_node;
    tail = new_node;
}

template<class T>
void SMLinkeList<T>::push_front(T vals[3]){
    if(!head){
        push_back(vals);
        return;
    }
    Node<T>* ptr = head;
    Node<T>* prev = NULL;
    bool added = false;
    while(ptr){
        if(ptr->r==vals[0]){
            if(ptr->c==vals[1]){
                ptr->val+=vals[2];
                added = true;
                break;
            }
        }
        else if(ptr->c>vals[1]&&ptr->r>=vals[0]){
             Node<T>* new_node = new Node(vals[0],vals[1],vals[2]);
            // if(prev){
            //     prev->next = new_node;
            //     new_node->next = ptr;
            //     break;
            // }
            // else{
            //     new_node->next = head;
            //     head = new_node;
            // }
            prev = ptr->next;
            ptr->next = new_node;
            new_node->next = prev;
            added=true;
        }
        // prev = ptr;
        ptr = ptr->next;

    }
    if(!added)
        push_back(vals);

}

template<class T>
void SMLinkeList<T>::display(){
    Node<T>* ptr = head;
    cout<<row<<" "<<col;
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            if(ptr&&i==ptr->r&&j==ptr->c){
                cout<<ptr->val<<" ";
                ptr = ptr->next;
                }
            else
                cout<<0<<" ";
                }
            cout<<'\n';
            
    }
}

template<class T>
SMLinkeList<T> SMLinkeList<T>::transpose(){
    SMLinkeList<T> Res;

    for(int i=0;i<col;i++){
        Node<T>* ptr = head;
        while(ptr){
            // cout<<ptr->val<<" ";
            if(ptr->c==i){
                T new_val[3] = {ptr->c,ptr->r,ptr->val};
                Res.push_back(new_val);
            }
            ptr = ptr->next;
        } 
    }

    Res.col = row;
    Res.row = col;
    return Res;
}


template <class T>
SMLinkeList<T> SMLinkeList<T>::operator+(SMLinkeList a){
    SMLinkeList<T> Res;
    Res.row = row;
    Res.col = col;
    Node<T>* ptr1 = head;
    Node<T>* ptr2 = a.head;

    while(ptr1&&ptr2){
        if(ptr1->r<ptr2->r){
            T new_val[3] = {ptr1->r,ptr1->c,ptr1->val};
            ptr1 = ptr1->next;
            Res.push_back(new_val);
        }
        else if(ptr1->r>ptr2->r){
            T new_val[3] = {ptr2->r,ptr2->c,ptr2->val};
            ptr2 = ptr2->next;
            Res.push_back(new_val);
        }
        else{
            if(ptr1->c<ptr2->c){
                T new_val[3] = {ptr1->r,ptr1->c,ptr1->val};
                ptr1 = ptr1->next;
                Res.push_back(new_val);
            }
            else if(ptr1->c>ptr2->c){
                T new_val[3] = {ptr2->r,ptr2->c,ptr2->val};
                ptr2 = ptr2->next;
                Res.push_back(new_val);
            }
            else{
                T new_val[3] = {ptr2->r,ptr2->c,ptr2->val+ptr1->val};
                // cout<<new_val[0]<<" "<<new_val[1]<<" "<<new_val[2]<<" "<<'\n';
                Res.push_back(new_val);
                ptr1 = ptr1->next;
                ptr2 = ptr2->next;
            }
        }
    }
    while(ptr1){
        T new_val[3] = {ptr1->r,ptr1->c,ptr1->val};
        ptr1 = ptr1->next;
        Res.push_back(new_val);
    }
    while(ptr2){
        T new_val[3] = {ptr2->r,ptr2->c,ptr2->val};
        ptr2 = ptr2->next;
        Res.push_back(new_val);
    }
    return Res;

}

template <class T>
SMLinkeList<T> SMLinkeList<T>::operator*(SMLinkeList a){
    if(col!=a.row){
        SMLinkeList<T> null;
        return null;
        }
    SMLinkeList<T> Res;
    a = a.transpose();

    Node<T>* ptr1 = head;
    Node<T>* ptr2 = a.head;
    while(ptr1){
        ptr2 = a.head;
        while(ptr2){

            // cout<<ptr2->c<<'\n';
            if(ptr1->c==ptr2->c){
                // cout<<ptr2->r<<" "<<ptr2->c<<" "<<ptr2->val<<'\n';
                T new_val[3] = {ptr1->r,ptr2->r,ptr1->val*ptr2->val};
                Res.push_back(new_val);
            }
            ptr2 = ptr2->next;
        }
        ptr1 = ptr1->next;
    }
    // Res.row = row;
    // Res.col = col;
    SMLinkeList<T> FRes;
    FRes.row = row;
    FRes.col = a.row; //since we did a transpose;

    Node<T>* ptr = Res.head;
    while(ptr){
        T new_val[3] = {ptr->r,ptr->c,ptr->val};
        FRes.push_front(new_val);
        ptr = ptr->next;
    }
    Node<T>* ptr3 = FRes.head;
    while(ptr3){
        cout<<ptr3->r<<" "<<ptr3->c<<" "<<ptr3->val<<" "<<'\n';
        ptr3=ptr3->next;
    }
    // cout<<"\n";
    // cout<<FRes.row<<" "<<FRes.col;

    return FRes;
}


int main(){

    SMLinkeList<int> SM1, SM2;
    // SMArray<int> SM2;

    int r1,c1;
    int r2,c2;
    int ele;
    cin>>r1>>c1;
    // r1 = 2;
    // c1 = 2;
    // int m1[2][2] = {{1,1}, {0,1}};
    SM1.row = r1;
    SM1.col = c1;
    for (int i = 0; i < r1; i++)
        for (int j = 0; j < c1; j++){
            cin>>ele;
            if (ele != 0){
                int new_ele[3];
                new_ele[0] = i;
                new_ele[1] = j;
                new_ele[2] = ele;
                SM1.push_back(new_ele);
            }
        }
    cin>>r2>>c2;
    SM2.row = r2;
    SM2.col = c2;

    // int m2[2][2] = {{2,1}, {1,0}};
    for (int i = 0; i < r2; i++)
        for (int j = 0; j < c2; j++){
            cin>>ele;

            if (ele != 0){
                int new_ele[3];
                new_ele[0] = i;
                new_ele[1] = j;
                new_ele[2] = ele;

                SM2.push_back(new_ele);
            }
        }
    // SM2.display();
    SMLinkeList<int> SMRes = SM1*SM2;

    // SM1 = SM1.transpose();
    // SMRes.display();

    
    // cin>>r2>>c2;
    // SM2.row = r2;
    // SM2.col = c2;
    // for (int i = 0; i < r2; i++)
    //     for (int j = 0; j < c2; j++){
    //         cin>>ele;
    //         if (ele != 0){
    //             float new_ele[3];
    //             new_ele[0] = i;
    //             new_ele[1] = j;
    //             new_ele[2] = ele;
    //             SM2.push_back(new_ele);
    //         }}

    
    // SMArray<float> SMres = SM1*SM2;


    // SM1.display();

    return 0;


}