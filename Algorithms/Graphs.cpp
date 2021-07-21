#include <iostream>
#include <vector> 
#include <algorithm> 
#include <limits> 

// Node definition 
struct Node{
    // Node contents 
    int value;
    std::vector<Node> neighbors; 
    // Helper variables for traversal 
    bool visited = false;
}; 

// Node equality
bool operator==(Node const& node1, Node const& node2){
    return node1.value == node2.value; 
}

// Graph definition (directed graph)
class Graph{
    public:
        // Data  
        std::vector<Node> graphNodes;
        int numNodes = 0; 

        // Add an existing node to the graph
        void addNode(Node newNode){
            graphNodes.push_back(newNode); 
            numNodes += 1; 
        }

        // Create a new node and add it to the graph 
        void addNode(int nodeValue){
            Node newNode = {nodeValue};
            graphNodes.push_back(newNode); 
            numNodes += 1; 
        }

        // Add a new edge 
        void addEdge(int start, int end){
            
            // Find start and end nodes if possible
            int source = -1; 
            int dest = -1; 
            for (int i = 0; i < numNodes; ++i){
                if (graphNodes[i].value == start) source = i; 
                if (graphNodes[i].value == end) dest = i; 
            }
            if (source != -1 && dest != -1){
                graphNodes[source].neighbors.push_back(graphNodes[dest]);
            }
            else{
                std::cout << "Edge not added" << std::endl; 
            }
        }

        // Remove a node by value 
        void removeNode(int nodeValue){
            for (int i = 0; i < numNodes; i++){
                if (graphNodes[i].value == nodeValue){
                    graphNodes.erase(graphNodes.begin() + i); 
                }
            }
        }

        // Print graph
        void printGraph(){
            for (int i = 0; i < numNodes; ++i){
                std::cout << "Node{value = " + std::to_string(graphNodes[i].value) + " neighbors = "; 
                for (int j = 0; j < graphNodes[i].neighbors.size(); ++j){
                    std::cout << std::to_string(graphNodes[i].neighbors[j].value) << ' '; 
                }
                std::cout << "}" << std::endl; 
            }
        }

        // Depth first traversal 
        void DFS(Node startNode){
            // Reset node visited status
            for (int i = 0; i < graphNodes.size(); ++i){
                graphNodes[i].visited = false; 
            }
            // Create stack 
            std::vector<Node> stack; 
            stack.push_back(startNode);

            // Main loop 
            while (!stack.empty()){
                Node current = stack.back(); 
                stack.pop_back(); 
                if (!current.visited){
                    current.visited = true; 
                    for (int i = 0; i < current.neighbors.size(); ++i){
                        stack.push_back(current.neighbors[i]); 
                    }
                }
            }
        }

        // Breadth first traversal 
        void BFS(Node startNode){
            // Reset node visited status
            for (int i = 0; i < graphNodes.size(); ++i){
                graphNodes[i].visited = false; 
            }
            // Setup queue
            std::vector<Node> queue; 
            startNode.visited = true; 
            queue.push_back(startNode); 
            // Main loop 
            while (!queue.empty()){
                Node current = queue.front(); 
                queue.erase(queue.begin());
                for (int i = 0; i < current.neighbors.size(); ++i){
                    if (current.neighbors[i].visited == false){
                        current.neighbors[i].visited = true; 
                        queue.push_back(current.neighbors[i]); 
                    }
                }

            }

        }
};

int main(){

    Graph myGraph; 
    myGraph.addNode(1); 
    myGraph.addNode(2); 
    myGraph.addNode(3); 
    myGraph.addNode(4); 

    myGraph.addEdge(1, 2); 
    myGraph.addEdge(2, 3); 
    myGraph.addEdge(3, 4); 
    myGraph.addEdge(2, 4); 

    myGraph.printGraph(); 
    return 0; 
}