#ifndef QUEUE_HPP

#define QUEUE_HPP

#include <iostream>

using namespace std;

struct QueueStruct
{
    string name, type;
    string value;
    QueueStruct *next;
};

struct QueueList
{
    QueueStruct *first;
    QueueStruct *last;
};

class Queue
{
    public:
        QueueList *queue;

        Queue();
        QueueStruct *createStruct(string name, string type, string value);
        QueueStruct *front();
        void pop();
        int empty();
        void push(QueueStruct *new_one);
        int size();
        void freeQueue();
        void printQueue();
};

#endif