#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

int** energy_matrix(int ***rgb, int H, int W, int C){
    /* Fuction to calculate  gradient matrix
       from the given rgb matrix
     Args: 
        rgb(***string): RGB matrix of the image
        H(int): Height of the RGB image
        W(int): Width of RGB image 
        C(int): Colours of RGB image   
     Returns:
        energy(**string): Gradient matrix
     */
    int **energy;
    energy = new int *[H];
    for(int i = 0; i < H; ++i) {

        energy[i] = new int[W];

        for(int j = 0; j < W; ++j) {

            int Rx,Gx,Bx,Ry,Gy,By,dx,dy;
            int xl=j-1<0?W-1:j-1;
            int xr=j+1>W-1?0:j+1;
            int yu=i-1<0?H-1:i-1;
            int yd=i+1>H-1?0:i+1;


            Rx = rgb[i][xl][0] - rgb[i][xr][0];
            Gx = rgb[i][xl][1] - rgb[i][xr][1];
            Bx = rgb[i][xl][2] - rgb[i][xr][2];

            
            Ry = rgb[yu][j][0] - rgb[yd][j][0];
            Gy = rgb[yu][j][1] - rgb[yd][j][1];
            By = rgb[yu][j][2] - rgb[yd][j][2];

            dx = Rx*Rx + Gx*Gx + Bx*Bx;
            dy = Ry*Ry + Gy*Gy + By*By;
            
            energy[i][j] = (int)sqrt(dx+dy);

        }
    }

    return energy;
}

int** get_cost_vertical(int **gradient_matrix, int H, int W){
    /* Fuction to calculate  cost matrix vertically
       from the given gradient matrix
     Args: 
        gradient_matrix(**string): Gradient matrix of the image
        H(int): Height of the image
        W(int): Width of image 
     Returns:
        cost_matrix(**string): Vertical cost matrix
     */
    int** cost_matrix = new int *[H];
    //initialising the first row with gradient value
    cost_matrix[0] = gradient_matrix[0];
    for(int i = 1; i < H; ++i) {
        cost_matrix[i] = new int[W];
        cost_matrix[i][0] = \
                min(cost_matrix[i-1][0], cost_matrix[i-1][1]) +\
                                        gradient_matrix[i][0];
        cost_matrix[i][W-1] =\
                    min(cost_matrix[i-1][W-2], cost_matrix[i-1][W-1]) +\
                                               gradient_matrix[i][W-1];
        for(int j = 1; j < W-1; ++j) {
            int mini = min(cost_matrix[i-1][j-1],cost_matrix[i-1][j]);
            mini = min(mini,cost_matrix[i-1][j+1]);
            cost_matrix[i][j] = gradient_matrix[i][j] + mini;
        }
    }
    return cost_matrix;
}

int** get_cost_horizontal(int **gradient_matrix, int H, int W){
    /* Fuction to calculate  cost matrix horizontally
       from the given gradient matrix
     Args: 
        gradient_matrix(**string): Gradient matrix of the image
        H(int): Height of the image
        W(int): Width of image 
     Returns:
        cost_matrix(**string): Horozontal cost matrix
     */

    int** cost_matrix = new int *[H];
    //initialising the first column with gradient value
    for(int i = 0; i < H; ++i){
        cost_matrix[i] = new int[W];
        cost_matrix[i][0]= gradient_matrix[i][0];
    }
    for(int j=1; j<W;++j){
        cost_matrix[0][j] = min(cost_matrix[0][j-1],cost_matrix[1][j-1]) + gradient_matrix[0][j];
        cost_matrix[H-1][j] = min(cost_matrix[H-2][j-1],cost_matrix[H-1][j-1]) + gradient_matrix[H-1][j];
        for(int i=1;i<H-1;++i){
            int mini = min(cost_matrix[i-1][j-1],cost_matrix[i][j-1]);
            mini = min(mini,cost_matrix[i+1][j-1]);
            cost_matrix[i][j] = gradient_matrix[i][j] + mini;
        }
    }
    return cost_matrix;
}

void shift_pixel_horizontal(int*** arr, int row, int index, int len){
     /* Fuction to shift pixel from the given row and index horizontally 
     Args: 
        arr(***string): Image array 
        row(int): Row of image that needs to be shifted
        index(int): Index from which shift needs to be done
        len(int): Length of the row
     */
        for(int i=index;i<len-1;i++){
            arr[row][i][0]=arr[row][i+1][0];
            arr[row][i][1]=arr[row][i+1][1];
            arr[row][i][2]=arr[row][i+1][2];
        }

}

void shift_pixel_vertical(int*** arr, int col, int index, int len){
     /* Fuction to shift pixel from the given column and index vertically 
     Args: 
        arr(***string): Image array 
        col(int): Column of image that needs to be shifted
        index(int): Index from which shift needs to be done
        len(int): Length of the column
     */
        for(int i=index;i<len-1;i++){
            arr[i][col][0]=arr[i+1][col][0];
            arr[i][col][1]=arr[i+1][col][1];
            arr[i][col][2]=arr[i+1][col][2];
        }

}

