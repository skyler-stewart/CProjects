#include <iostream>
#include <vector> 
#include <algorithm> 

// Node definition 
struct Node{
    int value;
    std::vector<Node> neighbors; 
    bool visited = false; 
}; 

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
        void addNode(int nodeValue, std::vector<Node> nodeNeighbors){
            Node newNode = {nodeValue, nodeNeighbors};
            graphNodes.push_back(newNode); 
            numNodes += 1; 
        }

        // Add a new edge 
        void addEdge(int start, int end){

            auto source = std::find(graphNodes.begin(), graphNodes.end(), start);
            auto destination = std::find(graphNodes.begin(), graphNodes.end(), end);
            if (source != graphNodes.end() && destination != graphNodes.end()){
                auto sourceIndex = source - graphNodes.begin();
                auto destinationIndex = destination - graphNodes.begin(); 
                graphNodes[sourceIndex].neighbors.push_back(graphNodes[destinationIndex]); 
            }
            else{
                std::cout << 'Edge not added' << std::endl; 
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
                std::cout << "Node{ value = " + std::to_string(graphNodes[i].value) + " neighbors = "; 
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
            std::vector<Node> queue; 
            startNode.visited = true; 
            queue.push_back(startNode); 
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

        // Shortest path between two nodes 
        void Dijkstra(Node startNode, Node endNode){

        }
};

int main(){

    Graph myGraph; 
    return 0; 
}