#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <emmintrin.h> //let us use msse2

/* gcc -O2 matmul-sse.c -o matmul-sse -DN=... */
/* gcc -O2 matmul-sse.c -S -DN=... */

/* Function to measure duration of execution */
void get_walltime(double *wct);


int main(){
	__m128 *vfa, *vfb, *vfc, *nsum;
	float *pa, *pb, *pc, *psum;

	/* Allocation of matrices */
	int a = posix_memalign((void **) &pa, 16, N * N * sizeof(float));
	if(a != 0) {			//prevents allocation errors
		exit(0);
	}

	int b = posix_memalign((void **) &pb, 16, N * N * sizeof(float));
	if(b != 0) {			//prevents allocation errors
		free(pa);
		exit(0);
	}

	int c = posix_memalign((void **) &pc, 16, N * N * sizeof(float));
	if(c != 0) {			//prevents allocation errors
		free(pa);
		free(pb);
		exit(0);
	}

	int sum = posix_memalign((void **) &psum, 16, 4 * sizeof(float));
	if(sum != 0) {			//prevents allocation errors
		free(pa);
		free(pb);
		free(pc);
		exit(0);
	}

	/* Definition and initialization of variables */
	double ts = 0;		//parameter for get_walltime() function to start measuring the duration of execution
	double te = 0;		//parameter for get_walltime() function to end measuring the duration of execution
	double mflop = 0;
	
	/* Note: we need these to avoid "Segmentation fault" and "Aborted (core damped)" errors when calculating the sum of the quadraple at the 4th for loop */
	float *fc = pc;
	float *fsum = psum;

	/* Initialization of matrices */
	for(int i=0;i<N*N;i++){
		pa[i] = 2.0;
		pb[i] = 3.0;
		pc[i] = 0.0;
	}

	for(int i=0;i<4;i++){
		psum[i] = 0.0;
	}

	/* Create __m128 pointers for matrices */
	vfa = (__m128 *) pa;		//__m128 pointer for matrix a
	vfb = (__m128 *) pb;		//__m128 pointer for matrix b
	vfc = (__m128 *) pc;		//__m128 pointer for matrix c
	nsum = (__m128 *) psum;		//__m128 pointer for matrix sum

	/* Call function get_walltime() to start measuring */
	get_walltime(&ts);

	/* Load */
	for(int k=0;k<N;k++){
		vfb = (__m128 *) pb;		//column
		for(int j=0;j<N;j++){
			*nsum = _mm_set_ps(0,0,0,0);

			vfa = (__m128 *) (pa + k * N);		//row
			
			for(int i=0;i<N;i+=4){
				*nsum = _mm_add_ps(*nsum, _mm_mul_ps(*vfa, *vfb));	//inner product (row * column)
				vfa++;
				vfb++;

			} //end of 3rd for loop
			
			/* Calculate the sum of the quadraple */
			fsum = psum;
			for(int i=0;i<4;i++){
				*fc = *fc + *fsum;
				fsum++;

			} //end of 4th for loop
			fc++;

		} //end of 2nd for loop

	} //end of 1st for loop

	/* Call function get_walltime() to end measuring */
	get_walltime(&te);

	/* Result check */
	for(int i=0;i<N*N;i++){
		if(pc[i] != 3.0 * 2.0 * N){
			printf("error\n");
			break;
		} //end of if
	} //end of for loop

	/* Calculation of Mflops/s */
	mflop = (N*N*N)/((te-ts) * 1e6);
	printf("Duration: %f,  MFLOPS/s: %f\n", te-ts, mflop);
	
	/* Deallocation of matrices */
	free(pa);
	free(pb);
	free(pc);

	return 0;

} //end of main

/* Function to measure duration of execution */
void get_walltime(double *wct) {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	*wct = (double) (tp.tv_sec + tp.tv_usec/1000000.0);

} // end of get_walltime
