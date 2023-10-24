#include "main.h"

class imp_res : public Restaurant {
public:
    class Queue {
    public:
        class Node {
        private:
            customer* data;
            Node* next;
            int joinOrder;
            friend class Queue;

        public:
            bool inTable;
            Node(customer* data, int joinOrder, Node* next) {
                this->data = data;
                this->joinOrder = joinOrder;
                this->next = next;
                inTable = 0;
            }
        };

    private:
        int count;
        Node* head;
        Node* tail;
        int timeStamp;

    public:
        Queue()
            : count(0), head(NULL), tail(NULL), timeStamp(0) {}
        ~Queue() {
            while (count) {
                dequeue();
            }
        }

        void dequeue();
        void enqueue(customer* cus) {
            if (!count) {
                head = new Node(cus, timeStamp++, NULL);
                tail = head;
            }
            else if (count == MAXSIZE) return; // queue deny

            else {
                tail->next = new Node(cus, timeStamp++, NULL);
                tail = tail->next;
            }
        }
        Node* front();

        bool findName(string& name) {
            Node* tmp = head;
            while (tmp != NULL) {
                if (tmp->data->name == name) return 1;
                tmp = tmp->next;
            }
            return 0;
        }
    };

    class Table { // circle DLL
    private:
        int count;
        customer* X;

        bool addDirection(int& energy) {
            if (count >= MAXSIZE / 2) {
                int RES = energy - X->energy;
                customer* tmp = X->next;
                while (tmp != X) { // return to X
                    int differ = energy - tmp->energy;
                    if (abs(differ) > abs(RES)) RES = differ; // keep the first max
                    tmp = tmp->next;
                }
                if (RES < 0) return 0;
                else return 1;
            }

            else { // count < MAXSIZE/2
                return (energy >= X->energy);
            }
        }

    public:
        friend class imp_res;
        Table()
            : count(0), X(NULL) {}
        ~Table() {
            while (count) {
                removeAt(0);
            }
        }

        void add(customer* cus) {
            if (!count) {
                cus->next = cus->prev = X;
                X = cus;
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

        void removeAt(int idx);
    };

    Table* table;
    Queue* waitline;
    Queue* timeline;

public:
    imp_res()
        : table(new Table), waitline(new Queue), timeline(new Queue) {}

    void RED(string name, int energy) {
        if (energy == 0 || timeline->findName(name)) return; // deny service

        customer* newCus = new customer(name, energy, NULL, NULL);

        if (table->count == MAXSIZE) waitline->enqueue(newCus); // move to queue

        else table->add(newCus); // get in table
    }

    void BLUE(int num) {
        cout << endl;
    }

    void PURPLE() {
        cout << "purple" << endl;
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
        cout << "light " << num << endl;
    }
};