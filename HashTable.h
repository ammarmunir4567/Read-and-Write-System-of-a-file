#pragma once
#include"queue .h"
#include <fstream>


class HashTable 
{
private:
    class Lnode {
    public:
        int file_id;
        queue<QNode> q;
        vector<QNode>grantedAccess;  // to keep track of users currrently accessing the file (exit priortiy queue, and pushed in this arr)
        
        int accessType; // can be -1 file is free, 0: read access or 1: write access
        // -1: no access (both queue and  vector above are empty),
        // logic is: if access type = -1, give user access to file because queue is empty and no one is accessing the file
        // if access type = 0, means some user(s) is/are having read access
        // in that case, allow users with high priority in the queue who want to read, and enter them in granted access vector
        Lnode() {
            file_id = 0;
            accessType = -1;
        }
        ~Lnode(){}
        void setter(int val) {
            file_id = val;
            
        }
        void printListNode(typename list<Lnode>::iterator&it)
        {
            cout << "File " << it->file_id << " ... ";
            cout << "Access granted to";
            if (grantedAccess.empty()) cout << " none\n";
            else
            {
                for (unsigned int i = 0; i < grantedAccess.size(); i++)
                {
                    cout << " User " << grantedAccess[i].userID<<",";
                }
                accessType == 0 ? cout << " read" : cout << " write";
                cout << endl;
            }
            if (!q.isEmpty())
            {
                cout << "\tNext User " << q.findMax();
                cout << "\tWaiting " << q.getSize() << " users\n";
            }
            else
            {
                cout << "\tNext none\n";
                cout << "\tWaiting none\n";
            }
            cout << endl;
        }
    };
    
    list <Lnode>* lis;
    int tableSize;
public:
    HashTable(int s)
    {
        tableSize = s;
        lis = new list<Lnode>[tableSize];
    }

    ~HashTable()
    {
        for (int i = 0; i < tableSize; i++)
            if (lis[i].empty() == false)
                while(lis[i].empty()==false)//for (typename list<Lnode>::iterator it = lis[i].begin(); it != lis[i].end(); ++it)
                    lis[i].pop_back();
        delete[]lis;
        lis = nullptr;
    }
    
    int HashFunc(int key) {
        return key % tableSize;
    }
    
    void Insert(int key)
    {
        typename list<Lnode>::iterator it;
        int hash_v = HashFunc(key);
        if (search(it, hash_v, key) == false)
        {
            Lnode L;
            L.file_id = key;
            lis[hash_v].push_back(L);
            cout << "\nFile Id " << key << " Inserted at index H" << hash_v + 1 << ".\n";
        }
        else
            cout << "\nFile Id " << key << " already exists.\n";
    }
    
    bool search(typename list<Lnode>::iterator& it, int index, int toSearch)
    {
        it = lis[index].begin();
        for (; it != lis[index].end(); ++it)
        {
            if (it->file_id == toSearch)
                return true;
        }
        return false;
    }
     
    void fileAccess(int prioritySetting,int userId, int fileId, bool operation)
    {
        QNode queueNode(userId,operation);
        int hash_index = HashFunc(fileId);
        typename list<Lnode>::iterator it;
        bool found = search(it, hash_index, fileId);
        if (!found)
        {
            cout << "\nFile not Found\n";
            return;
        }
        switch (prioritySetting)
        {
        case 0: queueNode.setPriority(it->q.findMin().getPriority() - 1); break; // lowest priority
        case 1: queueNode.setPriority(it->q.findMax().getPriority() + 1); break; // highest priority
        case 2:  // custom priority number
        {
            int temp=0;
            cout << "\nEnter Priority Number (0-100): "; cin >> temp;
            while (temp < 0 || temp>100)
            {
                cout << "Value out of range, enter again: "; cin >> temp;
            }
            queueNode.setPriority(temp); break;
        }
        default:
            queueNode.setPriority(prioritySetting); break;  // only used when loading data from txt file
        }
        if (found)
        {
            if (it->q.isEmpty())// waiting queue empty, next check if someone is accessing the file
            {
                if (it->accessType == -1)
                {
                    it->grantedAccess.push_back(queueNode);
                    it->accessType = operation == 0 ? 0 : 1;
                    cout << "\nUser "<< queueNode.userID<<" granted access to File#"<<fileId<<", as the file was not under use\n";
                }
                else if (it->accessType == 0 && operation == 0)
                {
                    it->grantedAccess.push_back(queueNode);
                    cout << "\nUser " << queueNode.userID << " granted read access to File#" << fileId << ", as file already under read access.\n";
                }
                else
                {
                    it->q.insert(queueNode);
                    cout << "\nCan't access file at the moment, added User " << queueNode.userID << " to waiting queue of File#" << fileId << ".\n";
                }
            }
            else
            {
                it->q.insert(queueNode);
                if (queueNode == it->q.findMax() && it->accessType == 0)
                {
                    it->q.extractMax();
                    it->grantedAccess.push_back(queueNode);
                    // File already being read, user has highest priority and wants to read, so grant access
                    cout << "\nUser " << queueNode.userID << " granted read access to File#"<<fileId<<", as file already under read access\n";
                }
                else
                    cout << "\nAdded User " << queueNode.userID << " to waiting queue of File#"<<fileId<<".\n";
            }
            
        }
       
    }
  
