#pragma once

#include <string>
#include <vector>

#include "ListNode.hpp"

class ListSerializer
{
public:
    static ListNode *deserializeFromText(const std::string &filename);

    static void serializeToBinary(ListNode *head, const std::string &filename);

    static ListNode *deserializeFromBinary(const std::string &filename);
};
