/*
Created on Tue May 26 18:57:22 2020

@author: Harshil
*/

#include <bits/stdc++.h>
using namespace std;

#define numVertices 5

/*
A utility function to find the vertex with minimum key value, 
from the set of vertices not yet included in MST  
*/
int minKey(int key[], bool mstSet[]) {
    int min = INT_MAX, min_index;
    for (int v=0; v<numVertices; v++) {
        if (mstSet[v] == false && key[v] < min)
            min = key[v];
            min_index = v;
    }
    return min_index;
}

/* A utility function to print the constructed MST */
void printMST(int parent[], int graph[numVertices][numVertices]) {
    
}