    void Release(int fileId, int user_id)
    {
        int hash_index = HashFunc(fileId);
        typename list<Lnode>::iterator it;
        QNode q1, q2, q3;
        bool foundFile = search(it, hash_index, fileId);
        if (foundFile)
        {
            typename vector<QNode>::iterator it1;
            bool foundUser = false;
            for (it1 = (*it).grantedAccess.begin(); it1 != (*it).grantedAccess.end(); ++it1)
                if ((*it1).userID == user_id)
                {
                    cout << "\nFile released from User " << user_id << " access.\n";
                    (*it).grantedAccess.erase(it1);
                    foundUser = true;
                    break;
                }
            if (foundUser == false)
            {
                cout << "\nNo User id " << user_id << " accessing the file\n"; return;
            }
            // granted access must be empty, for queue users to get access.
            // if a user reading a file is released but granted access not empty means other users are reading
            // the file, and users with max priority are automatically added if file is under read access
            if (it->grantedAccess.empty() == true)
                it->accessType = -1;
            if (it->q.isEmpty() == false)//exclusive right to write
            {
                //it->q->pri(7551);
                //it->q->extractMax(); // commented this why ? see '--(1)' neechay 
                q1 = it->q.findMax();
                if (q1.operationType == 1)
                {
                    cout << "\nAccess given to the highest priority with writing" << endl;
                    cout << "Write Access is given to the user id  " << q1.userID << endl;
                    cout << "priority : " << q1.key << ", Write Access " << endl;
                    if (it->grantedAccess.empty() == true)
                    {
                        it->q.extractMax();    // remove high priority from waiting queue . . .
                        it->grantedAccess.push_back(q1);  // . . . and push into grantedAccess
                        it->accessType = 1;
                    }
                }
                else if (q1.operationType == 0) //if it is read operation
                {                    
                    if (it->accessType == 0 || it->accessType == -1)//if some one is reading the file or it is free we will give it access
                    {
                        it->accessType = 0;//giving reading access
                        // commented the huge block of code below, and used this while loop instead
                        while (q1.operationType == 0&& it->q.isEmpty()==false)  // give read access to all highest priorty users with operation read
                        {
                            it->q.extractMax();
                            it->grantedAccess.push_back(q1);
                            cout << "\nRead Access given to User id " << q1.userID << "   priority key :" << q1.key << endl;
                            q1 = it->q.findMax();   
                        }
                    }//users with read acceess
                }
            }
        }
        else
            cout << "\nFile not Found\n";
    }

    void load(string filename){
        int f_id=0;
        int u_id=0;
        int pri=0;
        char com;
        char status = '\0';
        QNode temp2;
        typename list<Lnode>::iterator it;
        ifstream file;
        file.open(filename);
        if(file.is_open()==true)
        {
            cout<<"File is open"<<endl;
            int lineNo = 1;
            while(file>>f_id)
            {  
                file>>com>>u_id>>com>>pri>>com>>status;
                if (f_id < 5000 || f_id>99999)
                    cout << "\nInput File Id out of range, line#"<<lineNo<<" of input file discarded.\n";
                if (u_id < 0 || u_id>10000)
                    cout << "\nInput User Id out of range, line#" << lineNo << " of input file discarded.\n";
                if (pri < 0 || pri>100)
                    cout << "\nInput Priority out of range, line#" << lineNo << " of input file discarded.\n";
                if(status!='W'&&status!='R')
                    cout << "\nInput Operation type is Invalid, line#" << lineNo << " of input file discarded.\n";
                if ((f_id >= 5000 && f_id <= 99999) && (u_id >= 0 && u_id <= 10000) && (pri >= 0 && pri <= 100)&&(status=='W'||status=='R'))
                {
                    Lnode temp;
                    temp.file_id = f_id;
                    temp2.userID = u_id;
                    temp2.key = pri;
                    if (status == 'W') 
                        temp2.operationType = 1;
                    else 
                        temp2.operationType = 0;
                    int hash_v = HashFunc(temp.file_id);
                    bool found = search(it, hash_v, f_id);
                    if (found == true)   // file already exists
                        fileAccess(pri, u_id, f_id, status);  // give access if possible, else it will be entered into queue
                    else  // file not found, so first push back into the list
                    {
                        lis[hash_v].push_back(temp);
                        fileAccess(pri, u_id, f_id, status);  // load file and give access to user immediately
                    }
                }
                lineNo++;
            }         
            file.close();
        }
        else
            cout<<"File not open"<<endl;
    }
    
