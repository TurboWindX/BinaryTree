#include <iostream>
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes <3
//============================================================================

// forward declarations
double strToDouble(string str, char ch);

struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

// define a structure to hold bid information

class BinarySearchTree {

private:
    
    // Internal structure for tree node
    struct Node {
        Bid bid;
        // create the key for the given bid
        unsigned key;
        
        Node* left = nullptr;
        Node* right = nullptr;

        unsigned int Hash(int key) {
            unsigned bucketIndex;
            bucketIndex = key % 7;
            return bucketIndex;
        }

        Node(Bid aBid) {
            this->bid = aBid;
            key = Hash(atoi(this->bid.bidId.c_str()));
        }
    };
    Node* root;
    void addNode(Node* parentNode, Node* newNode);
    void inOrder(Node* node);
    void postOrder(Node* node);
    void preOrder(Node* node);
    Node* minValue(Node* node) {
        Node* current = root;
        while (current && current->left != NULL)
            current = current->left;
        return current;
    };

    void removeNode(Node* node, string bidId)
    {
        unsigned key = atoi(bidId.c_str()) % 7;
        Node* parentNode = nullptr;
        while (node != nullptr) {
            if (node->key == key) {
                if (node->bid.bidId == bidId) {
                    int c = 0;
                    // IS A LEAF
                    if (node->left == nullptr && node->right == nullptr) {
                        if (parentNode != nullptr) {
                            parentNode->left = nullptr;
                        }
                        free(node);
                        return;
                    }

                    //LEFT NOT NULL
                    //RIGHT NULL
                    if(node->left != nullptr && node->right == nullptr){
                        if (parentNode != nullptr) {
                            parentNode->left = node->left;
                        }
                        free(node);
                        return;
                    }
                    //RIGHT NOT NULL
                    //LEFT NULL
                    if (node->left == nullptr && node->right != nullptr) {
                        if (parentNode != nullptr) {
                            parentNode->left = node->right;
                        }
                        free(node);
                        return;
                    }
                    
                    //RIGHT NOT NULL
                    //LEFT NOT NULL
                    //node to be deleted has two children
                    //1.Find inorder successor of node
                    //2.Copy content of said successor to the node
                    //3.Delete inorder successor
                    if (node->left != nullptr && node->right != nullptr) {

                        parentNode->right = minValue(node->right);
                        parentNode->right->left = node->left;
                        return;
                    }
                }
                else {//good key but not good bid
                    parentNode = node;
                    node = node->left;
                }
            }
            else if (node->key < key) {
                parentNode = node;
                node = node->left;
            }
            else if (node->key > key){
                parentNode = node;
                node = node->right;
            }
            
        }    
    }

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void InOrder();
    void PostOrder();
    void PreOrder();
    void Insert(Bid bid);
    void Remove(string bidId);
    Bid Search(string bidId);
};

/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
    // FixMe (1): initialize housekeeping variables
    //root is equal to nullptr
    this->root = nullptr;
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {
    // recurse from root deleting every node

}

void BinarySearchTree::InOrder() {
    inOrder(root);
}

void BinarySearchTree::PostOrder() {
    postOrder(root);
}

void BinarySearchTree::PreOrder() {
    preOrder(root);
}

void BinarySearchTree::Insert(Bid bid) {
    Node* newNode = new Node(bid);
    if (root == nullptr) {
        root = newNode;
    }
    else {
        addNode(root,newNode);
    }
}

void BinarySearchTree::Remove(string bidId) {
    removeNode(root,bidId);
}

Bid BinarySearchTree::Search(string bidId) {
    Node* node = root; //init for search
    Bid bid; //init
    unsigned key = atoi(bidId.c_str()) % 7;  //hash bidid
    while (node != nullptr) {//looper
        if (node->key == key) {//found key
            if (node->bid.bidId == bidId) {//good bidid
                return node->bid;//return bid
            }
        }
        if (node->key <= key) {
            node = node->left;
        }
        else {
            node = node->right;
        } 
    }

    return bid;//no bid found :(
}



void BinarySearchTree::addNode(Node* parentNode, Node* newNode) {
    if (parentNode->key <= newNode->key) {
        if (parentNode->left == nullptr) {
            parentNode->left = newNode;
        }
        else {
            addNode(parentNode->left, newNode);
        }
    }
    if(parentNode->key > newNode->key) {
        if (parentNode->right == nullptr) {
            parentNode->right = newNode;
        }
        else {
            addNode(parentNode->right, newNode);
        }
    }
}

void BinarySearchTree::inOrder(Node* node) {
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->key << ":" << node->bid.bidId << endl;
        inOrder(node->right);
    }
}

void BinarySearchTree::postOrder(Node* node) {
    if (node != nullptr) {
        postOrder(node->left);
        postOrder(node->right);
        cout << node->key << endl;
    }

}

void BinarySearchTree::preOrder(Node* node) { 
    if (node != nullptr) {
        cout << node->key << endl;
        preOrder(node->left);
        preOrder(node->right);
    }
}


// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void loadBids(string csvPath, BinarySearchTree* bst) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            bst->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, bidKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        bidKey = "98005";
        break;
    case 3:
        csvPath = argv[1];
        bidKey = argv[2];
        break;
    default:
        csvPath = "res/eBid_Monthly_Sales_Dec_2016.csv";
        bidKey = "98005";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a binary search tree to hold all bids
    BinarySearchTree* bst = nullptr;

    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            bst = new BinarySearchTree();

            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            loadBids(csvPath, bst);

            //cout << bst->Size() << " bids read" << endl;

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bst->InOrder();
            break;

        case 3:
            ticks = clock();

            bid = bst->Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
            	cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 4:
            bst->Remove(bidKey);
            break;
        }
    }

    cout << "Good bye." << endl;

	return 0;
}
