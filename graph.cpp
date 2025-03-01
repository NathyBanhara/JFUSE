#include <iostream>
#include "graph.hpp"
#include "queue.hpp"
#include <bits/stdc++.h>
#include <string>
#include <regex>
#include "definitions/define.hpp"

using namespace std;

void Graph::getTaggedUnions(string name, string type, string parent, Queue q)
{
    if (name != "start_object")
    {
        if (name == "") name = parent;
        name = name + "." + type;

        q.pop();

        if (type != "object" && type != "array") return;

        if (type == "array")
        {
            while (q.front()->name != "end_array")
            {
                getTaggedUnions(q.front()->name, q.front()->type, name, q);
            }
            q.pop();
        }

        if (type == "object")
        {
            Queue siblingQueue = Queue();

            while (q.front()->name != "end_object")
            {
                siblingQueue.push(siblingQueue.createStruct(q.front()->name, q.front()->type, q.front()->value));
                getTaggedUnions(q.front()->name, q.front()->type, name, q);
            }
            
            saveTaggedUnion(siblingQueue);
            siblingQueue.freeQueue();
            q.pop();
        }
    }
    else
    {
        q.pop();
        Queue siblingQueue = Queue();
        while (q.front()->name != "end_object")
        {
            siblingQueue.push(siblingQueue.createStruct(q.front()->name, q.front()->type, q.front()->value));
            getTaggedUnions(q.front()->name, q.front()->type, " ", q);
        }

        saveTaggedUnion(siblingQueue);
            siblingQueue.freeQueue();
            q.pop();
    }
}

void Graph::checkArrayEnum(QueueStruct *q, vector<string> enums, string name)
{
    if (q->type == "object" || q->type == "array")
        this->lists.listNodes[name]->isEnum = false;
    else if (enums.size()+1 > MAXENUMARRAY)
        this->lists.listNodes[name]->isEnum = false;
}

Node *createNode (string type, string name, string path, bool canBeKey, double value)
{
    Node *newOne = new Node;
    newOne->counter = 1;
    newOne->type = type;
    newOne->name = name;
    newOne->isEnum = true;
    newOne->edges = NULL;
    newOne->hasTaggedUnion = false;
    newOne->isSomeonesTaggedUnion = false;
    newOne->key = canBeKey;
    newOne->min = value;
    newOne->max = value;
    if (path != "") newOne->paths[path] = 1;
    if (newOne->type != "array" && newOne->type != "object")
    {
        newOne->isCollection= false;
        newOne->isTuple = false;
    }
    if (newOne->type == "array")
    {
        newOne->isCollection = true;
        newOne->collectionWasModified = false;
        newOne->isTuple = false;
    }
    if (newOne->type == "object")
    {
        newOne->isCollection = false;
        newOne->isTuple = true;
    }
    
    return newOne;
}

void Graph::saveEnum(string name, string value)
{
    if (this->lists.listNodes[name]->isEnum)
    {
        if (!this->lists.listNodes[name]->enumerate[value])
        {
            if (this->lists.listNodes[name]->enumerate.size() + 1 > MAXENUM)
            {
                this->lists.listNodes[name]->isEnum = false;
                this->lists.listNodes[name]->enumerate.clear();
            }
            else
                this->lists.listNodes[name]->enumerate[value] = 1;
        }
        else this->lists.listNodes[name]->enumerate[value]++;
    }
}

void Graph::saveEnumArray(string name, vector<string> enums)
{
    if (enums.size() == 0) return;

    for (auto x : enums)
    {
        if (!this->lists.listNodes[name]->enumerate[x])
        {
            this->lists.listNodes[name]->enumerate[x] = 1;
        }
        else
        {
            this->lists.listNodes[name]->enumerate[x]++;
        }
    } 
}

