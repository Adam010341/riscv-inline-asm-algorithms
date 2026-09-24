#include <stdio.h>
#include <string.h>

/* Implemented in RV64IF inline assembly (see README for the design notes) */
int insertionSort(float *p_a, int arr_size)
{
    int shift_cnt = 0;  /* shift count */

    for (int i = 1; i < arr_size; i++) {
        float *p_i = p_a + i;  /* index of i-th element */
        int j = i - 1;
        
        asm volatile(
            "flw ft0, 0(%[p_i])\n\t"//ft0=key
            "slli t0, %[j], 2\n\t"//t0=j*4
            "add t1, %[p_a], t0\n\t"//t1=&A[j]

            "1:\n\t"
            "bltz %[j],3f\n\t"
            "flw ft1, 0(t1)\n\t"//ft1=A[j]
            "flt.s t2, ft0,ft1\n\t"//t2==1 if  key<A[j]
            "beqz t2, 3f\n\t"

            "fsw ft1, 4(t1)\n\t"
            "addi %[j], %[j], -1\n\t"
            "addi %[shift_cnt],%[shift_cnt], 1\n\t"
            "addi t1, t1, -4\n\t"
            "j 1b\n\t"

            "3:\n\t"
            "fsw ft0, 4(t1)\n\t"//A[j+1]=key
            :[j] "+r" (j), [shift_cnt] "+r" (shift_cnt)
            :[p_i] "r" (p_i), [p_a] "r" (p_a)
            :"t0","t1","t2","ft0","ft1","memory"
        );
    }
    
    return shift_cnt;
}

int main(int argc, char *argv[])
{
    FILE *input = stdin;
    FILE *output_file = NULL;
    float target = -1;
    
    if (argc >= 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "Error opening file: %s\n", argv[1]);
            return 1;
        }
    }
    
    // Read 'target' from the input file of problem 2 (2_*.txt) first
    if (argc >= 3) {
        output_file = fopen(argv[2], "r");
        if (output_file) {
            fscanf(output_file, "%f", &target);
            fclose(output_file);
        }
    }
    
    int arr_size;
    fscanf(input, "%d", &arr_size);
    float arr[arr_size];

    // Read 'floats' from input into the array
    for (int i = 0; i < arr_size; i++) {
        float data;
        fscanf(input, "%f", &data);
        arr[i] = data;
    }
    
    if (argc >= 2) {
        fclose(input);
    }

    float *p_a = &arr[0];
    
    // Call insertion sort function
    int shift_cnt = insertionSort(p_a, arr_size);
    
    p_a = &arr[0];
    
    // Write to output file
    if (argc >= 3 && argv[2]) {
        output_file = fopen(argv[2], "w");
        if (output_file) {
            // target
            fprintf(output_file, "%.1f\n", target);
            // arr_size
            fprintf(output_file, "%d\n", arr_size);
            // sorted array
            for (int i = 0; i < arr_size; i++) {
                fprintf(output_file, "%.1f ", arr[i]);
            }
            fprintf(output_file, "\n");
            fclose(output_file);
        }
    }
    
    // Print the result
    for (int i = 0; i < arr_size; i++) {
        printf("%.1f ", *p_a++);
    }
    printf("\n");
    printf("%d\n", shift_cnt);
    return 0;
}
