#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#ifdef DEBUG
const int RMAX = 100;
#else
const int RMAX = 10000000;
#endif

int thread_count;

void Usage(char* prog_name);
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p);
void Generate_list(int a[], int n);
void Print_list(int a[], int n, char* title);
void Read_list(int a[], int n);
void Read_Fichero(int a[], int n);
void Odd_even_First(int a[], int n);
void Odd_even_Second(int a[], int n);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   int n;
   char g_i;
   int* a;
   double t0, t1 ;
   double t2, t3;

   Get_args(argc, argv, &n, &g_i);
   a = malloc(n*sizeof(int));
   if (g_i == 'g') {
      Read_Fichero(a, n);
#     ifdef DEBUG
      Print_list(a, n, "Before sort");
#     endif
   } else {
      Read_list(a, n);
   }

   t0 = omp_get_wtime();
   Odd_even_First(a, n);
   t1 = omp_get_wtime();

   t2 = omp_get_wtime();
   Odd_even_Second(a, n);
   t3 = omp_get_wtime();

#  ifdef DEBUG
   Print_list(a, n, "After sort");
#  endif
   
   printf("First %5lf seconds\n", t1 - t0);
   printf("Second %5lf seconds\n", t3 - t2);
   //free(a);
   return 0;
}  /* main */

/*-----------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Summary of how to run program
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage:   %s <thread count> <n> <g|i>\n", prog_name);
   fprintf(stderr, "   n:   number of elements in list\n");
   fprintf(stderr, "  'g':  generate list using a random number generator\n");
   fprintf(stderr, "  'i':  user input list\n");
}  /* Usage */


/*-----------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get and check command line arguments
 * In args:   argc, argv
 * Out args:  n_p, g_i_p
 */
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p) {
   if (argc != 4 ) {
      Usage(argv[0]);
      exit(0);
   }
   thread_count = strtol(argv[1], NULL, 10);
   *n_p = strtol(argv[2], NULL, 10);
   *g_i_p = argv[3][0];

   if (*n_p <= 0 || (*g_i_p != 'g' && *g_i_p != 'i') ) {
      Usage(argv[0]);
      exit(0);
   }
}  /* Get_args */

void Read_Fichero(int a[], int n) {
   int num;
   int i = 0;
    FILE *f;
    f = fopen("datos.txt", "r");
    while( fscanf(f, "%i", &num) == 1){
      a[i] = num;
      //printf("%i\n", a[i]);
      i = i + 1;
    }
    if(feof(f))    {
        printf("Lectura correcta de elementos\n");
    }
    else    {
        printf("Error leyendo datos\n");
    }
    fclose(f);
}

/*-----------------------------------------------------------------
 * Function:  Generate_list
 * Purpose:   Use random number generator to generate list elements
 * In args:   n
 * Out args:  a
 */
void Generate_list(int a[], int n) {
   int i;
   srand(1);
   for (i = 0; i < n; i++)
      a[i] = rand() % RMAX;
}  /* Generate_list */

/*-----------------------------------------------------------------
 * Function:  Print_list
 * Purpose:   Print the elements in the list
 * In args:   a, n
 */
void Print_list(int a[], int n, char* title) {
   int i;

   printf("%s:\n", title);
   for (i = 0; i < n; i++)
      printf("%d ", a[i]);
   printf("\n\n");
}  /* Print_list */

/*-----------------------------------------------------------------
 * Function:  Read_list
 * Purpose:   Read elements of list from stdin
 * In args:   n
 * Out args:  a
 */
void Read_list(int a[], int n) {
   int i;

   printf("Please enter the elements of the list\n");
   for (i = 0; i < n; i++)
      scanf("%d", &a[i]);
}  /* Read_list */

/*-----------------------------------------------------------------
 * Function:     Odd_even
 * Purpose:      Sort list using odd-even transposition sort
 * In args:      n
 * In/out args:  a
 */

void Odd_even_First(int a[], int n) {
   int phase, i, tmp;
#  ifdef DEBUG
   char title[100];
#  endif

   for (phase = 0; phase < n; phase++) {
      if (phase % 2 == 0)
#        pragma omp parallel for num_threads(thread_count) \
            default(none) shared(a, n) private(i, tmp)
         for (i = 1; i < n; i += 2) {
            if (a[i-1] > a[i]) {
               tmp = a[i-1];
               a[i-1] = a[i];
               a[i] = tmp;
            }
         }
      else
#        pragma omp parallel for num_threads(thread_count) \
            default(none) shared(a, n) private(i, tmp)
         for (i = 1; i < n-1; i += 2) {
            if (a[i] > a[i+1]) {
               tmp = a[i+1];
               a[i+1] = a[i];
               a[i] = tmp;
            }
         }
#     ifdef DEBUG
      sprintf(title, "After phase %d", phase);
      Print_list(a, n, title);
#     endif
   }
}
void Odd_even_Second(int a[], int n) {
   int phase, i, tmp;

#  pragma omp parallel num_threads(thread_count) \
      default(none) shared(a, n) private(i, tmp, phase)
   for (phase = 0; phase < n; phase++) {
      if (phase % 2 == 0)
#        pragma omp for 
         for (i = 1; i < n; i += 2) {
            if (a[i-1] > a[i]) {
               tmp = a[i-1];
               a[i-1] = a[i];
               a[i] = tmp;
            }
         }
      else
#        pragma omp for 
         for (i = 1; i < n-1; i += 2) {
            if (a[i] > a[i+1]) {
               tmp = a[i+1];
               a[i+1] = a[i];
               a[i] = tmp;
            }
         }
   }
}
