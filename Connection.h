#pragma once
#include "ProcessingNode.h"

struct Connection {
    ProcessingNode* source;
    ProcessingNode* destination;
};