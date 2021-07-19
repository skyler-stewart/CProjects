#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <assert.h> 
#include <time.h>

#define THREAD_MAX 20
int threadCount = 0; 

void printVector(std::vector<int> myVector){
    for (int i = 0; i < myVector.size(); ++i){
        std::cout << myVector[i] << ' '; 
    }
    std::cout << std::endl;
}

// Merge Function ///////////////////////////////////////////////////////////////////
void merge(std::vector<int> &myVector, int left, int middle, int right){
    int p1 = left; 
    int p2 = middle+1;
    std::vector<int> mergedVector;

    // Main merge 
    while (p1 <= middle && p2 <= right){
        if (myVector[p1] <= myVector[p2]){
            mergedVector.push_back(myVector[p1]);
            p1++;  
        }
        else{
            mergedVector.push_back(myVector[p2]);
            p2++;  
        }
    }

    // Add dangling elements 
    while (p1 <= middle){
        mergedVector.push_back(myVector[p1]);
        p1++; 
    }
    while (p2 <= right){
        mergedVector.push_back(myVector[p2]);
        p2++; 
    }

    // Update original vector
    for (int i = 0; i < mergedVector.size(); i++){
        myVector[left + i] = mergedVector[i]; 
    }
}

// Sort Function ///////////////////////////////////////////////////////////////////
void mergeSort(std::vector<int> &myVector, int start, int end){
    if (start < end){
        int mid = start + int((end - start) / 2);

        // Multithreaded recursive calls
        if (threadCount < THREAD_MAX){
            threadCount += 2;
            std::thread leftThread(mergeSort, std::ref(myVector), start, mid);
            std::thread rightThread(mergeSort, std::ref(myVector), mid+1, end);
            leftThread.join(); 
            rightThread.join(); 
            threadCount -= 2; 
        }
        else{
            // Non-Multithreaded recursive calls 
            mergeSort(myVector, start, mid);
            mergeSort(myVector, mid+1, end); 
        }
        merge(myVector, start, mid, end);
    } 
}

// Main Function ///////////////////////////////////////////////////////////////////
int main(){
    // Create vector 
    std::vector<int> testVector;
    srand(time(NULL)); 
    for (int i = 0; i < 100000; ++i) {
        testVector.push_back(rand()%100); 
    }

    // Print and sort 
    printVector(testVector); 
    mergeSort(testVector, 0, testVector.size()-1);
    printVector(testVector);

    // Check if correctly sorted 
    assert(std::is_sorted(testVector.begin(), testVector.end()));
    return 0; 
}