void Graph::saveTaggedUnion(Queue siblingQueue)
{
    while (!siblingQueue.empty())
    {
        string name = siblingQueue.front()->name + "." + siblingQueue.front()->type;

        if (siblingQueue.front()->name == "end_object" || siblingQueue.front()->name == "end_array")
        {
            siblingQueue.pop();
            continue;
        }
        
        if (siblingQueue.front()->name != "" 
            && this->lists.listNodes[name]->isEnum 
            && this->lists.listNodes[name]->enumerate.size() > 1
            && siblingQueue.front()->next != NULL)
        {
            string sibling = siblingQueue.front()->next->name + "." + siblingQueue.front()->next->type;
            insertEdge(name, sibling, "sibling", siblingQueue.front()->value);
        }
        
        siblingQueue.pop();
    }
}

void Graph::checkTaggedUnions()
{
    for (auto x : this->lists.listNodes)
    {
        if (!x.second->hasTaggedUnion)
        {
            continue;
        }

        bool hasMoreThanOneDestiny = false;
        string destiny = "";

        for (auto y : x.second->taggedUnions)
        {   
            if (destiny == "")
            {
                destiny = y;
            } 
            else if (y != destiny) {
                hasMoreThanOneDestiny = true;
                break;
            }
        }

        if (!hasMoreThanOneDestiny)
        {
            x.second->hasTaggedUnion = false;
            x.second->taggedUnions.clear();
            this->lists.listNodes[destiny]->isSomeonesTaggedUnion = false;
        }
    }
}

void Graph::insertNode(
    string name, 
    string type, 
    string path, 
    Node *parent, 
    Node *parentParent, 
    bool canBeKey, 
    double value)
{
    if (parent != NULL && parent->type == "object" && parentParent->type == "array")
    {
        if (!this->lists.listNodes[name + "." + type])
        {
            parentParent->isCollection = false;
        }
    }
    setPath(path, name + "." + type);
    if (this->lists.listNodes[name + "." + type])
    {
        this->lists.listNodes[name + "." + type]->counter++;
        if (path != "")
        {
            if (!this->lists.listNodes[name + "." + type]->paths[path]) 
                this->lists.listNodes[name + "." + type]->paths[path] = 1;
            else this->lists.listNodes[name + "." + type]->paths[path]++;
        }

        if (!canBeKey)
        {
            this->lists.listNodes[name + "." + type]->key = false;
        }
       
        if ((type == "integer" || type == "double") && this->lists.listNodes[name + "." + type]->min > value)
        {
            this->lists.listNodes[name + "." + type]->min = value;
        } 
        
        if ((type == "integer" || type == "double") && this->lists.listNodes[name + "." + type]->max < value)
        {
            this->lists.listNodes[name + "." + type]->max = value;
        }

        return;
    }
    else
    {
        Node *newOne = createNode(type, name, path, canBeKey, value);

        name = name + "." + type;
        this->lists.listNodes[name] = newOne;
        return;
    }
}

void Graph::setPath(string path, string name)
{
    if (path == "") return;
    else if (this->lists.listPaths[path]) 
    {
        this->lists.listPaths[path]->nodes.insert(name);
    }
    else
    {
        Path *newOne = new Path();
        newOne->read = false;
        newOne->nodes.insert(name);
        this->lists.listPaths[path] = newOne;
    }
}

Edge *createEdge(Node *origin, Node *destiny, string relationshipType)
{
    Edge *e = new Edge;
    e->counter = 1;
    e->origin = origin;
    e->destiny = destiny;
    e->relationship = relationshipType;
    
    if (relationshipType == "sibling") e->isTaggedUnion = true;
    else e->isTaggedUnion = false;

    return e;
}

