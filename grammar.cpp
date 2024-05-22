#include <iostream>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "grammar.hpp"
#include "graph.hpp"
#include "definitions/define.hpp"
#include <set>

Grammar::Grammar(Graph graph)
{
    this->graph = graph;
    structure.open("structure.txt");
}

string writeType(string type)
{
    if (type == "string")
    {
        return STRING;
    }
    if (type == "integer" || type == "double")
    {
        return NUMBER;
    }
    if (type == "boolean")
    {
        return BOOl;
    }
    if (type == "null")
    {
        return type;
    }
    return type;
}

void Grammar::writeTaggedUnions(string origin, string parent, string path, string ruleName)
{
    if (!this->graph.lists.listNodes[origin]->hasTaggedUnion)
    {
        this->rules[ruleName] += "\n";
        return;
    }

    unordered_map<string, int> enumerates = this->graph.lists.listNodes[origin]->enumerate;
    for (auto x : this->graph.lists.listNodes[origin]->taggedUnions)
    {   
        for (auto z : this->graph.lists.listEdges[{x, origin}]->taggedUnion)
        {
            this->rules[ruleName] += 
                "IF "
                + this->graph.lists.listNodes[origin]->name
                + "."
                + z.first
                + " THEN "
                + x;
    
            writeStructure(x, parent, true, path, false);
            this->rules[ruleName] += "\n";
            if (enumerates[z.first]) enumerates.erase(z.first);
        }
    }
    if (enumerates.size() > 0)
    {
        for (auto x : enumerates)
        {

            this->rules[ruleName] += "IF "
                + this->graph.lists.listNodes[origin]->name
                + "."
                + x.first
                + "THEN"
                + " null\n";
        }
    }
    return;
}

void Grammar::writeRepeatedKeys(string path,
    string name,
    string pathBefore,
    string parent,
    bool canPrint,
    string ruleName)
{
    this->graph.lists.listPaths[path]->read = true;
    this->graph.lists.listPaths[path]->nodes.erase(name);

    if (this->graph.lists.listPaths[path]->nodes.size() == 0)
    {
        return;
    }

    set<string> notSomeonesTaggedUnion;

    for (auto x : this->graph.lists.listPaths[path]->nodes)
    {
        name = *this->graph.lists.listPaths[path]->nodes.begin();
        writeStructure(name, parent, canPrint, pathBefore, false);
        if (!this->graph.lists.listNodes[name]->isSomeonesTaggedUnion)
        {
            notSomeonesTaggedUnion.insert(name);
        }
    }
    
    for (auto x : notSomeonesTaggedUnion)
    {
        this->graph.lists.listPaths[path]->nodes.erase(name);
    }
}


void Grammar::printCollection(Node *node, string ruleName)
{
    set<string> types;
    for (ListEdge *aux = node->edges; aux != NULL; aux = aux->next)
    {
        types.insert(aux->edge->destiny->type);
    }

    this->rules[ruleName] += "{(";

    int i = 0;

    for (auto x : types)
    {
        if (i != 0)
        {
            this->rules[ruleName] += ", ";
            this->rules[ruleName] += STRING;
            this->rules[ruleName] += ":" + writeType(x);
        }
        else
        {
            this->rules[ruleName] += STRING;
            this->rules[ruleName] += ":" + writeType(x);
            i++;
        }
    }

    this->rules[ruleName] += ")+}\n";
    types.clear();
}

void Grammar::printTuple(Node *node, string ruleName)
{
    this->rules[ruleName] += "[";

    int i = 0;

    for (auto x : node->tuple->structure)
    {
        int quant = x.second;

        while (quant > 0)
        {
            quant--;
            if (i != 0)
            {
                this->rules[ruleName] += ", ";
            }
            else
            {
                i++;
            }

            this->rules[ruleName] += x.first;
        }
    }
    this->rules[ruleName] += "]\n";
}

