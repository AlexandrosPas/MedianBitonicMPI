#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
#include <string.h>
#include <sys/time.h>
 
#define MAX_SIZE 1048576
#define N 67108864

int temp[MAX_SIZE];
int list[MAX_SIZE];
 
void  quicksort(int list[],int m,int n);
void  merge_even(int elements, int array[], int temp[]);
void  merge_odd(int elements, int array[], int temp[]);
int   log_2(int x);
 
 
int main(int argc, char *argv[]){
	
  int i, j, k ;
  int elements;
  int nproc, rank;
  int* array, *tmp, buf[MAX_SIZE];
  int mask, mask2, partner, dim, bl;
  int f_half, s_half ;
  float median1;
 
  struct timeval first, second, lapsed;
  struct timezone tzp;
  MPI_Request request, request_array;
  MPI_Status status;
    //Initializze the MPI
 
    //printf( "Processes start");
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);
 
  elements = (N/nproc);
 
  array=(int*) malloc(elements * sizeof(int)); 
    
	
  gettimeofday(&first, &tzp);
 
  MPI_Buffer_attach(buf, MAX_SIZE);
    if(rank == 0){
      gettimeofday(&first, &tzp);
      tmp = (int*) malloc(elements * sizeof(int));
 
	for(i=0; i<elements; i++){		//Arxikopoihsh tou pinaka ths process 1
	  array[i] = rand()%N;
	}
          for(j=1; j<nproc; j++){		//Arxikopoihsh twn ypoloipwn pinakwn
	    for(i=0; i<elements; i++){		
	      tmp[i] = rand()%N;
	    }
	      MPI_Send(tmp, elements,MPI_INT, j, 50, MPI_COMM_WORLD);
	  }
	  free(tmp);
	}
    else
    {
      MPI_Recv(array, elements, MPI_INT, 0, 50, MPI_COMM_WORLD, &status);
    }
 
  quicksort(array, 0, elements-1); 
 
  for (i = 2, mask = 2; i <= nproc; i *= 2, mask = mask << 1){
    dim = log_2(i);
    mask2 = 1 << (dim - 1);
 
      if ((rank & mask) == 0){
	for (j = 0; j < dim; j++){
          partner = rank ^ mask2;
	    if (rank < partner){
	      MPI_Sendrecv(array, elements, MPI_INT, partner, 100,
              temp, elements, MPI_INT, partner, 100, MPI_COMM_WORLD, &status);
 
	      merge_even(elements, array, temp);
            }
            else 
	    {
	      MPI_Sendrecv(array, elements, MPI_INT, partner, 100,
              temp,elements, MPI_INT, partner, 100, MPI_COMM_WORLD, &status);
              merge_odd(elements, array, temp);
            }
              mask2 = mask2 >> 1;
      	}
      }
      else 
      {
        for (j = 0; j < dim; j++){
	  partner = rank ^ mask2;
	    if (rank > partner){
	      MPI_Sendrecv(array, elements, MPI_INT, partner, 100,
	      temp, elements, MPI_INT, partner, 100, MPI_COMM_WORLD, &status);
 	      merge_even(elements, array, temp);
            }
            else 
	    {
	      MPI_Sendrecv(array, elements, MPI_INT, partner, 100,
              temp, elements, MPI_INT, partner, 100, MPI_COMM_WORLD, &status);
	      merge_odd(elements, array, temp);
       	    }
	    mask2 = mask2 >> 1;
      	    }
    	}
       }
 
 /**************************** PRINT elements *******************************
  if (rank == 0){
    tmp = (int*) malloc( elements * sizeof(int));
    printf("The sorted sequence is\n");
      for (i = 0; i < elements;  i++){			//ektupwsh ta prwta element stoixeia pou
	printf("%d ", array[i]);      			//briskontai sto diko mas upologisth (rank=0)
      }
      for (j = 1; j < nproc; j++){
        MPI_Recv(tmp, elements, MPI_INT, j, 50, MPI_COMM_WORLD, &status);
	  for (i = 0; i < elements; i++){ 		// Recv kai ektupwsh ta upoloipa sorted stoixeia
	    printf("%d ", tmp[i]);}         		//twn allwn procceses
    	  }
      }
      else 
      {
	MPI_Send(array, elements, MPI_INT, 0, 50, MPI_COMM_WORLD);
      }
  ****************************************************************************/
  

  MPI_Buffer_detach(&buf, &bl);
  if (nproc > 1){
    if (rank == (nproc/2)){
      f_half = array[0]  ;                           // h procces n/2 pereiexei ton aristero
        printf("left median is %d " ,f_half );        //diameso sto prwto stoixeio tou pinaka ths
        printf("\n");
	//MPI_Send(f_half,1,MPI_INT, 0, 55, MPI_COMM_WORLD);
  }
	
    if (rank == (nproc/2 - 1 )){
      s_half = array[elements-1] ;               // h process n/2 -1 periexei ton dexio
        printf("right median is %d " ,s_half );	 //diameso sto teleutaio stoixeio tou pinaka ths
        printf("\n");
	//MPI_Send(s_half,1,MPI_INT, 0, 56, MPI_COMM_WORLD);
    }
  }
  else
  {
   if (rank == 0){
     median1 = (array[elements/2] + array[(elements/2) -1])/2;
     printf("median is %.2f ", median1);	
     printf("\n");
   }
  } 
  
  MPI_Finalize();
  
  if(rank==0){	                           // emfanish xronikhs diaforas metaxu twn duo klhsewn                                         
  gettimeofday(&second, &tzp);            //ths gettimeofday
  
  if(first.tv_usec>second.tv_usec){
    second.tv_usec += 1000000;
    second.tv_sec--;
  }
  
  lapsed.tv_usec = second.tv_usec - first.tv_usec;
  lapsed.tv_sec = second.tv_sec - first.tv_sec;

  printf("Time elapsed: %d, %d s\n", lapsed.tv_sec, lapsed.tv_usec); }
  
  return 0;
  }
 
