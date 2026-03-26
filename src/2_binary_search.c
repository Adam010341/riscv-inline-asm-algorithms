#include <stdio.h>

/* TODO: Implement your code below */
int binarySearch(float *p_a, int arr_size, float target)
{
    int result = -1;

    asm volatile(
        "li t0, 0\n\t"//t0=left
        "addi t1, %[arr_size], -1\n\t"//t1=right

        "1:\n\t"
        "blt t1, t0, 5f\n\t"
        "add t2,t0,t1\n\t"
        "srli t2, t2, 1\n\t"//t2=mid=(left+right)/2
        "slli t3, t2, 2\n\t"//t3=t2*4=mid*4
        "add t4, %[p_a], t3\n\t"//t4=&A[mid]
    
        "flw ft0, 0(t4)\n\t"//ft0=A[mid]
        "feq.s t6, ft0, %[target]\n\t"//t6=1 if A[mid]=target
        "bnez t6, 3f\n\t"

        "flt.s t5, %[target], ft0\n\t"//t5==0 if target>A[mid]
        "beqz t5, 4f\n\t"
        "addi t1, t2, -1\n\t"
        "j 1b\n\t"

        "4:\n\t"//target>A[mid]
        "addi t0, t2, 1\n\t"
        "j 1b\n\t"
        
        "3:\n\t"//found
        "addi %[result], t2, 0\n\t"

        "5:\n\t"//not found
        :[result] "+r" (result)
        :[target] "f" (target), [p_a] "r" (p_a), [arr_size] "r" (arr_size)
        :"t0","t1","t2","t3","t4","t5","t6", "ft0","memory"
    );

    return result;
}

int main(int argc, char *argv[])
{
    FILE *input = stdin;
    
    if (argc >= 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "Error opening file: %s\n", argv[1]);
            return 1;
        }
    }
    
    // Read 'target'
    float target;
    fscanf(input, "%f", &target);
    
    // Read 'arr_size'
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

    int index = binarySearch(p_a, arr_size, target);

    // Print the result
    printf("%d ", index);
    printf("\n");

    return 0;
}
