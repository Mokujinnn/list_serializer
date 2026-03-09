#include <iostream>

#include "ListSerializer.hpp"

void free_list(ListNode *head)
{
    ListNode *current = head;
    while (current != nullptr)
    {
        ListNode *next = current->next;
        delete current;
        current = next;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <text filename>\n";
        return 1;
    }

    ListNode *head = nullptr;

    try
    {
        head = ListSerializer::deserializeFromText(argv[1]);

        ListSerializer::serializeToBinary(head, "outlet.out");

        auto *restored = ListSerializer::deserializeFromBinary("outlet.out");

        std::cout << "Serializaton and deserialization successful\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        free_list(head);
        return 1;
    }

    free_list(head);
    return 0;
}