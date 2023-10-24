#include "main.h"

class imp_res : public Restaurant {
public:
    class Queue {
    public:
        class Node {
        public:
            customer* data;
            Node* next;
            int joinOrder;

        public:
            bool inTable;
            Node(customer* data, int joinOrder, Node* next, bool inTable) {
                this->data = data;
                this->joinOrder = joinOrder;
                this->next = next;
                this->inTable = inTable;
            }

            ~Node() {}

            bool operator>(Node* other) { // for shell sort compare
                if (abs(other->data->energy) == abs(this->data->energy)) {
                    return this->joinOrder < other->joinOrder;
                } else return abs(this->data->energy) > abs(other->data->energy);
            }
        };

    private:
        int count;
        Node* head;
        Node* tail;
        int timeStamp;
        friend class imp_res;

        void swap(Node* node1, Node* node2) {
            customer* tmpCus = node1->data;
            node1->data = node2->data;
            node2->data = tmpCus;

            int tmpOrder = node1->joinOrder;
            node1->joinOrder = node2->joinOrder;
            node2->joinOrder = tmpOrder;
        }

        int segmentIS(int n, int k) {
            int swapCnt = 0;
            for (int i = k; i < n; i += k) {
                for (int j = i; j >= k; j -= k) {
                    Node* pre_jNode = getAt(j - k);
                    Node* jNode = getAt(j);

                    if (jNode->operator>(pre_jNode)) {
                        // swap nodes
                        swap(pre_jNode, jNode);
                        swapCnt++;
                    } else break;
                }
            }
            return swapCnt;
        }

    public:
        Queue()
            : count(0), head(nullptr), tail(nullptr), timeStamp(0) {}
        ~Queue() {
            while (count) {
                dequeue();
            }
        }

        int getSize() {
            return count;
        }

        void dequeue() {
            if (count) {
                Node* tmp = head;
                head = head->next;
                delete tmp;
                count--;
                if (!count) tail = nullptr;
            }
        }

        void removeItem(customer* item) {
            if (head->data == item) return dequeue();
            Node* tmp = head;
            for (int i = 1; i < count; i++) {
                if (tmp->next->data == item) {
                    if (i == count - 1) tail = tmp;
                    Node* toDel = tmp->next;
                    tmp->next = tmp->next->next;
                    toDel->next = nullptr;
                    delete toDel;
                    count--;
                    break;
                }
            }
        }

        void enqueue(customer* cus, bool inTable = false) {
            if (!count) {
                head = new Node(cus, timeStamp++, nullptr, inTable);
                tail = head;
            } else if (count == MAXSIZE) return; // queue deny

            else {
                tail->next = new Node(cus, timeStamp++, nullptr, inTable);
                tail = tail->next;
            }
        }

        Node* front() {
            return head;
        }

        bool findName(string& name) {
            Node* tmp = head;
            while (tmp != nullptr) {
                if (tmp->data->name == name) return 1;
                tmp = tmp->next;
            }
            return 0;
        }

        Node* findItem(customer* cus) {
            Node* tmp = head;
            while (tmp != nullptr) {
                if (tmp->data == cus) return tmp;
                tmp = tmp->next;
            }
            return nullptr;
        }

        Node* getAt(int idx) {
            if (idx < 0 || idx >= count) return nullptr; // out of range
            Node* tmp = head;
            while (idx--) {
                tmp = tmp->next;
            }
            return tmp;
        }

        // return number of swap
        int shellSort(int n) {
            int swapCnt = 0;
            for (int i = n / 2; i > 2; i = i / 2) {
                for (int j = 0; j < i; j++) {
                    swapCnt += segmentIS(n - j, i);
                }
            }
            swapCnt += segmentIS(n, 1);
            return swapCnt;
        }

        void printInfo() {
            if (!count) return;
            Node* tmp = head;
            while (tmp != nullptr) {
                cout << tmp->data->name << "-" << tmp->data->energy << "\n";
                tmp = tmp->next;
            }
        }
    };

    class Table { // circle DLL
    private:
        int count;
        customer* X;
        friend class imp_res;

        bool addDirection(int& energy) {
            // ATTENTION
            if (count >= MAXSIZE / 2) {
                int RES = energy - X->energy;
                customer* head = X;
                customer* tmp = head->next;
                while (tmp != head) { // return to X
                    int differ = energy - tmp->energy;
                    if (abs(differ) > abs(RES)) {
                        RES = differ; // keep the first max
                        X = tmp;
                    }
                    tmp = tmp->next;
                }
                return (RES >= 0); // clockwise
            }

            else { // count < MAXSIZE/2
                return (energy >= X->energy);
            }
        }

