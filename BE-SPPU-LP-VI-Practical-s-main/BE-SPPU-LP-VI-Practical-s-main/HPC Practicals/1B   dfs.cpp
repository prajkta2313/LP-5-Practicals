// Import necessary libraries
#include <iostream> // For input and output
#include <vector>   // For dynamic arrays (adjacency list)
#include <omp.h>    // For parallelism with OpenMP

using namespace std;

// Define a structure for the Graph
struct Graph
{
    int V;                   // Number of vertices
    vector<vector<int>> adj; // Adjacency list

    // Constructor to initialize graph with V vertices
    Graph(int V)
    {
        this->V = V;
        adj.resize(V); // Resize adjacency list to hold V vertices
    }

    // Function to add an edge between two vertices u and v
    void addEdge(int u, int v)
    {
        adj[u].push_back(v); // Add v to u's list
        adj[v].push_back(u); // Since it's an undirected graph, add u to v's list
    }

    // Function to start Depth First Search from a starting node
    void DFS(int start)
    {
        vector<bool> visited(V, false); // Track visited vertices

// Start parallel region
#pragma omp parallel
        {
#pragma omp single
            {
                DFSUtil(start, visited); // Only one thread starts the DFSUtil
            }
        }
        cout << endl;
    }

private:
    // Utility function for DFS
    void DFSUtil(int u, vector<bool> &visited)
    {
#pragma omp critical
        {
            if (visited[u])
                return;        // If already visited, return
            visited[u] = true; // Mark as visited
            cout << u << " ";  // Process the node (print it)
        }

// Explore all neighbors
#pragma omp parallel for
        for (int i = 0; i < adj[u].size(); i++)
        {
            int v = adj[u][i];
            if (!visited[v])
            {
#pragma omp task
                DFSUtil(v, visited); // Create a new task for each unvisited neighbor
            }
        }

#pragma omp taskwait // Wait for all child tasks to complete
    }
};

// Main function
int main()
{
    int V; // Number of vertices
    cout << "Enter number of vertices: ";
    cin >> V;

    Graph g(V); // Create a graph with V vertices

    int edgeCount; // Number of edges
    cout << "Enter number of edges: ";
    cin >> edgeCount;

    cout << "Enter edges (format: u v):\n";
    for (int i = 0; i < edgeCount; i++)
    {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v); // Add each edge to the graph
    }

    cout << "Parallel DFS traversal starting from node 0:\n";
    g.DFS(0); // Start DFS from node 0

    return 0;
}

/*
-------------------------
How to Compile and Run:
-------------------------
g++ -fopenmp DFS.cpp -o dfs
./dfs

-------------------------
Sample Input:
-------------------------
Enter number of vertices: 5
Enter number of edges: 4
Enter edges (format: u v):
0 1
0 2
1 3
1 4

Expected Output:
Parallel DFS traversal starting from node 0:
0 1 3 4 2
(Or other DFS order depending on thread scheduling)
*/
