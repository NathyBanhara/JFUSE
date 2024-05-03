#include <iostream>
#include "graph.hpp"
#include "queue.hpp"
#include <bits/stdc++.h>
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

Node *createNode (string type, string name, string path)
{
    Node *newOne = new Node;
    newOne->counter = 1;
    newOne->type = type;
    newOne->name = name;
    newOne->isEnum = true;
    newOne->edges = NULL;
    newOne->hasTaggedUnion = false;
    newOne->isSomeonesTaggedUnion = false;
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

void Graph::saveEnum(QueueStruct *q)
{
    if (q->name == "") return;
    if (q->value == "") return;

    string name = q->name + "." + q->type;

    if (this->lists.listNodes[name]->isEnum)
    {
        if (!this->lists.listNodes[name]->enumerate[q->value])
        {
            if (this->lists.listNodes[name]->enumerate.size() + 1 > MAXENUM)
            {
                this->lists.listNodes[name]->isEnum = false;
                this->lists.listNodes[name]->enumerate.clear();
            }
            else
                this->lists.listNodes[name]->enumerate[q->value] = 1;
        }
        else this->lists.listNodes[name]->enumerate[q->value]++;
    }
}

void Graph::saveEnum2(string name, string value)
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

void Graph::insertNode(string name, string type, string path, Node *parent, Node *parentParent)
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
        return;
    }
    else
    {
        Node *newOne = createNode(type, name, path);
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

/*
void Graph::printGraph()
{
    for (auto x : this->lists.listNodes)
    {
        if (x.second->isSomeonesTaggedUnion)
            cout << x.second->name << "\n";
        /*
        cout << x.first << " ";
        for (auto z : x.second->enumerate)
        {
            cout << z.first << " ";
        }
        cout << "\n";*/
    //}
//}
/*
void Graph::printGraph()
{
    for (auto x : this->lists.listNodes)
    {
        cout << x.first << " -> " << flush;
        if (x.second->isEnum)
        {
            for (auto y : x.second->enumerate)
            {
                cout << y.first << ", ";
            }
        }
        cout << "\n";
    }
}*/

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
                    if (x.second->name == "profile_background_image_url")
                    {
                        cout << "profile_background_image_url" << "\n";
                    }
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
    if (required/childrenCount > MINREQUIREDTOBEATUPLE)
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
/*
//Com probabilidade
void Graph::printGraph()
{
    for (int i = 0; i < numVertexes; i++)
    {
        if (listNodes[i] == NULL) return;
        cout << listNodes[i]->name << "-> " << listNodes[i]->type;
        printf(" -> %d: ", listNodes[i]->counter);
        for (Edge *aux = listNodes[i]->next; aux != NULL; aux = aux->next)
        {
            if (aux->index == -1) 
                cout << " " << " -> " << aux->probability << ";";
            else
                cout << listNodes[aux->index]->name << " -> " << listNodes[aux->index]->type << " -> " << aux->probability << ";";
        }
        printf("\n");
    }
}
*/
/*
void Graph::printArrays()
{
    for (auto x : this->lists.listNodes)
    {
        if (x.second->type == "array")
        {
            if (x.second->checkArrayInfo)
            {
                for (auto y : x.second->arrayInfo)
                {
                    cout << x.first << " " << y.first << " -> " << y.second->counter << ": ";
                    for (auto w : y.second->set)
                    {
                        cout << ", " << w.first;
                    }
                    cout << "\n";
                }
            }
        }
    }
}*/

void Graph::percentualEdge()
{
    for (auto x : this->lists.listEdges)
    {
        x.second->probability = ((float)x.second->destiny->counter/x.second->origin->counter);
    }
}
/*
void Graph::checkObject(Node *node)
{
    int childrenCount = 0;
    int required = 0;
    unordered_map<string, int> types; //type, counter

    for (ListEdge *aux = node->edges; aux != NULL; aux = aux->next)
    {
        if (aux->edge->relationship == "parent")
        {
            childrenCount++;
            if (aux->edge->probability > MAXOPTIONAL)
            {
                required++;
                if (types[aux->edge->destiny->type]) types[aux->edge->destiny->type]++;
                else types[aux->edge->destiny->type] = 1;
            }
        }
    }
    if (required/childrenCount > MINREQUIREDTOBEATUPLE)
    {
        return;
    }
    else
    {
        string biggestTypeOccurance = "";
        for (auto x : types)
        {
            if (biggestTypeOccurance == "")
            {
                biggestTypeOccurance = x.first;
            }
            else if (x.second > types[biggestTypeOccurance])
            {
                biggestTypeOccurance = x.first;
            }
        }

        int differentTypes = 0;

        for (auto x : types)
        {
            if (x.first != biggestTypeOccurance)
            {
                differentTypes += x.second;
            }
        }

        // CALCULA A DIFERENÇA?
    }
}
*/

void Graph::freeGraph()
{
}