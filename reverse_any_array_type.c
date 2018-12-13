/*
    This program attempts to create a universal reverse function in C.

    Note that because of the way the program reverses data, cells are NOT reversed independently
    but rather, blocks are. ie when reversing integers for example;
        int arr[] = {1,2};
        arr is layed down as 00 00 00 01 00 00 00 02 (1=00000001, 2=00000002)
        reversing one byte at a time would result into 02 00 00 00 01 00 00 00,
        and obviously this is not what you want as the integers are now {02000000,01000000}
        Rather, we reverse blocks(4bytes for int) getting the result 00 00 00 02 00 00 00 01
        giving {00000002,00000001} which is the desired output!

    Also, note that becuase the blocks are swapped instead of individual bytes, endianess is NOT
    important and cant affect the swap/reverse operation(and this is a huge bonus)

    Author: Bukman, glayn2bukman@gmail.com
*/
#include<stdio.h>
#include<stdlib.h>

void reverse(void *arr, size_t unit_size, size_t count){
    // reverse array IN-PLACE
    // unit_size: size of each item in the array (sizeof(*arr) when calling the function)
    // count    : number of items in the array
    //
    // note that special care is taken for strings as you dont want to have '\0' at the beginning
    // for this reason, DONT include the '\0' in <count>(most functions that count the length
    // of a string do this)
    //
    // a typical way to call this function is eg
    //
    //      char *names[] = {...};
    //      reverse(names, sizeof(*names), sizeof(names)/sizeof(*names));
    //
    // the operations of this function are ENDEFINED IF <count> DOES not make sense eg when
    // <count> is computed as sizeof(names)/sizeof(*names) when the variable(names) is a pointer and
    // not an array(as the case when the array decays into a pointer when passed to a function)
    //
    // also, because this function sorts data in-place, use function <reversed> if the array
    // is of readonly data as is the case for
    //
    //      char *str = "hello, world!"; // str holds CONSTANT char* data
    //      reverse(str, sizeof(*str), strlen(str)); // Undefined Behaviou
    //
    //      // better solution...
    //      char *str = "hello, world!";
    //      str = reversed(str, sizeof(*str), strlen(str));
    //

    char *narr = arr, byte;

    size_t _count = 0, local_index;
    while(_count<(count/2)){
        if(unit_size>sizeof(char)){
            for(local_index=0; local_index<unit_size; ++local_index){
                byte = *(narr+(unit_size*_count)+local_index);
                *(narr+(unit_size*_count)+local_index) = *(narr+(unit_size*(count-_count-1))+local_index);
                *(narr+(unit_size*(count-_count-1))+local_index) = byte;
            }
        }else{ // for strings, the operation is unique...
            byte = *(narr+_count);
            *(narr+_count) = *(narr+(count-_count-1));
            *(narr+(count-_count-1)) = byte;
        }
        ++_count;
    }
}


void* reversed(void *arr, size_t unit_size, size_t count){
    // create copy of array, reverse copy IN-PLACE, and then return the reversed copy
    // PLEASE free the memory allocated by this function when done using it!
    char *narr=arr, *copy;

    if(unit_size>sizeof(char)){copy = malloc(sizeof(char)*(count*unit_size));}
    else{ copy = malloc(sizeof(char)*((count+1)*unit_size));}

    if(!copy) return NULL; // check for this in your code!

    for(size_t i=0; i<(unit_size*count); ++i) copy[i]=narr[i];

    reverse(copy,unit_size,count);

    return copy;
}


