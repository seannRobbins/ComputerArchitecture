#include <stdio.h>

int getBits(int instruction, int start, int end){
    int mask = 0;
    for(int i = start; i <= end; i++){
        mask += 1 << i;
    }
    return (instruction & mask) >> start;
}

int main(){
    printf("%d\n", getBits(0x1261, 12, 15));
}