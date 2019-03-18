#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/* gcc -O2 matmul-normal.c -o matmul-normal -DN=... -DR=... */
/* gcc -O2 matmul-normal.c -S -DN=... -DR=... */

/* Function to measure duration of execution */
void get_walltime(double *wct);


int main(){

	/* Allocation of matrices */
	float* a = (float*) malloc(N * N * sizeof(float));
	if(a == NULL) {			//prevents allocation errors
		exit(0);
	}

	float* b = (float*) malloc(N * N * sizeof(float));
	if(b == NULL) {			//prevents allocation errors
		free(a);
		exit(0);
	}

	float* c = (float*) malloc(N * N * sizeof(float));
	if(c == NULL) {			//prevents allocation errors
		free(a);
		free(b);
		exit(0);
	}

	/* Definition and initialization of variables */
	double ts = 0;		//parameter for get_walltime() function to start measuring the duration of execution
	double te = 0;		//parameter for get_walltime() function to end measuring the duration of execution
	double mflop = 0;
	float sum;		//sum of inner product (row * column)

	/* Definition of pointers for each matrix to access data */
	float *pa, *pb, *pc;

	/* Initialization of matrices */
	for(int i=0;i<N*N;i++){
		a[i] = 2.0;
		b[i] = 3.0;
		c[i] = 0.0;
	}
	
	/* Call function get_walltime() to start measuring */
	get_walltime(&ts);

	/* Load */
	pa = a;			//pointer to first element of matrix a
	pb = b;			//pointer to first element of matrix b
	pc = c;			//pointer to first element of matrix c
	for(int k=0;k<N;k++){
		pb = b;			//column
		for(int j=0;j<N;j++){
			pa = a + k * N;	//row
			sum = 0.0;
			for(int i=0;i<N;i++){
				sum += (*pa) * (*pb);	//inner product (row * column)
				pa++;			//next element of matrix a
				pb++;			//next element of matrix b

			} //end of 3rd for loop

			*pc = sum;
			pc++;				//next element of matrix c

		} //end of 2nd for loop
	} //end of 1st for loop

	/* Call function get_walltime() to end measuring */
	get_walltime(&te);

	/* Result check */
	for(int i=0;i<N*N;i++){
		if(c[i] != 3.0 * 2.0 * N){
			printf("%f\n", c[i]);
			printf("error\n");
			break;

		} //end of if
	} //end of for loop

	/* Calculation of Mflops/s */
	mflop = (N*N*N)/((te-ts) * 1e6);
	printf("Duration: %f,  MFLOPS/s: %f\n", te-ts, mflop);
	
	/* Deallocation of matrices */
	free(a);
	free(b);
	free(c);

	return 0;

} //end of main

/* Function to measure duration of execution */
void get_walltime(double *wct) {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	*wct = (double) (tp.tv_sec + tp.tv_usec/1000000.0);

} // end of get_walltime