void Graph::insertEdge(string origin, string destiny, string relationshipType, string value)
{   
    if (this->lists.listNodes[origin]->edges == NULL)
    {
        this->lists.listNodes[origin]->edges = new ListEdge;
        Edge *e = createEdge(this->lists.listNodes[origin], this->lists.listNodes[destiny], relationshipType);
        this->lists.listNodes[origin]->edges->edge = e;
        this->lists.listNodes[origin]->edges->next = NULL;
        this->lists.listEdges[{destiny, origin}] = e;
        
        if (relationshipType == "sibling")
        {
            this->lists.listNodes[origin]->hasTaggedUnion = true;
            this->lists.listNodes[origin]->taggedUnions.push_back(destiny);
            this->lists.listEdges[{destiny, origin}]->taggedUnion[value] = 1;
            this->lists.listNodes[destiny]->isSomeonesTaggedUnion = true;
        }

        return;
    }
    else if (this->lists.listEdges[{destiny, origin}])
    {
        this->lists.listEdges[{destiny, origin}]->counter++;
        if (relationshipType == "sibling" && this->lists.listEdges[{destiny, origin}]->isTaggedUnion)
        {
            if (this->lists.listEdges[{destiny, origin}]->taggedUnion.size() + 1 <= MAXTAGGEDUNION)
            {
                if (this->lists.listEdges[{destiny, origin}]->taggedUnion[value])
                    this->lists.listEdges[{destiny, origin}]->taggedUnion[value]++;
                else
                {
                    this->lists.listEdges[{destiny, origin}]->taggedUnion[value] = 1;
                }
            }
            else
            {
                this->lists.listNodes[origin]->hasTaggedUnion = false;
                this->lists.listNodes[origin]->taggedUnions.clear();
                this->lists.listEdges[{destiny, origin}]->isTaggedUnion = false;
                this->lists.listNodes[destiny]->isSomeonesTaggedUnion = false;
            }
        }
        return;
    }
    else
    {
        Edge *e = createEdge(this->lists.listNodes[origin], this->lists.listNodes[destiny], relationshipType);
        ListEdge *aux = this->lists.listNodes[origin]->edges;
        ListEdge *newOne = new ListEdge;
        newOne->edge = e;
        newOne->next = aux;
        this->lists.listNodes[origin]->edges = newOne;
        this->lists.listEdges[{destiny, origin}] = e;
        this->lists.listNodes[origin]->taggedUnions.push_back(destiny);
        if (relationshipType == "sibling") this->lists.listNodes[destiny]->isSomeonesTaggedUnion = true;
        return;
    }
}

void Graph::printGraph()
{
    for (auto x : this->lists.listNodes)
    {
        cout << x.second->name << ": ";
        for (ListEdge *aux = x.second->edges; aux != NULL; aux = aux->next)
        {
            cout << aux->edge->destiny->name;
            cout << ", ";
        }
        cout << "\n";
    }
}

bool isEnumAString(string enumerate)
{
    try
    {
        int value = std::stoi(enumerate);
        return true;
    }
    catch (const std::invalid_argument& e)
    {
        return false;
    }
    catch (const std::out_of_range& e) {
        return false;
    }
}

void Graph::checkEnums()
{
    for (auto x : this->lists.listNodes)
    {
        if (x.second->type == "array" && x.second->counter < MAXENUMARRAY*2)
        {
            x.second->isEnum = false;
        }
        if (x.second->type != "array" && x.second->counter < MAXENUM*2)
        {
            x.second->isEnum = false;
        }
        if (x.second->isEnum)
        {
            if (x.second->enumerate.size() == 0) x.second->isEnum = false;
            else
            {
                int stringCounter = 0;
                int totalCounter = 0;

                int emptySpaceCount = 0;

                for (auto e : x.second->enumerate)
                {
                    if (e.first == "" || e.first == " ")
                    {
                        emptySpaceCount++;
                    }
                    if (isEnumAString(e.first))
                    {
                        stringCounter++;
                    }

                    totalCounter++;
                }

                if (x.second->isEnum == false)
                {
                    continue;
                }
                
                if (emptySpaceCount == x.second->enumerate.size())
                {
                    x.second->isEnum = false;
                    continue;;
                }

                bool isStringADominantType = stringCounter/totalCounter >= DOMINANTENUMTYPE;

                set<string> toRemove;

                for (auto e : x.second->enumerate)
                {
                    if (isStringADominantType && !isEnumAString(e.first))
                    {
                        toRemove.insert(e.first);
                    }

                    if (!isStringADominantType && isEnumAString(e.first))
                    {
                        toRemove.insert(e.first);
                    }
                }

                for (auto e : toRemove)
                {
                    x.second->enumerate.erase(e);
                }
            }
        }
    }
}