int main(){
    // simple arrays...
    int iarray[] = {0,1,2,3,4,5,6};
    float farray[] = {0.23,4.57,9.18,0.68};
    char *str = "hello there"; /*  char *str = "hello there" would raise a seg-fault as
                                   str would then pointer to READ-ONLY memory. to solve
                                   this, we use function reversed(instead of reverse) for this*/
    char str2[] = "second string!";

    // 2-d arrays...
    int iarray2d[2][2] = {{0,1},{2,3}};
    char *words[] = {"Hello","Darkness","My","Old","Friend"};

    // more complexity...
    struct person{char *name; int age;}
        tom   = {.name="Tommy", .age=21},
        jerry = {.name="Jerry", .age=25};
    struct person *ppeople[] = {&tom, &jerry}, people[]={tom,jerry};

    // define functions to print our data...(dont mind these)
    void print_array(void *array, size_t count, char type){
        printf("\n%c-array: ",type);
        if('i'==type){ for(size_t i=0; i<count; ++i)printf("%d ",*((int*)array+i)); }
        else if('f'==type){ for(size_t i=0; i<count; ++i)printf("%.4f ",*((float*)array+i)); }
        else if('c'==type){ printf("%s ",(char*)array); }
        if('S'==type){ for(size_t i=0; i<count; ++i)printf("%s ",*((char**)array+i)); }
    }
    size_t strlen(const char *s){size_t l=0; while(*s && ++l && ++s); return l;}

    print_array(iarray, sizeof(iarray)/sizeof(*iarray), 'i');
    reverse(iarray, sizeof(*iarray), sizeof(iarray)/sizeof(*iarray));
    print_array(iarray, sizeof(iarray)/sizeof(*iarray), 'i');
    printf("\n");

    print_array(farray, sizeof(farray)/sizeof(*farray), 'f');
    reverse(farray, sizeof(*farray), sizeof(farray)/sizeof(*farray));
    print_array(farray, sizeof(farray)/sizeof(*farray), 'f');
    printf("\n");

    print_array(str, 0, 'c');
    str = reversed(str,sizeof(char), strlen(str)); /*notice that strlen DOES NOT include the terminating '\0'
                                                     since str currently points to const-char*,
                                                     we use function <reversed> to as it creates
                                                     a writable copy for the readonly memory
                                                     and then modifies that memory!*/
    print_array(str, 0, 'c');
    printf("\n");
    free(str); /* YOU WANT TO FREE THE MEMORY ALLOCATED BY function <reversed>*/

    print_array(str2, 0, 'c');
    reverse(str2, 1, sizeof(str2)-1); //notice that the terminating '\0' is still NOT used for the last argument
    print_array(str2, 0, 'c');
    printf("\n");

    printf("\ni2d-array: { (%d,%d), (%d,%d) }",iarray2d[0][0],iarray2d[0][1],iarray2d[1][0],iarray2d[1][1]);
    reverse(iarray2d, sizeof(*iarray2d), sizeof(iarray2d)/sizeof(*iarray2d));
    printf("\ni2d-array: { (%d,%d), (%d,%d) }",iarray2d[0][0],iarray2d[0][1],iarray2d[1][0],iarray2d[1][1]);
    printf("\n");

    print_array(words, sizeof(words)/sizeof(*words), 'S');
    reverse(words, sizeof(*words), sizeof(words)/sizeof(*words));
    print_array(words, sizeof(words)/sizeof(*words), 'S');
    printf("\n");

    printf("\nstruct*-array: { (%s,%d), (%s,%d) }",(*ppeople)->name,(*ppeople)->age,(*(ppeople+1))->name,(*(ppeople+1))->age);
    reverse(ppeople, sizeof(*ppeople), sizeof(ppeople)/sizeof(*ppeople));
    printf("\nstruct*-array: { (%s,%d), (%s,%d) }",(*ppeople)->name,(*ppeople)->age,(*(ppeople+1))->name,(*(ppeople+1))->age);
    printf("\n");

    printf("\nstruct-array: { (%s,%d), (%s,%d) }",(*people).name,(*people).age,(*(people+1)).name,(*(people+1)).age);
    reverse(people, sizeof(*people), sizeof(people)/sizeof(*people));
    printf("\nstruct-array: { (%s,%d), (%s,%d) }",(*people).name,(*people).age,(*(people+1)).name,(*(people+1)).age);
    printf("\n");


    return 0;
}
