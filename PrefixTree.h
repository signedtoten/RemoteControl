#pragma once
#include <vector>
#include <string>
#include <stack>
class PrefixTree
{
private:
	class Node {
	public:
		std::vector<Node*>nodes;
		char ch;
		Node* parent = nullptr;
		Node(char ch, Node* parent = nullptr);
	};
public:
	PrefixTree();
	~PrefixTree();
private:
	void run(Node* node, void (PrefixTree::* func)(Node* node));
	void deleteNode(Node* node);
private:
	std::vector<Node*> roots;
public:
	void add(std::string str);
	void add(std::vector<std::string> &vec);
	std::vector<std::string> getWord(std::string str);
};

