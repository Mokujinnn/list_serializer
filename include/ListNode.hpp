#pragma once

#include <string>

struct ListNode
{
    ListNode *prev = nullptr;
    ListNode *next = nullptr;
    ListNode *rand = nullptr;
    std::string data;
};