void vertical_seam(int ***rgb, int H, int W, int C){
    /* Fuction to do one pixel vertical seam of the given RGB image 
     Args: 
        rgb(***string): RGB image matrix
        H(int): Height of the RGB image
        W(int): Width of RGB image 
        C(int): Colours of RGB image   
     */

    int **gradient_matrix = energy_matrix(rgb, H, W, C);

    int **cost_matrix = get_cost_vertical(gradient_matrix,H,W);
    
    int min_index=0,min_val=cost_matrix[H-1][0];
    for(int j = 1; j < W; ++j){
        if(cost_matrix[H-1][j]<min_val){
            min_val=cost_matrix[H-1][j];
            min_index=j;
            }
        }
    shift_pixel_horizontal(rgb,H-1,min_index,W);

    

    for(int i = H-1; i > 0; --i) {
        int next_min_index,xl,xr;
        

        //checking for the edge cases
        xl = min_index==0?0:min_index-1;
        xr = min_index==W-1?W-1:min_index+1;

        next_min_index=cost_matrix[i-1][xl]<cost_matrix[i-1][min_index]?xl:min_index;
        next_min_index=cost_matrix[i-1][next_min_index]<cost_matrix[i-1][xr]?next_min_index:xr;

        min_index=next_min_index;
        shift_pixel_horizontal(rgb,i-1,min_index,W);

    }

}

void horizontal_seam(int ***rgb, int H, int W, int C){
    /* Fuction to do one pixel horizontal seam of the given RGB image 
     Args: 
        rgb(***string): RGB image matrix
        H(int): Height of the RGB image
        W(int): Width of RGB image 
        C(int): Colours of RGB image   
     */

    int **gradient_matrix = energy_matrix(rgb, H, W, C);


    int **cost_matrix = get_cost_horizontal(gradient_matrix,H,W);

    int min_index=0,min_val=cost_matrix[0][W-1];
    for(int i = 1; i < H; ++i){
        if(cost_matrix[i][W-1]<min_val){
            min_val=cost_matrix[i][W-1];
            min_index=i;
            }
        }
    shift_pixel_vertical(rgb,W-1,min_index,H);

    for(int j = W-1; j > 0; --j) {
        int next_min_index,yu,yd;
        

        //checking for the edge cases
        yu = min_index==0?0:min_index-1;
        yd = min_index==H-1?H-1:min_index+1;
       

        next_min_index=cost_matrix[yu][j-1]<cost_matrix[min_index][j-1]?yu:min_index;
        next_min_index=cost_matrix[next_min_index][j-1]<cost_matrix[yd][j-1]?next_min_index:yd;

        min_index=next_min_index;
        shift_pixel_vertical(rgb,j-1,min_index,H);

    }

}

void solve(int ***rgb, int H, int W, int C, int H_, int W_, int C_) {
    /* Fuction to do vertical seam and horizontal seam of the given RGB image 
     Args: 
        rgb(***string): RGB image matrix
        H(int): Height of the RGB image
        W(int): Width of RGB image 
        C(int): Colours of RGB image   
        H_(int): Height of the desired RGB image
        W_(int): Width of the desired RGB image 
        C_(int): Colours of the desired RGB image   
     */

    // We've provided you the driver.py and main.cpp for your convinience
    // Please go through them and understand the file handling and input/output format
    // Feel free to experiment on your own

    // can access the array using rgb[i][j][k] like a regular 3D array

    // Write your code here


    

    //Here we are doing one vertical and one horizontal seam alternatively
    //for better results 
    int w_diff=W-W_, h_diff=H-H_;

    int i,min_diff=min(w_diff,h_diff);

    for(i=0;i<min_diff;i++){
        vertical_seam(rgb,H-i,W-i,C);
        horizontal_seam(rgb,H-i,W-i-1,C);
    }

    //completing the remaining horizontal or vertical seam
    H=H-min_diff;
    W=W-min_diff;
    for(i=0;i<w_diff-min_diff;i++){
        vertical_seam(rgb,H,W-i,C);
    }
    W=W-i-min_diff;
    for(i=0;i<h_diff-min_diff;i++){
        horizontal_seam(rgb,H-i,W,C);
    }

}

int main() {
    string ip_dir = "./data/input/";
    string ip_file = "rgb_in.txt";
    ifstream fin(ip_dir + ip_file);

    int H, W, C;
    fin >> H >> W >> C;

    int ***rgb;
    rgb = new int **[H];
    for(int i = 0; i < H; ++i) {
        rgb[i] = new int *[W];
        for(int j = 0; j < W; ++j) {
            rgb[i][j] = new int[C];
            for(int k = 0; k < C; ++k)
                fin >> rgb[i][j][k];
        }
    }
    fin.close();

    int H_, W_, C_;
    cout << "Enter new value for H (must be less than " << H << "): ";
    cin >> H_;
    cout << "Enter new value for W (must be less than " << W << "): ";
    cin >> W_;
    cout << '\n';
    C_ = C;

    solve(rgb, H, W, C, H_, W_, C_);

    string op_dir = "./data/output/";
    string op_file = "rgb_out.txt";
    ofstream fout(op_dir + op_file);
    
    fout << H_ << " " << W_ << " " << C_ << endl;
    for(int i = 0; i < H_; ++i) {
        for(int j = 0; j < W_; ++j) {
            for(int k = 0; k < C_; ++k) {
                fout << rgb[i][j][k] << " ";
            }
        }
        fout << '\n';
    }
    fout.close();

    return 0;
}