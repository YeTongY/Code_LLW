#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>

using namespace std;

struct GameEvent{
    string triggerType;
    string triggerValue;
    string scriptPath;
};

#endif // EVENT_H

