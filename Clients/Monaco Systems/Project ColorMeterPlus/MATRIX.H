/*global*/  void Invert(double *,int );
/*global*/  void Trans(double *,int ,int ,double *);
/*global*/  int MatMult(double *,int ,int ,double *,int ,int ,double *);
/*global*/  int A_solve(double *c_mat,int c_rows,int c_cols,double *b_mat,int b_rows,int b_cols, double *a_mat);

typedef struct {
short rows;
short cols;
double *dat;
}Mat_Struct;