    void save()
    {
        ofstream fout("data.txt");
        for (int i = 0; i < tableSize; i++)
            if (lis[i].empty() == false)
                for (typename list<Lnode>::iterator it = lis[i].begin(); it != lis[i].end(); ++it)
                    it->q.printToFileHelp((*it).file_id, fout);
        fout.close();
    }

    bool deleteFile(int id)
    {
        typename list<Lnode>::iterator it;
        int hash_index = HashFunc(id);
        bool found = search(it, hash_index, id);
        if (found)
        {
            if (it->accessType == -1)
            {
                lis[hash_index].erase(it);
                cout << "\nFile deleted.\n";
                return true;
            }
            else
                cout << "\nFile under access, delete failed.\n";
        }
        else
            cout << "\nFile not found.\n";
        return false;
    }

    void printTable()
    {
        cout << "\n*+-+*-*-+*-* Printing Database *+**+**-**\n";
        bool EmptyTable = true;
        for (int i = 0; i < tableSize; i++)
            if (lis[i].empty() == false)
                for (typename list<Lnode>::iterator it = lis[i].begin(); it != lis[i].end(); ++it)
                {
                    EmptyTable = false;
                    cout << "\nH" << i + 1 << " -> ";
                    (*it).printListNode(it);
                }
        if (EmptyTable)
            cout << "Database is Empty";
        cout << "\n*+-+**-*-+*-* Printed Database *+**+**-**\n"; 
    }
    
    void tableMain()
    {
        int x = 1, temp = 0, fid=0, uid=0;
        cout << "*+-+*-*-+*-*+*-*+**+**-**\n";
        cout << "*WELCOME TO THE DATABASE*\n";
        cout << "*+-+*-*-+*-*+*-*+**+**-**\n";
        while (x)
        {
            cout << "\n----------- MENU -----------\n";
            cout << "Press 1:\tPrint Hash table" << endl;
            cout << "Press 2:\tLoad Data" << endl;
            cout << "Press 3:\tInsert a file" << endl;
            cout << "Press 4:\tDelete a file" << endl;
            cout << "Press 5:\tRequest access to a file by a user" << endl;
            cout << "Press 6:\tRelease the file by a user" << endl;
            cout << "Press 0:\tEXIT" << endl;
            cin >> x;
            while (x < 0 || x>6)
            {
                cout << "Invalid input, try again: "; cin >> x;
            }
            cout << "----------------------------\n";
            switch (x)
            {
            case 1: printTable(); break;
            case 2:  // load
            {
                string name = "data.txt";
                load(name);
                printTable();
                break;
            }
            case 3:  // insert
                cout << "\n----------- File Insertion -----------";
                cout << "\nEnter file Id (5000-99999) to insert: "; cin >> temp;
                while (temp < 5000 || temp>99999)
                {
                    cout << "File id should be b/w 5000 - 99999 inclusive, try again: "; cin >> temp;
                }
                Insert(temp);
                cout << "\n--- File Insert Operation Complete ---\n";
                break;
            case 4:
                cout << "\n-------------- File Deletion --------------";
                cout << "\nEnter the file id to delete" << endl;
                cin >> temp;
                deleteFile(temp);
                cout << "\n----- File Deletion Operation Complete ----\n";
                break;
            case 5:
            {
                cout << "\n-------------- File Access --------------";
                int id = 0, pr_ch = 0, pr = 0; bool op = 0;
                cout << "\nEnter User Id (0-10000): "; cin >> id;
                while (id < 0 || id>10000)
                {
                    cout << "User id should be b/w 0 - 10000 inclusive, try again: "; cin >> id;
                }
               
                cout << "Enter:\n0:\tLowest Priority\n1:\tHighest Priority\n2:\tCustom Priority\nYour Choice:\t"; cin >> pr_ch;
                while (pr_ch < 0 || pr_ch>2)
                {
                    cout << "Invalid choice, try again: "; cin >> pr_ch;
                }
                cout << "\nEnter file Id to Access: "; cin >> temp;
                int temp2 = 0;
                cout << "Enter, 0: Read or, 1: Write: "; while (!(cin >> temp2)&&(temp2<0||temp2>1)) { cout << "Invalid choice, enter again: "; }
                op = temp2;
                fileAccess(pr_ch, id,  temp, op);
                cout << "\n---- File Access Operation Complete -----\n";
                break;
            }
            case 6:
                cout << "\n-------------- File Release --------------";
                cout << "\nEnter File Id: "; cin >> fid;
                cout << "Enter User Id: "; cin >> uid;
                Release(fid, uid);
                cout << "\n---- File Release Operation Complete -----\n";
                break;
            }
            cout << endl << endl;
        }
        cout << "\nSaving Database . . .";
        save(); // save table and waiting queues, granted access users will be discarded bcoz bijli chali gai
        cout << "\nDatabase Saved . . . \n\n";
        cout << "*+-+*-*-+*-*+*-*+**+**-**--*+-**\n";
        cout << "*THANKS FOR USING, RATE 5 STARS*\n";
        cout << "*+-+*-*-+*-*+*-*+**+**-**--*+-**\n\n";
    }
};