void Graph::checkKeys(string jsonFilename)
{
    for (auto x : this->lists.listNodes)
    {
        if (x.second->isEnum) 
        {
            x.second->key = false;
        }
        else if (x.second->key) 
        {
            std::string key = x.second->name;

            std::string command =
                "grep -oE '\"" + key + "\": *\"[^\"]*\"' " + jsonFilename +
                " | sed 's/\"" + key + "\": *\"//g' | sed 's/\"//g'; " +
                "grep -oE '\"" + key + "\": *[0-9]+' " + jsonFilename +
                " | sed 's/\"" + key + "\": *//g'; " +
                "grep -oE '\"" + key + "\": *[0-9]+\\.[0-9]+' " + jsonFilename +
                " | sed 's/\"" + key + "\": *//g'";

            FILE *file = popen(command.c_str(), "r");
            if (file == nullptr) {
                continue;
            }

            std::unordered_set<std::string> values;
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), file) != nullptr) {
                string value(buffer);
                size_t pos = value.find_last_not_of("\n");
                if (pos != std::string::npos) {
                    value.erase(pos + 1);
                }
                
                if (values.find(value) != values.end())
                {
                    x.second->key = false;
                    break;
                } 
                else {
                    values.insert(value);
                }
            }

            fclose(file);
        }
    }
}

void Graph::checkTupleAndCollection()
{
    for (auto x : this->lists.listNodes)
    {
        if (x.second->type == "object" && x.second->name != "start_object")
        {
            checkObject(x.second);
        }
        else if (x.second->type == "array")
        {
            checkArray(x.second);
        }
    }
}

void Graph::setCollectionStructure(queue<std::string> children, Node *node)
{
    int total = children.size();

    if (node->collectionWasModified)
    {
        return;
    }

    if (node->arrayInfo[total])
    {
        checkIfItsEqual(children, node);
    }
    else
    {
        insertStructure(children, node);
    }
}

void Graph::checkIfItsEqual(queue<std::string> children, Node *node)
{
    int total = children.size();
    unordered_map<string, int> newStructure;
    
    while (!children.empty())
    {
        if (newStructure[children.front()])
        {
            newStructure[children.front()]++;
        }
        else
        {
            newStructure[children.front()] = 1;
        }
        children.pop();
    }

    Collection *sourceStructure = node->arrayInfo[total];

    if (newStructure.size() != sourceStructure->structure.size())
    {
        sourceStructure->valid = false;
        return;
    }

    for (auto x : newStructure)
    {
        if (!sourceStructure->structure[x.first])
        {
            sourceStructure->valid = false;
        }

        int sourceSetCounter = sourceStructure->structure[x.first];

        if (sourceSetCounter != x.second)
        {
            sourceStructure->valid = false;
            return;
        }
    }
    sourceStructure->counter++;
}

void Graph::insertStructure(queue<std::string> children, Node *node)
{
    int total = children.size();
    unordered_map<string, int> newStructure;

    while (!children.empty())
    {
        if (newStructure[children.front()])
        {
            newStructure[children.front()]++;
        }
        else
        {
            newStructure[children.front()] = 1;
        }
        children.pop();
    }

    Collection *newOne = new Collection();
    newOne->size = children.size();
    newOne->counter = 1;
    newOne->valid = true;
    newOne->structure = newStructure;

    node->arrayInfo[total] = newOne;
}

