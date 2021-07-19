#include <vector>
#include <iostream> 
#include <stdlib.h>

int partition(std::vector<int>& vals, int low, int high){
    int pivotIndex = low + ((high - low) / 2);
    int pivotValue = vals[pivotIndex];
    int i = low, j = high;
    int temp;
    while(i <= j) {
        while(vals[i] < pivotValue) {
            i++;
        }
        while(vals[j] > pivotValue) {
            j--;
        }
        if(i <= j) {
            temp = vals[i];
            vals[i] = vals[j];
            vals[j] = temp;
            i++;
            j--; 
        }
    }
    return i;
}

void quickSort(std::vector<int>& vals, int low, int high){
    if (low < high){
        int pivot = partition(vals, low, high);
        quickSort(vals, low, pivot-1);
        quickSort(vals, pivot, high);
    }
}

int main(){
    std::vector<int> myVector = { 90, 20, 30, 50, 40 };
    int l = myVector.size();

    for (int i=0; i<l; ++i){
        std::cout << myVector[i] << std::endl; 
    }

    quickSort(myVector, 0, l-1); 

    std::cout << "Quicksort Completed" << std::endl; 
    for (int i=0; i<l; ++i){
        std::cout << myVector[i] << std::endl; 
    }

    return 0; 
}