    public:
        Table()
            : count(0), X(nullptr) {}
        ~Table() {
            while (count) {
                remove(X);
            }
        }

        void add(customer* cus) {
            if (!count) {
                X = cus;
                cus->next = cus->prev = X;
            }

            else {
                int clockWise = addDirection(cus->energy);
                if (clockWise) {
                    cus->prev = X;
                    cus->next = X->next;
                    X->next = cus;

                    X = X->next;
                    X->next->prev = X;
                } else {
                    cus->prev = X->prev;
                    cus->next = X;
                    X->prev = cus;

                    X = X->prev;
                    X->prev->next = X;
                }
            }

            count++;
        }

        void add(string& name, int& energy) {
            if (!count) {
                X = new customer(name, energy, X, X);
            }

            else {
                int clockWise = addDirection(energy);
                if (clockWise) {
                    X->next = new customer(name, energy, X, X->next);
                    X = X->next;
                    X->next->prev = X;
                } else {
                    X->prev = new customer(name, energy, X->prev, X);
                    X = X->prev;
                    X->prev->next = X;
                }
            }

            count++;
        }

        void remove(customer* cus) {
            if (!count) return;

            customer* tmp = X;
            for (int i = 0; i < count; i++) {
                if (tmp == cus) break;
                tmp = tmp->next;
            }
            if (tmp->energy > 0) X = X->next;
            else X = X->prev;

            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            tmp->prev = nullptr;
            tmp->next = nullptr;
            delete tmp;

            if (!count) X = nullptr;
            count--;
        }

        void printInfo(int clockWise) {
            if (!count) return;
            cout << X->name << "-" << X->energy << "\n";
            if (clockWise) {
                customer* tmp = X->next;
                while (tmp != X) {
                    cout << tmp->name << "-" << tmp->energy << "\n";
                    tmp = tmp->next;
                }
            } else {
                customer* tmp = X->prev;
                while (tmp != X) {
                    cout << tmp->name << "-" << tmp->energy << "\n";
                    tmp = tmp->prev;
                }
            }
        }
    };

    Table* table;
    Queue* waitline;
    Queue* timeline;

public:
    imp_res()
        : table(new Table), waitline(new Queue), timeline(new Queue) {}

    void RED(string name, int energy) {
        if (energy == 0 || timeline->findName(name) || timeline->getSize() == 2 * MAXSIZE) return; // deny service

        customer* newCus = new customer(name, energy, nullptr, nullptr);

        if (table->count == MAXSIZE) {
            waitline->enqueue(newCus); // move to queue
            timeline->enqueue(newCus);
        }

        else {
            table->add(newCus); // get in table
            timeline->enqueue(newCus, true);
        }
    }

    void BLUE(int num) {
        if (!num) return;
        Queue::Node* tmp = timeline->head;
        while (num && tmp != nullptr) {
            if (tmp->inTable) {
                customer* toDel = tmp->data;
                tmp = tmp->next;
                table->remove(toDel);
                timeline->removeItem(toDel);
                delete toDel;
                num--;
            } else tmp = tmp->next;
        }

        // add cus to table
        while (table->count < MAXSIZE && waitline->count) {
            customer* toMove = waitline->front()->data;
            table->add(toMove);
            waitline->dequeue();
            (timeline->findItem(toMove))->inTable = 1; // moved to table
        }
    }

    void PURPLE() {
        if (!waitline->count) return;
        Queue::Node* tmp = waitline->head;
        Queue::Node* maxE_N = tmp;
        int maxE_Id = 0;
        for (int i = 1; i < waitline->count; i++) {
            if (abs(tmp->data->energy) == abs(maxE_N->data->energy)) {
                if (tmp->joinOrder > maxE_N->joinOrder) {
                    maxE_N = tmp;
                    maxE_Id = i;
                }
            } else if (abs(tmp->data->energy) > abs(maxE_N->data->energy)) {
                maxE_N = tmp;
                maxE_Id = i;
            }
            tmp = tmp->next;
        }

        BLUE(waitline->shellSort(maxE_Id + 1) % MAXSIZE);
    }

    void REVERSAL() {
        cout << "reversal" << endl;
    }
    void UNLIMITED_VOID() {
        cout << "unlimited_void" << endl;
    }
    void DOMAIN_EXPANSION() {
        cout << "domain_expansion" << endl;
    }
    void LIGHT(int num) {
        if (num == 0) waitline->printInfo();
        else if (num > 0) table->printInfo(true);
        else table->printInfo(false);
    }
};