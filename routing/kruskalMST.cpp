/*
Created on Tue May 26 16:39:22 2020

@author: Harshil
*/


/*  Kruskal’s Minimum Spanning Tree Algorithm
    This algorithm treats the graph as a forest and every node it has as an individual tree. 
    A tree connects to another only and only if, it has the least cost among all available options 
    and does not violate MST properties.
*/

#include <bits/stdc++.h>
using namespace std;


class Edge {
    public:
        int src, dest, weight;
};

class Graph {
    public:
        int numVertices, numEdges;
        Edge* edge;
};

Graph* createGraph(int numVertices, int numEdges) {
    Graph* graph = new Graph;
    graph -> numVertices = numVertices;
    graph -> numEdges    = numEdges;
    graph -> edge        = new Edge[numEdges];
    return graph;
}

class subset {
    public:
        int parent, rank;
};

int find(subset subsets[], int i) {
    if (subsets[i].parent != i)
        subsets[i].parent = find(subsets, subsets[i].parent);
    return subsets[i].parent;
}

void Union(subset subsets[], int x, int y) {
    int xroot = find(subsets, x);
    int yroot = find(subsets, y);

    if (subsets[xroot].rank < subsets[yroot].rank)
        subsets[xroot].parent = yroot;
    else if (subsets[xroot].rank > subsets[yroot].rank)
        subsets[yroot].parent = xroot;
    else {
        subsets[xroot].parent = xroot;
        subsets[xroot].rank++;
    }
}

int myComp(const void* a, const void* b) {
    Edge* a1 = (Edge*)a;
    Edge* b1 = (Edge*)b;
    return a1->weight > b1->weight;
}

void KruskalMST(Graph* graph) {
    int V = graph -> numVertices;
    Edge result[V];
    int e = 0, i = 0;

    qsort(graph->edge, graph->numEdges, sizeof(graph->edge[0]), myComp);

    subset *subsets = new subset[(V * sizeof(subset))];

    for (int v = 0; v < V; v++) {
        subsets[v].parent = v;
        subsets[v].rank   = 0;
    } 

    while (e < V-1 && i < graph->numEdges) {
        Edge next_edge = graph->edge[i++];
        int x = find(subsets, next_edge.src);
        int y = find(subsets, next_edge.dest);

        if (x!=y) {
            result[e++] = next_edge;
            Union(subsets, x, y);
        }
    }

    cout << "Following edges constructed in MST\n";
    for (i=0; i<e; i++)
        cout << result[i].src << " -- " << result[i].dest << " == " << result[i].weight << endl;
    return;
}


int main()  
{  
    /* Let us create following weighted graph  
            10  
        0--------1  
        | \ |  
    6| 5\ |15  
        | \ |  
        2--------3  
            4 */
    int numVertices = 4; // Number of vertices in graph  
    int numEdges = 5; // Number of edges in graph  
    Graph* graph = createGraph(numVertices, numEdges);  
  
  
    // add edge 0-1  
    graph->edge[0].src    = 0;  
    graph->edge[0].dest   = 1;  
    graph->edge[0].weight = 10;  
  
    // add edge 0-2  
    graph->edge[1].src    = 0;  
    graph->edge[1].dest   = 2;  
    graph->edge[1].weight = 6;  
  
    // add edge 0-3  
    graph->edge[2].src    = 0;  
    graph->edge[2].dest   = 3;  
    graph->edge[2].weight = 5;  
  
    // add edge 1-3  
    graph->edge[3].src    = 1;  
    graph->edge[3].dest   = 3;  
    graph->edge[3].weight = 15;  
  
    // add edge 2-3  
    graph->edge[4].src    = 2;  
    graph->edge[4].dest   = 3;  
    graph->edge[4].weight = 4;  
  
    KruskalMST(graph);  
  
    return 0;  
}  
  