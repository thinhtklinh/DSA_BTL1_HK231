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

        void swap(Node* node1, Node* node2) {
            customer* tmpCus = node1->data;
            node1->data = node2->data;
            node2->data = tmpCus;

            int tmpOrder = node1->joinOrder;
            node1->joinOrder = node2->joinOrder;
            node2->joinOrder = tmpOrder;
        }

        int segmentIS(int startIdx, int n, int k) {
            int swapCnt = 0;
            for (int i = k; i < n; i += k) {
                for (int j = i; j >= k; j -= k) {
                    Node* pre_jNode = getAt(startIdx + j - k);
                    Node* jNode = getAt(startIdx + j);

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
        Queue() : count(0), head(nullptr), tail(nullptr), timeStamp(0) {}
        ~Queue() {}

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
                    // toDel->data = nullptr; // ??
                    delete toDel;
                    count--;
                    break;
                }
                else tmp = tmp->next;
            }
        }

        void enqueue(customer* cus, bool inTable = false) {
            if (!count) {
                head = new Node(cus, timeStamp++, nullptr, inTable);
                tail = head;
            }

            else {
                tail->next = new Node(cus, timeStamp++, nullptr, inTable);
                tail = tail->next;
            }
            count++;
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
                    swapCnt += segmentIS(j, n - j, i);
                }
            }
            swapCnt += segmentIS(0, n, 1);
            return swapCnt;
        }

        void domainExpansion(Node* p, imp_res* restaurant, int sign) {
            if (p == nullptr) return;
            customer* toDel;
            if (sign * p->data->energy > 0) {
                toDel = p->data;
                if (p->inTable) {
                    restaurant->table->remove(toDel);
                } else {
                    restaurant->waitline->removeItem(toDel);
                }
                p = p->next;
                removeItem(toDel); // remove in timeline
                domainExpansion(p, restaurant, sign);
                toDel->print();
                delete toDel;
            } else domainExpansion(p->next, restaurant, sign);
        }

        void printInfo() {
            if (!count) return;
            Node* tmp = head;
            while (tmp != nullptr) {
                tmp->data->print();
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
        Table() : count(0), X(nullptr) {}
        ~Table() {}

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
            
            if (cus->energy > 0) X = cus->next;
            else X = cus->prev;

            cus->prev->next = cus->next;
            cus->next->prev = cus->prev;
            cus->prev = nullptr;
            cus->next = nullptr;
            // delete tmp; isolate but not free yet

            count--;
            if (!count) X = nullptr;
        }

        void reverse(int sign) {
            customer* head = X;
            customer* tail = head->next;
            int cnt = 2;
            customer* saveInit = X;

            while (cnt <= count && head != tail) {
                if (sign * head->energy < 0) {
                    head = head->prev;
                    cnt++;
                }

                else if (sign * tail->energy < 0) {
                    tail = tail->next;
                    cnt++;
                }

                else {
                    customer* tmp = head->next;
                    head->next = tail->next;
                    tail->next = tmp;

                    head->next->prev = head;
                    tail->next->prev = tail;

                    tmp = head->prev;
                    head->prev = tail->prev;
                    tail->prev = tmp;

                    head->prev->next = head;
                    tail->prev->next = tail;

                    tmp = head;
                    head = tail;
                    tail = tmp;

                    if (head == X) X = tail;
                    else if (tail == X) X = head;

                    head = head->prev;
                    tail = tail->next;
                    cnt += 2;
                }
            }

            if (sign < 0) reverse(-sign);
            X = saveInit;
        }

        void printInfo(int clockWise) {
            if (!count) return;
            customer* tmp = X;
            for (int i = 0; i < count; i++) {
                tmp->print();
                if (clockWise) tmp = tmp->next;
                else tmp = tmp->prev;
            }
        }
    };

    Table* table;
    Queue* waitline;
    Queue* timeline;

public:
    imp_res()
        : table(new Table), waitline(new Queue), timeline(new Queue) {}

    ~imp_res() {
        // loop timeline: remove
        while (timeline->getSize()) {
            timeline->removeItem(timeline->front()->data);
        }

        // loop waitline: remove, free
        while (waitline->getSize()) {
            customer* toDel  = waitline->front()->data;
            waitline->removeItem(toDel);
            delete toDel;
        }

        // loop table: remove, free
        while (table->count) {
            customer* toDel = table->X;
            table->remove(toDel);
            delete toDel;
        }

        delete table;
        delete waitline;
        delete timeline;
    }

    void RED(string name, int energy) {
        if (energy == 0 || timeline->findName(name) || waitline->getSize() == MAXSIZE) return; // deny service

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
        Queue::Node* tmp = timeline->front();
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
        while (table->count < MAXSIZE && waitline->getSize()) {
            customer* toMove = waitline->front()->data;
            table->add(toMove);
            waitline->dequeue();
            (timeline->findItem(toMove))->inTable = 1; // moved to table
        }
    }

    void PURPLE() {
        if (!waitline->getSize()) return;
        Queue::Node* tmp = waitline->front();
        Queue::Node* maxE_N = tmp;
        int maxE_Id = 0;
        tmp = tmp->next;
        for (int i = 1; i < waitline->getSize(); i++) {
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
        if (table->count <= 2) return;
        // reverse negative
        table->reverse(-1);
    }

    void UNLIMITED_VOID() {
        if (table->count < 4) return;
        int minSum = INT32_MAX;
        int maxLen = 0;
        int tmpSum;
        customer* pI = table->X;
        customer *start, *end;

        for (int i = 1; i <= table->count; i++) {
            customer* pJ = pI;
            tmpSum = 0;
            for (int j = 1; j <= table->count; j++) {
                tmpSum += pJ->energy;
                if (j >= 4) {
                    if (tmpSum == minSum) {
                        if (maxLen <= j) {
                            maxLen = j;
                            start = pI;
                            end = pJ;
                        }
                    } else if (tmpSum < minSum) {
                        minSum = tmpSum;
                        maxLen = j;
                        start = pI;
                        end = pJ;
                    }
                }
                pJ = pJ->next;
            }
            pI = pI->next;
        }

        customer* tmp = start->next;
        customer* minCus = start;
        for (int i = 1; i < maxLen; i++) {
            if (tmp->energy < minCus->energy) minCus = tmp;
            tmp = tmp->next;
        }

        tmp = minCus;
        for (int i = 0; i < maxLen; i++) {
            tmp->print();
            if (tmp == end) tmp = start;
            else tmp = tmp->next;
        }
    }

    void DOMAIN_EXPANSION() {
        if (!timeline->getSize()) return;

        int Epos = 0;
        int Eneg = 0;
        Queue::Node* tmp = timeline->front();
        while (tmp != nullptr) {
            if (tmp->data->energy > 0) Epos += tmp->data->energy;
            else Eneg += tmp->data->energy;
            tmp = tmp->next;
        }

        // remove OL
        if (Epos >= abs(Eneg)) timeline->domainExpansion(timeline->front(), this, -1);

        // remove CTS
        else timeline->domainExpansion(timeline->front(), this, 1);

        // add cus to table
        while (table->count < MAXSIZE && waitline->getSize()) {
            customer* toMove = waitline->front()->data;
            table->add(toMove);
            waitline->dequeue();
            (timeline->findItem(toMove))->inTable = 1; // moved to table
        }
    }

    void LIGHT(int num) {
        if (num == 0) waitline->printInfo();
        else if (num > 0) table->printInfo(true);
        else table->printInfo(false);
    }
};