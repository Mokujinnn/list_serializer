#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

#include "ListSerializer.hpp"

#define MAX_DATA_LENGTH 1000

namespace
{
    struct NodeHeader
    {
        std::uint32_t data_length;
        std::int32_t rand_index;
    };

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

    nodes_data_t read_data_from_text_file(const std::string &filename)
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

    ListNode *create_node(const std::string &data)
    {
        return new ListNode(data);
    }

    void set_prev_and_next_pointers(const std::vector<ListNode *> &nodes)
    {
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
    }

    void delete_nodes(const std::vector<ListNode *> &nodes)
    {
        for (auto node : nodes)
        {
            delete node;
        }
    }

    void set_rand_pointers(const std::vector<ListNode *> &nodes, const nodes_data_t &nodes_data)
    {
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            int rand_index = nodes_data[i].second;
            if (rand_index >= 0 && rand_index < static_cast<int>(nodes.size()))
            {
                nodes[i]->rand = nodes[rand_index];
            }
            else if (rand_index != -1)
            {
                delete_nodes(nodes);
                throw std::runtime_error("Invalid rand index: " + std::to_string(rand_index));
            }
        }
    }

    ListNode *create_nodes(const nodes_data_t &nodes_data)
    {
        std::vector<ListNode *> nodes;
        nodes.reserve(nodes_data.size());

        for (const auto &item : nodes_data)
        {
            nodes.push_back(create_node(item.first));
        }

        set_prev_and_next_pointers(nodes);

        set_rand_pointers(nodes, nodes_data);

        return nodes[0];
    }

    int getNodeIndex(ListNode *node, const std::vector<ListNode *> &nodes)
    {
        if (node == nullptr)
            return -1;

        for (size_t i = 0; i < nodes.size(); ++i)
        {
            if (nodes[i] == node)
            {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    std::vector<ListNode *> create_vector(ListNode *head)
    {
        std::vector<ListNode *> nodes;
        ListNode *current = head;
        while (current != nullptr)
        {
            nodes.push_back(current);
            current = current->next;
        }

        return nodes;
    }

    void write_empty_file(const std::string &filename)
    {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Cannot create output file: " + filename);
        }
        uint32_t node_count = 0;
        file.write(reinterpret_cast<const char *>(&node_count), sizeof(node_count));
    }

    void write_to_binary_file(const std::string &filename, const std::vector<ListNode *> &nodes)
    {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Cannot create output file: " + filename);
        }

        std::uint32_t node_count = static_cast<std::uint32_t>(nodes.size());
        file.write(reinterpret_cast<const char *>(&node_count), sizeof(node_count));

        for (size_t i = 0; i < nodes.size(); ++i)
        {
            ListNode *node = nodes[i];

            int rand_index = getNodeIndex(node->rand, nodes);

            NodeHeader header;
            header.data_length = node->data.size();
            header.rand_index = rand_index;

            file.write(reinterpret_cast<const char *>(&header), sizeof(header));
            file.write(node->data.c_str(), node->data.size());
        }
    }

    NodeHeader read_header_from_binary_file(std::ifstream &file)
    {
        NodeHeader header;
        file.read(reinterpret_cast<char *>(&header), sizeof(header));

        if (file.eof())
        {
            throw std::runtime_error("Unexpected end of file");
        }

        if (header.data_length > MAX_DATA_LENGTH)
        {
            throw std::runtime_error("Data length exceeds maximum allowed");
        }

        return header;
    }

    std::string read_data_from_binary_file(std::ifstream &file, std::uint32_t data_length)
    {
        std::string data;
        data.resize(data_length);
        file.read(&data[0], data_length);

        return data;
    }

    void read_nodes_from_binary_file(std::ifstream &file, nodes_data_t &nodes_data, uint32_t node_count)
    {
        for (uint32_t i = 0; i < node_count; ++i)
        {
            NodeHeader header = read_header_from_binary_file(file);

            std::string data = read_data_from_binary_file(file, header.data_length);

            nodes_data.emplace_back(std::move(data), header.rand_index);
        }
    }

    nodes_data_t read_from_binary_file(std::ifstream &file, uint32_t node_count)
    {
        nodes_data_t nodes_data;
        nodes_data.reserve(node_count);

        read_nodes_from_binary_file(file, nodes_data, node_count);

        return nodes_data;
    }

} // namespace

ListNode *ListSerializer::deserializeFromText(const std::string &filename)
{
    auto nodes_data = read_data_from_text_file(filename);

    if (nodes_data.empty())
    {
        return nullptr;
    }

    auto *head = create_nodes(nodes_data);
    return head;
}

void ListSerializer::serializeToBinary(ListNode *head, const std::string &filename)
{
    if (head == nullptr)
    {
        write_empty_file(filename);
        return;
    }

    auto nodes = create_vector(head);
    write_to_binary_file(filename, nodes);
}

ListNode *ListSerializer::deserializeFromBinary(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open binary file: " + filename);
    }

    uint32_t node_count;
    file.read(reinterpret_cast<char *>(&node_count), sizeof(node_count));

    if (node_count == 0 || file.eof())
    {
        return nullptr;
    }

    auto nodes_data = read_from_binary_file(file, node_count);
    auto *head = create_nodes(nodes_data);

    return head;
}
