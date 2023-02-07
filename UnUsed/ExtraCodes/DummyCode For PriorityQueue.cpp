#include <iostream>
#include <queue>
using namespace std;

class Event
{
public:
    int timeStamp, type;
    Event(int t, int ty)
    {
        timeStamp = t;
        type = ty;
    }
    void operator=(const Event &E1)
    {
        this->timeStamp = E1.timeStamp;
        this->type = E1.type;
    }
};

ostream &operator<<(ostream &out, const Event E)
{
    out << E.timeStamp << " " << E.type;
    return out;
}

class compareTimestamp
{
public:
    bool operator()(Event &E1, Event &E2)
    {
        if (E1.timeStamp > E2.timeStamp)
            return true;
        else
            return false;
    }
};

int main()
{
    Event E1(3, 2), E2(6, 1), E3(2, 0), E4(1, 3);
    priority_queue<Event, vector<Event>, compareTimestamp> EventQueue;

    EventQueue.push(E1);
    EventQueue.push(E2);
    EventQueue.push(E3);
    EventQueue.push(E4);

    while (!EventQueue.empty())
    {
        Event temp = EventQueue.top();
        cout << temp << endl;
        EventQueue.pop();
    }
    return 0;
}
