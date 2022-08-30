#include <iostream>
#include <string>

using namespace std;

bool BigIsEqual(string a, string b){
    /* Fuction to check if the given two string
     are equal or not
     Args: 
        a(string): string 1
        b(string): string 2   
     Returns:
        bool: result of a==b
     */
    int l1=a.length(),l2=b.length();
    if (l1!=l2) return false;
    int p1=0,p2=0;
    while(p1<l1){
        if((int)(a[p1++]-'0')!=(int)(b[p2++]-'0')) return false;
    }
    return true;
}

bool BigGreaterThan(string a,string b){
     /* Fuction to check if first string is greater
     than the second string
     Args: 
        a(string): string 1
        b(string): string 2   
     Returns:
        bool: result of a>b
     */
    int l1=a.length(),l2=b.length();
    if (l1>l2) return true;
    if (l2>l1) return false;
    int p1=0,p2=0;
    while(p1<l1){
        if((int)(a[p1]-'0')<(int)(b[p2]-'0')) return false;
        else if((int)(a[p1]-'0')>(int)(b[p2]-'0')) return true;
        p1++;
        p2++;
    }
    if(!BigIsEqual(a,b)) return true;
    return false;


}

bool BigLessThan(string a,string b){
    /* Fuction to check if first string is lesser
     than the second string
     Args: 
        a(string): string 1
        b(string): string 2   
     Returns:
        bool: result of a<b
     */
    int l1=a.length(),l2=b.length();
    if (l2>l1) return true;
    if (l1>l2) return false;
    int p1=0,p2=0;
    while(p1<l1){
        if((int)(a[p1]-'0')>(int)(b[p2]-'0')) return false;
        else if((int)(a[p1]-'0')<(int)(b[p2]-'0')) return true;
        p1++;
        p2++;
        
    }
    if(!BigIsEqual(a,b)) return true;
    return false;


}

string BigAdd(string a, string b){
    /* Fuction to add two strings 
     Args: 
        a(string): string 1
        b(string): string 2   
     Returns:
        sum(string): sum of the given strings 
     */
    int l1=a.length()-1,l2=b.length()-1;
    if(l2>l1) {
        swap(a,b);
        swap(l1,l2);
    }
    int s,c=0;
    string  sum = "";
    while(l1>=0 && l2>=0){
        s=(int)(a[l1--]-'0')+(int)(b[l2--]-'0')+c;
        c=s/10;
        s%=10;
        sum=(char)('0'+s)+sum;
    }
    
    while(l1>=0){
        s=(int)(a[l1--]-'0') + c;
        c=s/10;
        s%=10;
        sum=(char)('0'+s)+sum;
    }
    if(c>0) sum=(char)('0'+c)+sum;

    return sum;
}

string BigSub(string a, string b){
    /* Fuction to substract two strings 
       Expected a is greater than or equal to b
     Args: 
        a(string): string 1
        b(string): string 2   
     Returns:
        diff(string): difference of the given strings 
     */
    int l1=a.length()-1,l2=b.length()-1;
    string diff="";
    int d,c=0;
    while(l1>=0&&l2>=0){
        d=(int)(a[l1--]-'0')-(int)(b[l2--]-'0')-c;
        if (d<0) {
            d+=10;
            c=1;
        }
        else c=0;
        diff=(char)('0'+d)+diff;
    }
    while(l1>=0){
        d=(int)(a[l1--]-'0')-c;
        if (d<0) {
            d+=10;
            c=1;
        }
        else c=0;
        diff=(char)('0'+d)+diff;
    }

    int nz=0;
    while(diff[nz]=='0') nz++;
    if(nz!=diff.length()){
        diff=diff.substr(nz);
    }
    else diff="0";

    return diff;

}

string BigMul(string a, string b){
    /* Fuction to multiply two strings 
     Args: 
        a(string): string 1
        b(string): string 2   
     Returns:
        prod1(string): product of the given strings 
     */
    int l1=a.length()-1,l2=b.length()-1;
    if(l2>l1) {
        swap(a,b);
        swap(l1,l2);
    }
    int p, zeros=0;
    string prod1 = "0";
    while(l2>=0){
        int mul,p1=l1,c=0;
        mul=(int)(b[l2--]-'0');
        string prod2 = "";
        while(p1>=0){
            p=(int)(a[p1--]-'0')*mul+c;
            c=p/10;
            p%=10;
            prod2=(char)('0'+p) + prod2;
        }
        if(c>0) prod2=(char)('0'+c) + prod2;

        //Adding zeros at the end of the product before adding them to result
        string stuff(zeros++, '0');
        prod2+=stuff;
        prod1=BigAdd(prod1,prod2);
    }

    return prod1;

}

