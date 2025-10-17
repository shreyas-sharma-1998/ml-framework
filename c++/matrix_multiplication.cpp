#include<iostream>
using namespace std;
void printMatrix(vector<vector<int>> &matrix) {
    for (int r = 0; r < matrix.size(); ++r) {
        for (int c = 0; c < matrix[0].size(); ++c) {
            cout<<matrix[r][c]<<",";
        }
        cout<<endl;
    }
}
vector<vector<int>> operator*(vector<vector<int>> &a, vector<vector<int>> &b) {
    int a_rows = a.size();
    int a_columns = a[0].size();
    int b_rows = b.size();
    int b_columns = b[0].size();
    if (a_columns != b_rows) return vector<vector<int>>();
    vector<vector<int>> res(a_rows, vector<int>(b_columns));
    for (int r = 0; r < a_rows; ++r) {
        for (int c = 0; c < b_columns; ++c) {
            int value = 0;
            for (int j = 0; j < a_columns; ++j) {
                value += (a[r][j]*b[j][c]);
            }
            res[r][c] = value;
        }
    }
    return res;
}
int main() {
    vector<vector<int>> a(3, vector<int>(4));
    vector<vector<int>> b(4, vector<int>(3));
    vector<vector<int>> c;
    int j = 1;
    for (int r = 0; r <= 2; r++) {
        for (int c = 0; c <= 3; c++) {
            a[r][c] = j++;
        }
    }
    for (int r = 0; r <= 3; r++) {
        for (int c = 0; c <= 2; c++) {
            b[r][c] = j++;
        }
    }
    c = a*b;
    cout<<"Left Matrix"<<endl;
    printMatrix(a);
    cout<<"Right Matrix"<<endl;
    printMatrix(b);
    cout<<"Result"<<endl;
    printMatrix(c);
    return 0;
}