#ifndef GRAPH_HPP

#define GRAPH_HPP

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include "queue.hpp"

using namespace std;

typedef struct Node Node;
typedef struct Edge Edge;
typedef struct ListEdge ListEdge;
typedef struct ListEdgesKey ListEdgesKey;
typedef struct Collection Collection;

struct Node
{
    string name;
    int counter;
    string type;
    unordered_map<string, int> enumerate; // value, counter
    unordered_map<string, int> paths; //string -> path int -> counter (arrumar para paths grandes)
    unordered_map<int, Collection*> arrayInfo; //total, structure
    vector<string> taggedUnions; //tagged unions que originou
    Collection *tuple;
    bool hasTaggedUnion;
    bool isSomeonesTaggedUnion;
    bool isEnum;
    bool collectionWasModified;
    bool isCollection;
    bool isTuple;
    ListEdge *edges;
};

struct Edge
{
    Node *origin;
    Node *destiny;
    string relationship;
    int counter;
    float probability;
    bool isTaggedUnion;
    unordered_map<string, int> taggedUnion; //string -> value (se não for string, converte pra string)
};

//Enumerate = (key: its value, value: count (how many times the relationship happened))

struct ListEdge
{
    Edge *edge;
    ListEdge *next;
};

struct ListEdgesKey
{
    string edge;
    string node;

    bool operator==(const ListEdgesKey& other) const {
        return std::tie(edge, node) == std::tie(other.edge, other.node);
    }
};

namespace std {
    template <>
    struct hash<ListEdgesKey> {
        std::size_t operator()(const ListEdgesKey& key) const {
            return std::hash<string>()(key.edge) ^ std::hash<string>()(key.node);
        }
    };
}

struct Path
{
    bool read;
    unordered_set<string> nodes;
};

struct Lists
{
    unordered_map<string, Node*> listNodes;
    unordered_map<ListEdgesKey, Edge*> listEdges;
    unordered_map<string, Path*> listPaths;
};

struct Collection
{
    int size;
    int counter;
    bool valid;
    unordered_map<string, int> structure; //type // counter
};

// enum é uma sequência de caracteres sem espaço
// dar free em tudo

class Graph
{
    public:
        Lists lists;

        QueueStruct* getAtt(string name, string type, string parent, QueueStruct *q, string path);
        void saveEnum(QueueStruct *q);
        void saveEnum2(string name, string value);
        void saveEnumArray(string name, vector<string> enums);
        void getTaggedUnions(string name, string type, string parent, Queue q);
        void saveTaggedUnion(Queue q);
        void insertNode(string name, string type, string path, Node* parent, Node *parentParent);
        void insertEdge(string origin, string destiny, string relationshipType, string value);
        void setArray(string name, int counter, unordered_map<string, int> set);
        void checkEnums();
        void setCollectionStructure(queue<std::string> children, Node *node);
        void checkTupleAndCollection();
        void checkObject(Node *node);
        void checkArray(Node *node);
        void insertStructure(queue<std::string> children, Node *node);
        void checkIfItsEqual(queue<std::string> children, Node *node);
        void checkArrayEnum(QueueStruct *q, vector<string> enums, string name);
        void setPath(string path, string name);
        void checkArrayType();
        void percentualEdge();
        void printGraph();
        void printTaggedUnions();
        void printTypes();
        void printArrays();
        void freeGraph(); //CONFERIR
};

#endif