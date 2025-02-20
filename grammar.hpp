#ifndef GRAMMAR_HPP

#define GRAMMAR_HPP

#include <iostream>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <map>
#include "graph.hpp"

using namespace std;

struct Rule {
    char name;
    string description;
};

class Grammar 
{
    public:
        Graph graph;
        ofstream structure;
        map<string, string> rules;

    public:
        Grammar(Graph graph, string fileGenerated);
        void writeStructure(string name, string parent, bool canPrint, string path, bool isObjectFromArray, bool getMinAndMax);
        void writeTaggedUnions(string origin, string parent, string path, string ruleName, bool getMinAndMax);
        void writeRepeatedKeys(
            string path, 
            string name, 
            string pathBefore, 
            string parent, 
            bool canPrint, 
            string ruleName, 
            string parentRule,
            bool getMinAndMax);
        void printCollection(Node *node, string ruleName);
        void printTuple(Node *node, string ruleName);
        void printAll();
};

#endif