string BigDiv(string a, string b){
    /* Fuction to divide two strings 
     Args: 
        a(string): string 1
        b(string): string 2   
     Returns:
        quo(string): quotient of the given strings 
     */
    string rem, quo= "";
    //here we are splitting the number then using naive approach for division
    while (!BigLessThan(a,b)){
        int l1=a.length()-1,l2=b.length()-1;
        string l1_begin = a.substr(0,l2+1);
        string l1_cont = a.substr(l2+1);
        if(BigLessThan(l1_begin,b)) {
            l1_begin = a.substr(0,l2+2);
            l1_cont = a.substr(l2+2);
            }
    
        int i=0;
        while(!BigLessThan(l1_begin,b)){
            l1_begin=BigSub(l1_begin,b);
            i++;
            if (i>9) i=0;
        }
        rem=l1_begin;
        a=rem+l1_cont;
        quo+=(char)('0'+i);
    }
    return quo;
}

string BigMod(string a, string b){
    /* Fuction to find modulus two strings 
     Args: 
        a(string): string 1
        b(string): string 2   
     Returns:
        a(string): a%b string 
     */
    //here we are splitting the number then using naive approach for division
    while (!BigLessThan(a,b)){
        int l1=a.length()-1,l2=b.length()-1;
        string l1_begin = a.substr(0,l2+1);
        string l1_cont = a.substr(l2+1);
        if(BigLessThan(l1_begin,b)) {
            l1_begin = a.substr(0,l2+2);
            l1_cont = a.substr(l2+2);
        }
    
        while(!BigLessThan(l1_begin,b)){
            l1_begin=BigSub(l1_begin,b);
        }
        string rem=l1_begin;
        a=rem+l1_cont;
    }
    return a;
}

string BigExp(string a, long b){
    /* Fuction to find exponent of a string
     Args: 
        a(string): string number 
        b(long): power of exponent   
     Returns:
        ans(string): a^b string 
     */
    string ans = "1";
    while(b>0){
        if(b%2) ans = BigMul(a,ans);
        a=BigMul(a,a);
        b/=2;
    }
    return ans;

}

string BigGCD(string a, string b){
    /* Fuction to find GCD of two strings
     Args: 
        a(string): string 1
        b(string): string 2
     Returns:
        string: GCD(a,b)
     */
    if(b=="0") return a;
    return BigGCD(b,BigMod(a,b));
}

string BigFact(string a){
    /* Fuction to find factorial of a string
     Args: 
        a(string): string number  
     Returns:
        fact(string): a! 
     */
    string fact=a,prev=a;
    bool start=true;

    if(a=="0") {
        fact="1";
        start=false;
        };
    while(start){
        if(prev=="1") break;
        prev=BigSub(prev,"1");
        // cout<<prev<<'\n';
        fact=BigMul(fact,prev);
    }
    return fact;
}

string evaluate_expression(string expr,int opr){
    /* Fuction to check the syntax of the given expression and returns the 
       result of the expression based on the operation given
     Args: 
        expr(string): Expression that needs to be evaluated
        opr(int): Number representing the operation that needs to be performed
     Returns:
        res: Result of the evaluated expression 
     */
    int len = expr.length();
    string valid_opr;
    if(opr==1) valid_opr={'+','-','x'};
    else valid_opr={' '};

    string operands[len];
    int prev=0,j=0;
    for(int i=0; i<len; i++){
        if(!isdigit(expr[i])){
            if (valid_opr.find(expr[i])<valid_opr.length())
            {
                string sub;
                sub=expr.substr(prev,i-prev);
                prev=i+1;
                operands[j++]=sub;
                operands[j++]=expr[i];
            }
            else {
                cout<<"Please enter a valid input\n";
                exit(0);
            }
        }
    }
    string sub=expr.substr(prev);
    operands[j]=sub;

    // Evaluating the expression 
    string res,temp_val,new_exp[j];
    int k=0;

    switch(opr){
        case 1:
            for(int i=0;i<=j;i++){
                if(operands[i]=="x"){
                    temp_val=BigMul(operands[i-1],operands[i+1]);
                    operands[i-1]="NaN";
                    operands[i]="NaN";
                    operands[i+1]=temp_val;
                }
            }
            for(int i=0;i<=j;i++){
                if(operands[i]!="NaN")
                    new_exp[k++]=operands[i];
            }
            res=new_exp[0];
            for(int i=0;i<=j;i++){
                if (new_exp[i]=="+") res=BigAdd(res,new_exp[i+1]);
                else if(new_exp[i]=="-") res=BigSub(res,new_exp[i+1]);
            }
            break;
        case 2:
            res=BigExp(operands[0],stol(operands[2]));
            break;
        case 3:
            res=BigGCD(operands[0],operands[2]);
            break;
        case 4:
            res=BigFact(operands[0]);
            break;

        default: 
            cout<<"Please enter a valid input\n";
            exit(0);
    }
    return res;

}

int main(){

    //Gettting the input operation and expression
    int opr;
    string expr1,expr2,res;

    cin>>opr;
    switch(opr){
        case 1:
            cin>>expr1;
            break;
        case 2:
            cin>>expr1>>expr2;
            expr1=expr1+' '+expr2;
            break;
        case 3:
            cin>>expr1>>expr2;
            expr1=expr1+' '+expr2;
            break;
        case 4:
            cin>>expr1;
            break;

        default: 
            cout<<"Please enter a valid input\n";
            exit(0);
    }
    res=evaluate_expression(expr1,opr);
    cout<<'\n'<<res;

    return 0;
}