void Graph::checkArray(Node *node)
{
    if (node->arrayInfo.size() == 0 || node->isEnum || node->isCollection == false)
    {
        return;
    }

    int arrayCounter = node->counter;
    
    for (auto x : node->arrayInfo)
    {
        int sizeCounter = x.second->counter;

        if ((sizeCounter/arrayCounter) > MINFORANARRAYTOBEATUPLE)
        {
            if (x.second->valid)
            {
                node->isTuple = true;

                ListEdge *aux = node->edges;
                
                while (aux != NULL)
                {
                    ListEdge *lastEdge = aux;
                    aux = aux->next;
                    free(lastEdge);
                }

                node->edges = NULL;
                node->tuple = x.second;
            }
            return;
        }
    }
}

void Graph::checkObject(Node *node)
{
    double childrenCount = 0;
    double required = 0;
    set<string> taggedUnions;

    for (ListEdge *aux = node->edges; aux != NULL; aux = aux->next)
    {
        if (aux->edge->destiny->hasTaggedUnion) {
            for (auto x : aux->edge->destiny->taggedUnions)
            {
                taggedUnions.insert(x);
            }
        }
    }

    for (ListEdge *aux = node->edges; aux != NULL; aux = aux->next)
    {
        if (aux->edge->relationship == "parent")
        {
            childrenCount++;
            if (aux->edge->probability > MAXOPTIONAL 
                || taggedUnions.find(aux->edge->destiny->name + "." + aux->edge->destiny->type) != taggedUnions.end())
            {
                required++;
            }
        }
    }
    taggedUnions.clear();
    if (required/childrenCount >= MINREQUIREDTOBEATUPLE)
    {
        return;
    }
    else
    {
        node->isCollection = true;
    }
}

void Graph::checkArrayType()
{
    for (auto x : this->lists.listNodes)
    {
        if (x.second->type == "array" && !x.second->isEnum)
        {
            vector<string> edgesNotArrayType;
            bool hasOnlyOneType = false;
            double counterTotalElementOcurrancess = 0;

            for (ListEdge *aux = x.second->edges; aux != NULL; aux = aux->next)
            {
                counterTotalElementOcurrancess += aux->edge->counter;
            }

            for (ListEdge *aux = x.second->edges; aux != NULL; aux = aux->next)
            {
                double perOccurance = ((double)aux->edge->counter/counterTotalElementOcurrancess);

                if (perOccurance <= ARRAYTYPE)
                {
                    edgesNotArrayType.push_back(aux->edge->destiny->type);
                }
                else
                {
                    hasOnlyOneType = true;
                }
            }

            if (!hasOnlyOneType) continue;
            ListEdge *aux = x.second->edges;
            ListEdge *last = NULL;
            bool found = false;

            while (aux != NULL)
            {
                for (auto z : edgesNotArrayType)
                {
                    if (aux->edge->destiny->type == z)
                    {
                        if (last == NULL)
                        {
                            x.second->edges = aux->next;
                        }
                        else
                        {
                            last->next = aux->next;
                        }
                        
                        free(aux);
                        aux = x.second->edges;
                        found = true;
                    }
                }
                if (!found) aux = aux->next;
                found = false;
                last = aux;
            }
        }
    }
}

void Graph::printTaggedUnions()
{
    for (auto x : this->lists.listEdges)
    {
        if (x.second->taggedUnion.size() > 0)
        {
            for (auto z : x.second->taggedUnion)
            {
                cout << x.first.node << " -> " << z.first << " -> " << x.first.edge;
                cout << "\n";
            }
            cout << "\n";
        }
    }
}

void Graph::percentualEdge()
{
    for (auto x : this->lists.listEdges)
    {
        x.second->probability = ((float)x.second->destiny->counter/x.second->origin->counter);
    }
}