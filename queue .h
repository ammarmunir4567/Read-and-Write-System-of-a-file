#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include <list>
#include <iterator>
using namespace std;


struct QNode
{
    int key;
    int userID;
    bool operationType;   // 0: read, 1: write
    QNode()
    {
        userID = 0;
        key = 0;
        operationType = 0;
    }
    QNode(int priority, int id, bool opType)
    {
        key = priority;
        userID = id;
        operationType = opType;
    }
    QNode(int id, bool opType)
    {
        key = 0;
        userID = id;
        operationType = opType;
    }
    QNode(int id)
    {
        userID=id;
        key = 0;
        operationType = 0;
    }
    QNode(const QNode&q)
    {
        *this=q;
    }
    int getPriority() { return key; }
    void setPriority(int p)
    { 
        if (p < 0)   // keep prioerity within range 0-100
            p = 0;
        else if (p > 100)
            p = 100;
        key = p; 
    }
    QNode&operator=(const QNode&q)
    {
        key=q.key;
        userID=q.userID;
        operationType=q.operationType;
        return *this;
    }
    QNode&operator=(int id)
    {
        userID=id;
        return *this;
    }
    QNode& operator=(bool highPriority)
    {
        if (highPriority)
            this->key = 101;
        else
            this->key = -1;
        return *this;
    }
    friend ofstream& operator<<(ofstream& out, const QNode& node)
    {
        out << node.userID << ',' << node.key << ',';
        node.operationType ? out << 'W' : out << 'R';
        out << endl;
        return out;
    }
    friend istream&operator>>(istream &in, QNode&q)
    {
        in>>q.key;
        in>>q.operationType;
        return in;
    }
    friend ostream& operator<<(ostream& out, const QNode&q)
    {
        out << q.userID;
        //out<<"\nKEY: "<<q.key<<" User Id: "<<q.userID<<" Operation: ";
        q.operationType ? out<<", write\n" : out<<", read\n";
        return out;
    } 
    bool operator==(QNode q)
    {
        if (q.key == this->key && q.operationType == this->operationType && q.userID == this->userID)
            return true;
        else
            return false;
    }
    bool operator==(int n)
    {
        if (this->key == n)
            return true;
        else
            return false;
    }
    bool operator<(QNode&q)
    {
        if(key<=q.key)
            return true;
        else
            return false;
    }
    bool operator>(QNode&q)
    {
        if(key>q.key)
            return true;
        else
            return false;
    }
};


template<class k>
class queue
{
    vector<k> queueMaxHeap;
    int size;
    k min;
public:
    queue()
    {
        bool highPriority = true;
        size=0;
        min = highPriority; // max priority=100, so initialize min with 101 (using overloaded = operator with bool type)
    }
    queue(int s)
    {
        bool highPriority = true;  
        size=abs(s);
        min = highPriority;  // max priority=100, so initialize min with 101 (using overloaded = operator with bool type)
    }
    queue(const queue&q)
    {
        *this = q;
    }
    void operator=(const queue&q)
    {
        if(this->size!=0)
            this->~queue();
        this->size=q.size;
        this->min = q.min;
        for(int i=0;i<size;i++)
            this->queueMaxHeap.push_back(q.queueMaxHeap[i]);
    }
    ~queue()
    {
        while(!queueMaxHeap.empty())
        {
            queueMaxHeap.pop_back();
            size--;
        }
    }
    vector<k> buildQueue(vector<k>&arr, int n)  // array and its size
    {
        while (!queueMaxHeap.empty())
        {
            queueMaxHeap.pop_back();
            size--;
        }
        for(int i=0;i<n;i++)
        {
            queueMaxHeap.push_back(arr[i]);
            if (arr[i] < min)
                min = arr[i];
            size++;
        }
        int index=(size/2)-1;
        for(int i=index;i>=0;i--)
            heapify(i);
        return queueMaxHeap;
    }
    void insert(k node)
    {
        queueMaxHeap.push_back(node);
        if (node < min)
            min = node;
        size++;
        shiftUp(size-1);    // heapify
    }
    k extractMax()
    {
        if (!isEmpty())
        {
            k max = queueMaxHeap[0];
            queueMaxHeap[0] = queueMaxHeap[size - 1];
            queueMaxHeap.pop_back();
            size--;
            shiftDown(0); // heapify root if needed
            return max;
        }
        else
        {
            k m = false;   // return lowest priority (dummy), -1 if queue is empty --- using overloaded = operator
            return m;
        }
    }
    k findMax()
    {
        if(size>0)
            return queueMaxHeap[0];
        k m = true;   // return highest priority (dummy), -1 if queue is empty --- using overloaded = operator
        return m;
    }
    k findMin()
    {
        return min;
        //k min =queueMaxHeap[size/2];
        //for(int i=(size/2)+1;i<size;i++)
        //{
        //    if(queueMaxHeap[i]<min)
        //        min=queueMaxHeap[i];
        //}
    }
    bool isEmpty() { return (size == 0) ? true : false; }
    int getSize() { return size; }
    // helper functions
    friend ostream&operator<<(ostream&out, const queue&q)
    {
        for(int i=0;i<q.size;i++)
            out<<q.queueMaxHeap[i];
        out<<endl;
        if(q.size<=0)
            out<<"Empty Queue\n";
        return out;
    }
    void swap(k&n1, k&n2)
    {
        k temp=n1;
        n1=n2;
        n2=temp;
    }
    
    void pri(int fileId){
        
        for (int i = 0; i < size; i++)
        {
            cout << fileId << ","; cout << queueMaxHeap[i];
        }
        
    }
    void printToFileHelp(int fileId, ofstream&fout)
    {
        for (int i = 0; i < size; i++)
        {
            fout << fileId << ","; fout << queueMaxHeap[i];
        }
    }
    void heapify(int i)
    {
        int largest = i,left = 2 * i + 1, right= 2 * i + 2;
        if(left<size&&queueMaxHeap[left]>queueMaxHeap[largest])
            largest=left;
        if(right<size&&queueMaxHeap[right]>queueMaxHeap[largest])
            largest=right;
        if(largest!=i)
        {
            swap(queueMaxHeap[largest], queueMaxHeap[i]);
            heapify(largest);
        }
    }
    // swap node with parent
    void shiftUp(int index)
    {
        while (index > 0 && queueMaxHeap[(index-1)/2] < queueMaxHeap[index])
        {
            // Swap with parent
            swap(queueMaxHeap[((index-1)/2)], queueMaxHeap[index]);
            index = ((index-1)/2);
        }
    }
    // swaps node with its child if the child is greater
    void shiftDown(int index)
    {
        int maxIndex = index;
        // Left Child:
        int l = ((2 * index) + 1);
        if (l < size && queueMaxHeap[l] > queueMaxHeap[maxIndex])
            maxIndex = l;
        // Right Child:
        int r = ((2 * index) + 2);
        if (r < size && queueMaxHeap[r] > queueMaxHeap[maxIndex])
            maxIndex = r;
        // If i not same as maxIndex
        if (index != maxIndex)
        {
            swap(queueMaxHeap[index], queueMaxHeap[maxIndex]);
            shiftDown(maxIndex);
        }
    }
};

