#pragma once
#include <cstdint>

#define RED true
#define BLACK false
#define BST 0
#define TD234 1
#define BU23 2

struct BstValue
{
  uint64_t version;
  uint64_t field;
};

typedef uint64_t BstNodeKey;
typedef BstValue* BstNodeValue;

struct BstNode
{
  BstNodeKey key;
  BstNodeValue value;
  struct BstNode *left;
  struct BstNode *right;
  bool color;
  int N;
  int height;
};

typedef BstNode RedBlackBSTNode;

RedBlackBSTNode *test();
int ipl();
int size();
bool check();
int sizeRed();
int rootRank();
int height();
int heightB();
void deleteNodeMax();
void deleteNodeMin();
BstNodeKey max();
BstNodeKey min();

BstNodeValue get(BstNodeKey key);
void put(BstNodeKey key, BstNodeValue value);
void deleteNode(BstNodeKey key);
bool contains(BstNodeKey key);

bool eq(BstNodeKey a, BstNodeKey b);
bool less(BstNodeKey a, BstNodeKey b);

bool isBST();
bool isBST(RedBlackBSTNode *node, uint64_t min, uint64_t max);
bool is234();
bool is234(RedBlackBSTNode *node);
bool isBalanced();
bool isBalanced(RedBlackBSTNode *node, int black);
bool isRed(RedBlackBSTNode *node);

int ipl(RedBlackBSTNode *node);
int sizeRed(RedBlackBSTNode *node);

int height(RedBlackBSTNode *node);
int rootRank(RedBlackBSTNode *root);
int size(RedBlackBSTNode *node);
int sizeRedBlackBST(RedBlackBSTNode *node);
void colorFlip(RedBlackBSTNode *node);
BstNodeKey max(RedBlackBSTNode *node);
BstNodeKey min(RedBlackBSTNode *node);
BstNodeValue get(RedBlackBSTNode *node, BstNodeKey key);
RedBlackBSTNode *insert(RedBlackBSTNode *node, BstNodeKey key, BstNodeValue value);
RedBlackBSTNode *deleteNodeMin(RedBlackBSTNode *node);
RedBlackBSTNode *deleteNodeMax(RedBlackBSTNode *node);
RedBlackBSTNode *deleteNode(RedBlackBSTNode *node, BstNodeKey key);
RedBlackBSTNode *rotateLeft(RedBlackBSTNode *node);
RedBlackBSTNode *rotateRight(RedBlackBSTNode *node);
RedBlackBSTNode *moveRedLeft(RedBlackBSTNode *node);
RedBlackBSTNode *moveRedRight(RedBlackBSTNode *node);
RedBlackBSTNode *fixUp(RedBlackBSTNode *node);
RedBlackBSTNode *setN(RedBlackBSTNode *node);
