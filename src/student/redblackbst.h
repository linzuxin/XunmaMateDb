#pragma once
#include <cstdint>

#define RED true
#define BLACK false
#define BST 0
#define TD234 1
#define BU23 2

struct bstNodeValue
{
  uint64_t version;
  uint64_t field;
};

struct bstNodeKey
{
  uint64_t key;
};

struct bstNode
{
  uint64_t key;
  uint64_t value;
  struct bstNode *left;
  struct bstNode *right;
  bool color;
  int N;
  int height;
};

typedef bstNode redBlackBSTNode;

redBlackBSTNode * test();
int ipl();
int size();
bool check();
int sizeRed();
int rootRank();
int height();
int heightB();
void deleteNodeMax();
void deleteNodeMin();
uint64_t max();
uint64_t min();

uint64_t get(uint64_t key);
void put(uint64_t key, uint64_t value);
void deleteNode(uint64_t key);
bool contains(uint64_t key);

bool isBST();
bool isBST(redBlackBSTNode *node, uint64_t min, uint64_t max);
bool is234();
bool is234(redBlackBSTNode *node);
bool isBalanced();
bool isBalanced(redBlackBSTNode *node, int black);
bool isRed(redBlackBSTNode *node);

int ipl(redBlackBSTNode *node);
int sizeRed(redBlackBSTNode *node);

int height(redBlackBSTNode *node);
int rootRank(redBlackBSTNode *root);
int size(redBlackBSTNode *node);
int sizeRedBlackBST(redBlackBSTNode *node);
void colorFlip(redBlackBSTNode *node);
uint64_t max(redBlackBSTNode *node);
uint64_t min(redBlackBSTNode *node);
uint64_t get(redBlackBSTNode *node, uint64_t key);
redBlackBSTNode *insert(redBlackBSTNode *node, uint64_t key, uint64_t value);
redBlackBSTNode *deleteNodeMin(redBlackBSTNode *node);
redBlackBSTNode *deleteNodeMax(redBlackBSTNode *node);
redBlackBSTNode *deleteNode(redBlackBSTNode *node, uint64_t key);
redBlackBSTNode *rotateLeft(redBlackBSTNode *node);
redBlackBSTNode *rotateRight(redBlackBSTNode *node);
redBlackBSTNode *moveRedLeft(redBlackBSTNode *node);
redBlackBSTNode *moveRedRight(redBlackBSTNode *node);
redBlackBSTNode *fixUp(redBlackBSTNode *node);
redBlackBSTNode *setN(redBlackBSTNode *node);
