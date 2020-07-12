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
  uint32_t index;
};

typedef uint64_t KeyNode;
typedef BstValue* ValueNode;

struct KeyValueNode 
{
  KeyNode key;
  ValueNode value;
  struct KeyValueNode *left;
  struct KeyValueNode *right;
  bool color;
  int N;
  int height;
};

typedef KeyValueNode KeyMap;

KeyMap *test(KeyMap **root);
int iplRoot(KeyMap *root);
int sizeRoot(KeyMap *root);
bool checkRoot(KeyMap *root);
int sizeRedRoot(KeyMap *root);
int rootRankRoot(KeyMap *root);
int heightRoot(KeyMap *root);
void deleteNodeMaxRoot(KeyMap **root);
void deleteNodeMinRoot(KeyMap **root);
KeyNode maxRoot(KeyMap *root);
KeyNode minRoot(KeyMap *root);
bool isBSTRoot(KeyMap *root);
bool isBalancedRoot(KeyMap *root);
bool is234Root(KeyMap *root);
ValueNode getRoot(KeyMap *root,KeyNode key);
void putRoot(KeyMap **root,KeyNode key, ValueNode value);
void deleteNodeRoot(KeyMap **root,KeyNode key);
bool containsRoot(KeyMap *root,KeyNode key);

bool eq(KeyNode a, KeyNode b);
bool less(KeyNode a, KeyNode b);
bool isBST(KeyMap *node, uint64_t min, uint64_t max);
bool is234(KeyMap *node);
bool isBalanced(KeyMap *node, int black);
bool isRed(KeyMap *node);
int ipl(KeyMap *node);
int sizeRed(KeyMap *node);

int height(KeyMap *node);
int rootRank(KeyMap *root);
int size(KeyMap *node);
int sizeRedBlackBST(KeyMap *node);
void colorFlip(KeyMap *node);
KeyNode max(KeyMap *node);
KeyNode min(KeyMap *node);
ValueNode get(KeyMap *node, KeyNode key);
KeyMap *insert(KeyMap *node, KeyNode key, ValueNode value);
KeyMap *deleteNodeMin(KeyMap *node);
KeyMap *deleteNodeMax(KeyMap *node);
KeyMap *deleteNode(KeyMap *node, KeyNode key);
KeyMap *rotateLeft(KeyMap *node);
KeyMap *rotateRight(KeyMap *node);
KeyMap *moveRedLeft(KeyMap *node);
KeyMap *moveRedRight(KeyMap *node);
KeyMap *fixUp(KeyMap *node);
KeyMap *setN(KeyMap *node);
