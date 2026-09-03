#include "BlockPool.h"
#include "Connection.h"

class AudioRuntime {
public:
    AudioRuntime(BlockPool& pool); // Accept a reference to a BlockPool 

    bool addNode(ProcessingNode* node); //Append a node to node array

    bool addConnection(ProcessingNode* source, // Append a connection to connections array
                       ProcessingNode* destination);

    bool submitInput(ProcessingNode* destination,
                     AudioBlock* block);

    void runTick();

private:
    static constexpr int MAX_NODES = 8;
    static constexpr int MAX_CONNECTIONS = 12;

    BlockPool& blockPool;

    ProcessingNode* nodes[MAX_NODES];
    int nodeCount;

    Connection connections[MAX_CONNECTIONS];
    int connectionCount;

    void routeOutput(ProcessingNode* source,
                     AudioBlock* block);
};