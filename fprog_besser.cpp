#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <memory>
#include "doctest.h"  // Assuming doctest.h is available for testing

// Define a Node structure for the Red-Black Tree
template<typename T>
struct Node {
    T data;
    std::shared_ptr<Node<T>> left;
    std::shared_ptr<Node<T>> right;
    std::shared_ptr<Node<T>> parent;
    bool isRed;

    Node(T value) : data(value), left(nullptr), right(nullptr), parent(nullptr), isRed(true) {}
};

// Define the Red-Black Tree Data Structure using functional programming principles
template<typename T>
class PersistentRBTree {
public:
    PersistentRBTree() : root(nullptr) {}

    PersistentRBTree add(T value) const {
        auto newRoot = insert(root, value);
        newRoot->isRed = false;
        return PersistentRBTree(newRoot);
    }

    std::vector<T> getSortedElements() const {
        return inOrderTraversal(root);
    }

private:
    std::shared_ptr<Node<T>> root;

    PersistentRBTree(std::shared_ptr<Node<T>> root) : root(root) {}

    std::shared_ptr<Node<T>> insert(std::shared_ptr<Node<T>> node, T value) const {
        if (!node) {
            return std::make_shared<Node<T>>(value);
        }

        if (value < node->data) {
            auto newNode = std::make_shared<Node<T>>(*node);
            newNode->left = insert(node->left, value);
            newNode->left->parent = newNode;
            return balance(newNode);
        } else if (value > node->data) {
            auto newNode = std::make_shared<Node<T>>(*node);
            newNode->right = insert(node->right, value);
            newNode->right->parent = newNode;
            return balance(newNode);
        }
        // Duplicate values are ignored to maintain unique words
        return node;
    }

    std::vector<T> inOrderTraversal(std::shared_ptr<Node<T>> node) const {
        if (!node) {
            return {};
        }
        std::vector<T> result;
        auto leftResult = inOrderTraversal(node->left);
        result.insert(result.end(), leftResult.begin(), leftResult.end());
        result.push_back(node->data);
        auto rightResult = inOrderTraversal(node->right);
        result.insert(result.end(), rightResult.begin(), rightResult.end());
        return result;
    }

    std::shared_ptr<Node<T>> balance(std::shared_ptr<Node<T>> node) const {
        if (isRed(node->right) && !isRed(node->left)) {
            node = rotateLeft(node);
        }
        if (isRed(node->left) && isRed(node->left->left)) {
            node = rotateRight(node);
        }
        if (isRed(node->left) && isRed(node->right)) {
            flipColors(node);
        }
        return node;
    }

    bool isRed(std::shared_ptr<Node<T>> node) const {
        return node != nullptr && node->isRed;
    }

    std::shared_ptr<Node<T>> rotateLeft(std::shared_ptr<Node<T>> node) const {
        auto newNode = std::make_shared<Node<T>>(*node->right);
        auto newParent = std::make_shared<Node<T>>(*node);

        newParent->right = newNode->left;
        if (newNode->left) {
            newNode->left->parent = newParent;
        }
        newNode->left = newParent;
        newParent->parent = newNode;

        newNode->parent = node->parent;
        newNode->isRed = node->isRed;
        newParent->isRed = true;

        return newNode;
    }

    std::shared_ptr<Node<T>> rotateRight(std::shared_ptr<Node<T>> node) const {
        auto newNode = std::make_shared<Node<T>>(*node->left);
        auto newParent = std::make_shared<Node<T>>(*node);

        newParent->left = newNode->right;
        if (newNode->right) {
            newNode->right->parent = newParent;
        }
        newNode->right = newParent;
        newParent->parent = newNode;

        newNode->parent = node->parent;
        newNode->isRed = node->isRed;
        newParent->isRed = true;

        return newNode;
    }

    void flipColors(std::shared_ptr<Node<T>> node) const {
        node->isRed = !node->isRed;
        if (node->left) node->left->isRed = !node->left->isRed;
        if (node->right) node->right->isRed = !node->right->isRed;
    }
};

// Function to read the text file and return content as a string
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to tokenize the text into words
std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    std::stringstream ss(text);
    while (ss >> word) {
        word.erase(std::remove_if(word.begin(), word.end(), [](unsigned char c) { return std::ispunct(c) || std::isdigit(c); }), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty()) {
            words.push_back(word);
        }
    }
    return words;
}

// Function to write the sorted list of words to a file
void writeToFile(const std::string& filename, const std::vector<std::string>& words) {
    std::ofstream file(filename);
    for (const auto& word : words) {
        file << word << "\n";
    }
}

int main() {
    // Step 1: Read the text file
    std::string text = readFile("war_and_peace.txt");

    // Step 2: Tokenize the text
    std::vector<std::string> words = tokenize(text);

    // Step 3: Insert each unique word into the Red-Black Tree
    PersistentRBTree<std::string> rbTree;
    for (const auto& word : words) {
        rbTree = rbTree.add(word);
    }

    // Step 4: Traverse the tree to get the sorted list of words
    std::vector<std::string> sortedWords = rbTree.getSortedElements();

    // Step 5: Write the sorted list to "output.txt"
    writeToFile("output.txt", sortedWords);

    return 0;
}

// Unit tests using doctest
TEST_CASE("Tokenize function") {
    std::string text = "Hello, World! 123";
    std::vector<std::string> words = tokenize(text);
    CHECK(words == std::vector<std::string>{"hello", "world"});
}

TEST_CASE("PersistentRBTree insertion and traversal") {
    PersistentRBTree<int> rbTree;
    rbTree = rbTree.add(5);
    rbTree = rbTree.add(3);
    rbTree = rbTree.add(7);
    rbTree = rbTree.add(3);  // Duplicate

    std::vector<int> sortedElements = rbTree.getSortedElements();
    CHECK(sortedElements == std::vector<int>{3, 5, 7});
}
