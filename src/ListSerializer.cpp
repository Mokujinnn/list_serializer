#include <fstream>
#include <vector>
#include <string>

#include "ListSerializer.hpp"

namespace
{
    using nodes_data_t = std::vector<std::pair<std::string, int>>;

    void parse_line(nodes_data_t &nodes, std::string &line, int line_number)
    {
        size_t delimiter_pos = line.find(';');
        if (delimiter_pos == std::string::npos)
        {
            throw std::runtime_error("Invalid line format in line " + std::to_string(line_number) + ": " + line);
        }

        std::string data = line.substr(0, delimiter_pos);
        std::string rand_index_str = line.substr(delimiter_pos + 1);

        try
        {
            int rand_index = std::stoi(rand_index_str);
            nodes.emplace_back(data, rand_index);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Invalid rand_index in line: " + line);
        }
    }

    nodes_data_t read_data_from_file(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("Cannot open input file: " + filename);
        }

        nodes_data_t nodes;
        std::string line;
        int line_number = 0;

        while (std::getline(file, line))
        {
            line_number++;
            if (line.empty())
                continue;

            parse_line(nodes, line, line_number);
        }

        return nodes;
    }

    ListNode *create_nodes(const nodes_data_t &nodes_data)
    {
        std::vector<ListNode *> nodes;
        nodes.reserve(nodes_data.size());

        for (const auto &item : nodes_data)
        {
            nodes.push_back(new ListNode(item.first));
        }

        for (size_t i = 0; i < nodes.size(); ++i)
        {
            if (i > 0)
            {
                nodes[i]->prev = nodes[i - 1];
            }
            if (i < nodes.size() - 1)
            {
                nodes[i]->next = nodes[i + 1];
            }
        }

        for (size_t i = 0; i < nodes.size(); ++i)
        {
            int rand_index = nodes_data[i].second;
            if (rand_index >= 0 && rand_index < static_cast<int>(nodes.size()))
            {
                nodes[i]->rand = nodes[rand_index];
            }
            else if (rand_index != -1)
            {
                for (auto node : nodes)
                {
                    delete node;
                }
                throw std::runtime_error("Invalid rand index: " + std::to_string(rand_index));
            }
        }

        return nodes[0];
    }

} // namespace

ListNode *ListSerializer::deserializeFromText(const std::string &filename)
{
    auto nodes_data = read_data_from_file(filename);

    if (nodes_data.empty())
    {
        return nullptr;
    }

    auto *head = create_nodes(nodes_data);
    return head;
}

void ListSerializer::serializeToBinary(ListNode *head, const std::string &filename)
{
}

ListNode *ListSerializer::deserializeFromBinary(const std::string &filename)
{
    return nullptr;
}
