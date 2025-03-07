#include <iostream>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "grammar.hpp"
#include "graph.hpp"
#include "definitions/define.hpp"
#include <set>

Grammar::Grammar(Graph graph, string fileGenerated)
{
    this->graph = graph;
    structure.open(fileGenerated);
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
    if (type == "date")
    {
        return DATE;
    }
    if (type == "time")
    {
        return TIME;
    }
    if (type == "timestamp")
    {
        return TIMESTAMP;
    }
    
    return type;
}

void Grammar::writeTaggedUnions(string origin, string parent, string path, string ruleName, bool getMinAndMax)
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
    
            writeStructure(x, parent, true, path, false, getMinAndMax);
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
    string ruleName,
    string parentRule,
    bool getMinAndMax)
{
    this->graph.lists.listPaths[path]->read = true;
    this->graph.lists.listPaths[path]->nodes.erase(name);

    if (this->graph.lists.listPaths[path]->nodes.size() == 0)
    {
        return;
    }

    set<string> notSomeonesTaggedUnion;
    unordered_map<string, bool> readTypes;

    readTypes[writeType(this->graph.lists.listNodes[name]->type)] = true;

    for (auto x : this->graph.lists.listPaths[path]->nodes)
    {
        name = *this->graph.lists.listPaths[path]->nodes.begin();
        if (readTypes[writeType(this->graph.lists.listNodes[name]->type)])
        {
            continue;
        }
        if ((this->graph.lists.listNodes[name]->type != "array" 
                || (this->graph.lists.listNodes[name]->type == "array" && this->graph.lists.listNodes[name]->isEnum)) 
            && this->graph.lists.listNodes[name]->type != "object"
            && !this->graph.lists.listNodes[name]->hasTaggedUnion
            && !this->graph.lists.listNodes[name]->isSomeonesTaggedUnion)
        {
            this->rules[parentRule] += " | ";
        }
        writeStructure(name, parent, canPrint, pathBefore, false, getMinAndMax);
        if (!this->graph.lists.listNodes[name]->isSomeonesTaggedUnion)
        {
            notSomeonesTaggedUnion.insert(name);
        }
        readTypes[writeType(this->graph.lists.listNodes[name]->type)] = true;
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

void Grammar::writeStructure(
    string name, 
    string parent, 
    bool canPrint, 
    string path, 
    bool isObjectFromArray, 
    bool getMinAndMax)
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

    if (name == "start_object.object") {
        ruleName = "root";
        this->rules["root"] = "root::= ";
    }

    if (name != "start_object.object" 
        && (this->graph.lists.listNodes[name]->type == "object" 
            || (this->graph.lists.listNodes[name]->type == "array"
                    && !this->graph.lists.listNodes[name]->isEnum))) 
    {
        ruleName = name;
        this->rules[ruleName] = ruleName + "::= ";
    }

    if (this->graph.lists.listNodes[name]->type != "object" && this->graph.lists.listNodes[name]->type != "array")
    {
        string parentRule = parent;

        if (parent == "start_object.object") 
        {
            parentRule = "root";
        }
        
        if (!this->graph.lists.listNodes[name]->isEnum)
        {   
            this->rules[parentRule] += writeType(this->graph.lists.listNodes[name]->type);
            if (this->graph.lists.listNodes[name]->key) 
            {
                this->rules[parentRule] += "k";
            }
            if (getMinAndMax 
                && (this->graph.lists.listNodes[name]->type == "integer" 
                    || this->graph.lists.listNodes[name]->type == "double"))
            {
                double min = this->graph.lists.listNodes[name]->min - (this->graph.lists.listNodes[name]->min * MINANDMAXTHRESHOLD);
                double max = this->graph.lists.listNodes[name]->max + (this->graph.lists.listNodes[name]->max * MINANDMAXTHRESHOLD);
                if (this->graph.lists.listNodes[name]->type == "integer")
                {
                    int mininum = min;
                    int maximum = max;
                    this->rules[parentRule] += "(min=" + to_string(mininum)   + " max=" + to_string(maximum) + ")";
                }
                else 
                {
                    this->rules[parentRule] += "(min=" + to_string(min)   + "; max=" + to_string(max) + ")";
                }
            } 
        }
        else
        {
            if (this->graph.lists.listNodes[name]->enumerate.size() == 1)
            {
                for (auto x : this->graph.lists.listNodes[name]->enumerate)
                {
                    this->rules[parentRule] += x.first;
                }
            }
            else if (!this->graph.lists.listNodes[name]->hasTaggedUnion)
            {
                this->rules[parentRule] += "[";
                int i = 0;
                for (auto x : this->graph.lists.listNodes[name]->enumerate)
                {
                    if (i == 0)
                    {
                        this->rules[parentRule] += x.first;
                        i = 1;
                    }
                    else
                    {
                        this->rules[parentRule] += ", " + x.first;
                    }
                }
                this->rules[parentRule] += "]";
            }
            if (this->graph.lists.listNodes[name]->hasTaggedUnion) {
                ruleName = name;
                this->rules[ruleName] = ruleName + "::= ";
                writeTaggedUnions(name, parent, pathBefore, ruleName, getMinAndMax);
            }
        }

        if (!this->graph.lists.listPaths[path]->read 
            && this->graph.lists.listPaths[path]->nodes.size() > 1)
                writeRepeatedKeys(path, name, pathBefore, parent, canPrint, ruleName, parentRule, getMinAndMax);
        else this->graph.lists.listPaths[path]->read = true;
        return;
    }
    else if (this->graph.lists.listNodes[name]->type == "object" || this->graph.lists.listNodes[name]->type == "array")
    {
        if (this->graph.lists.listNodes[name]->type == "object")
        {
            ListEdge *aux = this->graph.lists.listNodes[name]->edges;

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

                    string pathEdge = path;
                    if (path != "") pathEdge += "." + nameToDisplay;

                    if ((!this->graph.lists.listPaths[pathEdge] || this->graph.lists.listPaths[pathEdge]->read)
                        && this->graph.lists.listPaths[pathEdge]->nodes.size() != 1)
                    {
                    } 
                    else 
                    {
                        if (!aux->edge->destiny->hasTaggedUnion 
                            && aux->edge->destiny->type != "object"
                            && (aux->edge->destiny->type != "array" || aux->edge->destiny->isEnum)) {
                                this->rules[ruleName] += " " + nameToDisplay + ": ";
                            }
                        else this->rules[ruleName] += " " + nameToDisplay + ": " + nameEdge;
                    }
                    writeStructure(nameEdge, name, canPrint, path, false, getMinAndMax);
                    aux = aux->next;
                };
            }
            writeTaggedUnions(name, parent, pathBefore, ruleName, getMinAndMax);
            if (parent == "start_object.object")
            {
                parent = "root";
            }

            if (!this->graph.lists.listPaths[path]->read 
                && this->graph.lists.listPaths[path]->nodes.size() > 1)
                    writeRepeatedKeys(path, name, pathBefore, parent, canPrint, ruleName, parent, getMinAndMax);
            else this->graph.lists.listPaths[path]->read = true;
            return;
        }
        if (this->graph.lists.listNodes[name]->type == "array")
        {
            if (this->graph.lists.listNodes[name]->isTuple) printTuple(this->graph.lists.listNodes[name], ruleName);
            if (!this->graph.lists.listNodes[name]->isTuple)
            {
                ruleName = name;
                int i = 0;
                if (this->graph.lists.listNodes[name]->isEnum)
                {
                    string parentRule = parent;
                    if (parent == "start_object.object") 
                    {
                        ruleName = "root";
                        parentRule = "root";
                    }

                    if (parent != "start_object.object")
                    {
                        ruleName = parent;
                    }
                    this->rules[parentRule] += "[";
                    for (auto x : this->graph.lists.listNodes[name]->enumerate)
                    {
                        if (i == 0)
                        {
                            this->rules[parentRule] += x.first;
                            i = 1;
                        }
                        else this->rules[parentRule] += ", " + x.first;
                    }
                    this->rules[parentRule] += "]";
                }
                else
                {
                    this->rules[ruleName] += "[";
                    for (ListEdge *aux = this->graph.lists.listNodes[name]->edges; aux != NULL; aux = aux->next)
                    {
                        if (aux->edge->relationship != "sibling")
                        {
                            string nameEdge = "";
                            if (aux->edge->destiny->name == "") nameEdge = name; 
                            else nameEdge = aux->edge->destiny->name;

                            string edgePath = "";
                            if (parent == "start_object.object") {
                                edgePath = path + "." + "array." + aux->edge->destiny->name;
                            } else {
                                edgePath = path + "." + aux->edge->destiny->name;
                            }
                            
                            if (i == 0)
                            {
                                if (aux->edge->destiny->type == "object")
                                {
                                    this->rules[ruleName] += nameEdge + ".object";
                                    writeStructure(nameEdge, name, true, edgePath, true, getMinAndMax);
                                }
                                else this->rules[ruleName] += aux->edge->destiny->type;
                                i = 1;
                            }
                            else
                            {
                                if (aux->edge->destiny->type == "object")
                                {
                                    this->rules[ruleName] += ", " + nameEdge + ".object";
                                    writeStructure(nameEdge, name, true, edgePath, true, getMinAndMax);
                                }
                                else this->rules[ruleName] += ", " + aux->edge->destiny->type;
                            }
                        }
                    }
                    this->rules[ruleName] += "]\n";
                }
            }
            if (parent == "start_object.object")
            {
                parent = "root";
            }

            if (!this->graph.lists.listPaths[path]->read 
                && this->graph.lists.listPaths[path]->nodes.size() > 1)
                writeRepeatedKeys(path, name, pathBefore, parent, canPrint, ruleName, parent, getMinAndMax);
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