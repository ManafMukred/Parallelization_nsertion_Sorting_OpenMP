//sort-simple.c
#include <time.h> // for CPU time
#include <sys/time.h> //for gettimeofday
#include <stdio.h>
#include <omp.h>
#define max_len 100000
#define LENGTH 40

main(){
int i=1,len,j,k,Nme,me,nthreads,max_loc;
double b[max_len+1],c[max_len+1],bnew,cnew,time,c_me[max_len+1],b_me[max_len+1],low, high,interval;
char name[LENGTH]="100k.txt",line[LENGTH];
FILE *fp;
clock_t cpu0,cpu1,cpu2,cpu3; // clock_t defined in <time.h> and <sys/types.h> as int
struct timeval time0, time1,time2,time3; // for wall clock in s and us
double  dtime12,dtime03; // for wall clock in s (real number)

/////////////////////start zero timer ///////////////////////
cpu0 = clock();    // assign initial CPU time (IN CPU CLOCKS)
gettimeofday(&time0, NULL); // returns structure with time in s and us (microseconds)
/////////////////////start zero timer ///////////////////////


//printf("Input filename\n"); scanf("%s",name);
fp=fopen(name,"r");
while(1){ //1 serves as true, i.e. condition which is always true
  if(fgets(line, LENGTH,fp)==NULL)break; // finish reading when empty line is read
  if(sscanf(line, "%lf %lf",&b[i],&c[i])==-1) break; // finish reading after error
  i++;
}
len=i-1;
fclose(fp);
///////find max and min of the elements /////////
    double max = b[1], min = b[1];
    for(i=1; i<=len; i++)
    {
        /* If current element is greater than max */
        if(b[i] > max)
        { max = b[i];max_loc =i; }
        /* If current element is smaller than min */
        if(b[i] < min)
        { min = b[i]; }
    }
    printf("Number of items to sort: %i\n",len);
    printf("Maximum element = %lf\n", max);
    printf("Minimum element = %lf\n", min);
///////find max and min of the elements /////////

/////////////////////start first timer ///////////////////////
cpu1 = clock();    // assign initial CPU time (IN CPU CLOCKS)
gettimeofday(&time1, NULL); // returns structure with time in s and us (microseconds)
/////////////////////start first timer ///////////////////////


#pragma omp parallel private(nthreads,i,me,low,high,Nme, b_me, c_me,interval)

{      ////////////////////////////////////////////////// start parallel region //////////////////////////////////////////

me = omp_get_thread_num();   /* Obtain thread number */
nthreads = omp_get_num_threads();
printf("Number of threads = %d\n", nthreads);

interval = (max - min) / nthreads; //divide the difference between max and min into nthreads sections
Nme=0;
low =  ( me* interval) + min ;
high=  ( (me + 1)* interval) + min;
// printf("intrval %d = %lf\n",me, interval);


// assign maximum value to the last thread because it won't be counted as part of the search loop
if (me == (nthreads-1) ) {
b_me[Nme]=b[max_loc];
c_me[Nme]=c[max_loc];
Nme++;
}

// the following for loop reads all elements except the maximum, so we assign it directly to b_me[0] of the core 0
for (i=1;i<=len;i++) {
  if((b[i]>=low) & (b[i]<high)) {
    b_me[Nme]=b[i]; 
    c_me[Nme]=c[i]; 
    Nme++;
                               } 

                    }


// Start sorting 
for(j=1;j<Nme;j++){ // start sorting with the second item
  bnew=b_me[j];cnew=c_me[j];
  for(i=0;i<j;i++){
    // if(bnew==b[i]) printf("Equal numbers %lf\n",bnew);
    if((bnew<b_me[i]) | ((bnew==b_me[i])&(cnew<c_me[i])))
	{
	  for(k=j;k>i;k--) { b_me[k]=b_me[k-1]; c_me[k]=c_me[k-1]; } //shift larger numbers
	  b_me[i]=bnew; c_me[i]=cnew; //insert number j on ith position
	  break; //no need to check larger items which are already sorted !
	} // else do nothing - j is the largest number
  }   // continue checking other numbers if larger or not
}



///////////////////begin second timer ///////////////////////
cpu2 = clock();    // assign CPU time (IN CPU CLOCKS)
gettimeofday(&time2, NULL);
///////////////////begin second timer ///////////////////////

// calculate sorting time ==> dtime12
dtime12 = ((time2.tv_sec  - time1.tv_sec)+(time2.tv_usec - time1.tv_usec)/1e6);
// calculate sorting time ==> dtime12

///////// writing to a file in order ////////
#pragma omp for ordered
for (int m=0 ; m < nthreads;m++) { // ordered begining   

#pragma omp ordered 
{ 
if(me==0) fp=fopen("parallel_sorted.txt","w"); 
else {fp=fopen("parallel_sorted.txt","a");}

   for (i=0;i<Nme;i++) {
          fprintf(fp,"%lf %lf\n",b_me[i],c_me[i]);
                        }
          
fclose(fp);
}
                              } // ordered begining 

///////// writing to a file in order ////////


}      ////////////////////////////////////////////////// end parallel region //////////////////////////////////////////


/////////////////////begin second timer ///////////////////////
cpu3 = clock();    // assign initial CPU time (IN CPU CLOCKS)
gettimeofday(&time3, NULL); // returns structure with time in s and us (microseconds)
/////////////////////begin third timer ///////////////////////

// calculate sorting time ==> dtime12
dtime03 = ((time3.tv_sec  - time0.tv_sec)+(time3.tv_usec - time0.tv_usec)/1e6);
// calculate sorting time ==> dtime12

// print timings
printf("Elapsed wall time sorting         CPU time\n");
printf("Duration 12 %f               %f\n", dtime12,(float) (cpu2-cpu1)/CLOCKS_PER_SEC);
printf("Elapsed wall time complete        CPU time\n");
printf("Duration 03 %f                %f\n", dtime03,(float) (cpu3-cpu0)/CLOCKS_PER_SEC);
// print timings

}