void Grammar::writeStructure(string name, string parent, bool canPrint, string path, bool isObjectFromArray)
{
    string pathBefore = path;
    if (path != "" && !isObjectFromArray) path += "." + this->graph.lists.listNodes[name]->name;
    if (path == "" && !isObjectFromArray) path = this->graph.lists.listNodes[name]->name;

    if (isObjectFromArray)
    {
        name += ".object";
    }

    string ruleName = "";

    if (!this->graph.lists.listPaths[path] || (this->graph.lists.listNodes[name]->isSomeonesTaggedUnion && !canPrint)
        || this->graph.lists.listPaths[path]->nodes.size() == 0) return;

    bool isOptional = false;
    if (parent != "" && this->graph.lists.listEdges[{name, parent}]->probability < MAXOPTIONAL)
        isOptional = true;

    if (name == "start_object.object") {
        ruleName = "root";
        this->rules["root"] = "root::= ";
    }

    if (name != "start_object.object") {
        ruleName = name;
        this->rules[ruleName] = ruleName + "::= ";
    }

    if (this->graph.lists.listNodes[name]->type != "object" && this->graph.lists.listNodes[name]->type != "array")
    {
        if (!this->graph.lists.listNodes[name]->isEnum)
        {
            if (isOptional) this->rules[ruleName] += OPTIONAL;
            
            this->rules[ruleName] += writeType(this->graph.lists.listNodes[name]->type) + "\n";
        }
        else
        {
            if (this->graph.lists.listNodes[name]->enumerate.size() == 1)
            {
                for (auto x : this->graph.lists.listNodes[name]->enumerate)
                {
                    if (isOptional) this->rules[ruleName] += OPTIONAL;

                    this->rules[ruleName] += x.first;
                }
            }
            else
            {
                if (this->graph.lists.listNodes[name]->hasTaggedUnion)
                {
                    if (isOptional) this->rules[ruleName] += OPTIONAL;
                }
                else
                {
                    if (isOptional) this->rules[ruleName] += OPTIONAL;

                    this->rules[ruleName] += "[";
                    int i = 0;
                    for (auto x : this->graph.lists.listNodes[name]->enumerate)
                    {
                        if (i == 0)
                        {
                            this->rules[ruleName] += x.first;
                            i = 1;
                        }
                        else
                        {
                            this->rules[ruleName] += ", " + x.first;
                        }
                    }
                    this->rules[ruleName] += "]\n";
                }
            }
            writeTaggedUnions(name, parent, pathBefore, ruleName);
        }
        if (!this->graph.lists.listPaths[path]->read 
            && this->graph.lists.listPaths[path]->nodes.size() > 1)
                writeRepeatedKeys(path, name, pathBefore, parent, canPrint, ruleName);
        else this->graph.lists.listPaths[path]->read = true;
        return;
    }
    else if (this->graph.lists.listNodes[name]->type == "object" || this->graph.lists.listNodes[name]->type == "array")
    {
        if (this->graph.lists.listNodes[name]->type == "object")
        {
            ListEdge *aux = this->graph.lists.listNodes[name]->edges;

            if (isOptional) this->rules[ruleName] += OPTIONAL;
            if (this->graph.lists.listNodes[name]->isCollection) printCollection(this->graph.lists.listNodes[name], ruleName);
            else
            {
                while (aux != NULL)
                {
                    string nameEdge = "";
                    if (aux->edge->destiny->name == "") nameEdge = name; 
                    else nameEdge = aux->edge->destiny->name;
                    string nameToDisplay = nameEdge;
                    nameEdge = nameEdge + "." + aux->edge->destiny->type;
                    this->rules[ruleName] += " " + nameToDisplay + ": " + nameEdge;
                    writeStructure(nameEdge, name, canPrint, path, false);
                    aux = aux->next;
                };
            }
            writeTaggedUnions(name, parent, pathBefore, ruleName);
            if (name != "start_object.object")
            {
                if (!this->graph.lists.listPaths[path]->read 
                    && this->graph.lists.listPaths[path]->nodes.size() > 1)
                        writeRepeatedKeys(path, name, pathBefore, parent, canPrint, ruleName);
                else this->graph.lists.listPaths[path]->read = true;
                return;
            }
            return;
        }
        if (this->graph.lists.listNodes[name]->type == "array")
        {
            if (isOptional)
                this->rules[ruleName] += OPTIONAL;
            if (this->graph.lists.listNodes[name]->isTuple) printTuple(this->graph.lists.listNodes[name], ruleName);
            if (!this->graph.lists.listNodes[name]->isTuple)
            {
                this->rules[ruleName] += "[";
                int i = 0;
                if (this->graph.lists.listNodes[name]->isEnum)
                {
                    for (auto x : this->graph.lists.listNodes[name]->enumerate)
                    {
                        if (i == 0)
                        {
                            this->rules[ruleName] += x.first;
                            i = 1;
                        }
                        else this->rules[ruleName] += ", " + x.first;
                    }
                }
                else
                {
                    for (ListEdge *aux = this->graph.lists.listNodes[name]->edges; aux != NULL; aux = aux->next)
                    {
                        if (aux->edge->relationship != "sibling")
                        {
                            string nameEdge = "";
                            if (aux->edge->destiny->name == "") nameEdge = name; 
                            else nameEdge = aux->edge->destiny->name;
                            string edgePath = path + "." + aux->edge->destiny->name;
                            if (i == 0)
                            {
                                if (aux->edge->destiny->type == "object")
                                {
                                    this->rules[ruleName] += nameEdge + ".object";
                                    writeStructure(nameEdge, name, true, edgePath, true);
                                }
                                else this->rules[ruleName] += aux->edge->destiny->type;
                                i = 1;
                            }
                            else
                            {
                                if (aux->edge->destiny->type == "object")
                                {
                                    this->rules[ruleName] += ", " + nameEdge + ".object";
                                    writeStructure(nameEdge, name, true, edgePath, true);
                                }
                                else this->rules[ruleName] += ", " + aux->edge->destiny->type;
                            }
                        }
                    }
                }
                this->rules[ruleName] += "]\n";
            }
            writeTaggedUnions(name, parent, pathBefore, ruleName);
            if (!this->graph.lists.listPaths[path]->read 
                && this->graph.lists.listPaths[path]->nodes.size() > 1)
                writeRepeatedKeys(path, name, pathBefore, parent, canPrint, ruleName);
            else this->graph.lists.listPaths[path]->read = true;
            return;
        }
    }
}

void Grammar::printAll()
{
    for (auto x : this->rules)
    {
        this->structure << x.second;
    }
    structure.close();
}