int log_2(int x){
  int i = 0;
  while (x > 1){
    x = x / 2;
    i++;
  }
  return i;
}
 
void merge_even(int elements, int array[], int temp[]){

  int i;
  int j = 0, k = 0;
 
  for (i = 0; i < elements; i++)
    if (array[j] <= temp[k]){
      list[i] = array[j];
      j++;
    } 
    else 
    {
    list[i] = temp[k];
    k++;
    }
 
  for (i = 0; i < elements; i++)
    array[i] = list[i];
}
 
void merge_odd(int elements, int array[], int temp[]){

  int i;
  int j = elements - 1;
  int k = elements - 1;
 
  for (i = elements - 1; i >= 0; i--)
    if (array[j] >= temp[k]){
      list[i] = array[j];
      j--;
    }
    else 
    {
      list[i] = temp[k];
      k--;
    }
  for (i = 0; i < elements; i++)
    array[i] = list[i];
}

void swap(int *x,int *y){

  int temp;
  temp = *x;
  *x = *y;
  *y = temp;
}

int choose_pivot(int i,int j){
  return((i+j) /2);
}

void quicksort(int list[],int m,int n){

  int key,i,j,k;
  if( m < n){
    k = choose_pivot(m,n);
    swap(&list[m],&list[k]);
    key = list[m];
    i = m+1;
    j = n;
      while(i <= j)
      {
         while((i <= n) && (list[i] <= key))
           i++;
         while((j >= m) && (list[j] > key))
           j--;
         if( i < j)
           swap(&list[i],&list[j]);
      }
	  // swap two elements
    swap(&list[m],&list[j]);
	  // recursively sort the lesser list
    quicksort(list,m,j-1);
    quicksort(list,j+1,n);
   }
}
