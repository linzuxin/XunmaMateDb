#pragma once
#include <cstdint>

#define RED true
#define BLACK false
#define BST 0
#define TD234 1
#define BU23 2
#define SPECIES TD234

struct BstValue
{
  uint64_t version;
  uint64_t field;
  uint32_t index;
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

RedBlackBSTNode *test(RedBlackBSTNode **root);
int iplRoot(RedBlackBSTNode *root);
int sizeRoot(RedBlackBSTNode *root);
bool checkRoot(RedBlackBSTNode *root);
int sizeRedRoot(RedBlackBSTNode *root);
int rootRankRoot(RedBlackBSTNode *root);
int heightRoot(RedBlackBSTNode *root);
void deleteNodeMaxRoot(RedBlackBSTNode **root);
void deleteNodeMinRoot(RedBlackBSTNode **root);
BstNodeKey maxRoot(RedBlackBSTNode *root);
BstNodeKey minRoot(RedBlackBSTNode *root);
bool isBSTRoot(RedBlackBSTNode *root);
bool isBalancedRoot(RedBlackBSTNode *root);
bool is234Root(RedBlackBSTNode *root);
BstNodeValue getRoot(RedBlackBSTNode *root,BstNodeKey key);
void putRoot(RedBlackBSTNode **root,BstNodeKey key, BstNodeValue value);
void deleteNodeRoot(RedBlackBSTNode **root,BstNodeKey key);
bool containsRoot(RedBlackBSTNode *root,BstNodeKey key);

bool eq(BstNodeKey a, BstNodeKey b);
bool less(BstNodeKey a, BstNodeKey b);
bool isBST(RedBlackBSTNode *node, uint64_t min, uint64_t max);
bool is234(RedBlackBSTNode *node);
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
