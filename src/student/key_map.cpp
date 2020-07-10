#include <stdlib.h>
#include <stdio.h>
#include "key_map.h"

int sizeRoot(KeyMap *root)
{
  return size(root);
}
int size(KeyMap *node)
{
  if (node == NULL)
    return 0;
  else
    return node->N;
}
int sizeRedBlackBST(KeyMap *node)
{
  if (node == NULL)
    return 0;
  else
    return node->N;
}
int rootRankRoot(KeyMap *root)
{
  if (root == NULL)
    return 0;
  else
    return size(root->left);
}
int rootRank(KeyMap *node)
{
  if (node == NULL)
    return 0;
  else
    return size(node->left);
}
int heightRoot(KeyMap *root)
{
  return height(root);
}
int height(KeyMap *node)
{
  if (node == NULL)
    return 0;
  else
    return node->height;
}
bool containsRoot(KeyMap *root, KeyNode key)
{
  return (get(root, key) != NULL);
}
ValueNode getRoot(KeyMap *root, KeyNode key)
{
  return get(root, key);
}
ValueNode get(KeyMap *node, KeyNode key)
{
  if (node == NULL)
    return NULL;
  if (eq(key, node->key))
    return node->value;
  else if (less(key, node->key))
    return get(node->left, key);
  else
    return get(node->right, key);
}
KeyNode minRoot(KeyMap *root)
{
  if (root == NULL)
    return 0;
  else
    return min(root);
}
KeyNode maxRoot(KeyMap *root)
{
  if (root == NULL)
    return 0;
  else
    return max(root);
}
KeyNode min(KeyMap *node)
{
  if (node->left == NULL)
    return node->key;
  else
    return min(node->left);
}
KeyNode max(KeyMap *node)
{
  if (node->right == NULL)
    return node->key;
  else
    return max(node->right);
}

void putRoot(KeyMap **root, KeyNode key, ValueNode value)
{
  *root = insert(*root, key, value);
  (*root)->color = BLACK;
}

KeyMap *insert(KeyMap *node, KeyNode key, ValueNode value)
{
  if (node == NULL)
  {
    KeyMap *nodeTemp = (KeyMap *)malloc(sizeof(KeyMap));
    nodeTemp->key = key;
    nodeTemp->value = value;
    nodeTemp->color = RED;
    nodeTemp->N = 1;
    nodeTemp->height = 1;
    return nodeTemp;
  }

  if (SPECIES == TD234)
    if (isRed(node->left) && isRed(node->right))
      colorFlip(node);

  if (eq(key, node->key))
    node->value = value;
  else if (less(key, node->key))
    node->left = insert(node->left, key, value);
  else
    node->right = insert(node->right, key, value);

  if (SPECIES == BST)
    return setN(node);

  if (isRed(node->right))
    node = rotateLeft(node);

  if (isRed(node->left) && isRed(node->left->left))
    node = rotateRight(node);

  if (SPECIES == BU23)
    if (isRed(node->left) && isRed(node->right))
      colorFlip(node);

  return setN(node);
}

void deleteNodeMinRoot(KeyMap **root)
{
  (*root) = deleteNodeMin(*root);
  (*root)->color = BLACK;
}

KeyMap *deleteNodeMin(KeyMap *node)
{
  if (node->left == NULL)
    return NULL;

  if (!isRed(node->left) && !isRed(node->left->left))
    node = moveRedLeft(node);

  node->left = deleteNodeMin(node->left);

  return fixUp(node);
}

void deleteNodeMaxRoot(KeyMap **root)
{
  (*root) = deleteNodeMax((*root));
  (*root)->color = BLACK;
}
bool less(KeyNode a, KeyNode b)
{
  return a < b;
}
bool eq(KeyNode a, KeyNode b)
{
  return a == b;
}
KeyMap *deleteNodeMax(KeyMap *node)
{
  if (isRed(node->left))
    node = rotateRight(node);

  if (node->right == NULL)
    return NULL;

  if (!isRed(node->right) && !isRed(node->right->left))
    node = moveRedRight(node);

  node->right = deleteNodeMax(node->right);

  return fixUp(node);
}

void deleteNodeRoot(KeyMap **root, KeyNode key)
{
  (*root) = deleteNode((*root), key);
  (*root)->color = BLACK;
}

KeyMap *deleteNode(KeyMap *node, KeyNode key)
{
  if (less(key, node->key))
  {
    if (!isRed(node->left) && !isRed(node->left->left))
      node = moveRedLeft(node);
    node->left = deleteNode(node->left, key);
  }
  else
  {
    if (isRed(node->left))
      node = rotateRight(node);
    if (less(key, node->key) && (node->right == NULL))
      return NULL;
    if (!isRed(node->right) && !isRed(node->right->left))
      node = moveRedRight(node);
    if (eq(key, node->key))
    {
      node->value = get(node->right, min(node->right));
      node->key = min(node->right);
      node->right = deleteNodeMin(node->right);
    }
    else
      node->right = deleteNode(node->right, key);
  }

  return fixUp(node);
}

bool isRed(KeyMap *node)
{
  if (node == NULL)
    return false;
  return (node->color == RED);
}

void colorFlip(KeyMap *node)
{
  node->color = !node->color;
  node->left->color = !node->left->color;
  node->right->color = !node->right->color;
}

KeyMap *rotateLeft(KeyMap *node)
{
  KeyMap *rnode = node->right;
  node->right = rnode->left;
  rnode->left = setN(node);
  rnode->color = rnode->left->color;
  rnode->left->color = RED;
  return setN(rnode);
}

KeyMap *rotateRight(KeyMap *node)
{
  KeyMap *rnode = node->left;
  node->left = rnode->right;
  rnode->right = setN(node);
  rnode->color = rnode->right->color;
  rnode->right->color = RED;
  return setN(rnode);
}

KeyMap *moveRedLeft(KeyMap *node)
{
  colorFlip(node);
  if (isRed(node->right->left))
  {
    node->right = rotateRight(node->right);
    node = rotateLeft(node);
    colorFlip(node);
  }
  return node;
}

KeyMap *moveRedRight(KeyMap *node)
{
  colorFlip(node);
  if (isRed(node->left->left))
  {
    node = rotateRight(node);
    colorFlip(node);
  }
  return node;
}

KeyMap *fixUp(KeyMap *node)
{
  if (isRed(node->right))
    node = rotateLeft(node);

  if (isRed(node->left) && isRed(node->left->left))
    node = rotateRight(node);

  if (isRed(node->left) && isRed(node->right))
    colorFlip(node);

  return setN(node);
}

KeyMap *setN(KeyMap *node)
{
  node->N = size(node->left) + size(node->right) + 1;
  if (height(node->left) > height(node->right))
    node->height = height(node->left) + 1;
  else
    node->height = height(node->right) + 1;
  return node;
}

int iplRoot(KeyMap *root)
{
  return ipl(root);
}

int ipl(KeyMap *node)
{
  if (node == NULL)
    return 0;
  return size(node) - 1 + ipl(node->left) + ipl(node->right);
}

int sizeRedRoot(KeyMap *root)
{
  return sizeRed(root);
}

int sizeRed(KeyMap *node)
{
  if (node == NULL)
    return 0;
  if (isRed(node))
    return 1 + sizeRed(node->left) + sizeRed(node->right);
  else
    return sizeRed(node->left) + sizeRed(node->right);
}

bool checkRoot(KeyMap *root)
{
  return isBSTRoot(root) && is234Root(root) && isBalancedRoot(root);
}

bool isBSTRoot(KeyMap *root)
{
  return isBST(root, minRoot(root), maxRoot(root));
}

bool isBST(KeyMap *node, uint64_t min, uint64_t max)
{
  if (node == NULL)
    return true;
  if (less(node->key, min) || less(max, node->key))
    return false;
  return isBST(node->left, min, node->key) && isBST(node->right, node->key, max);
}

bool is234Root(KeyMap *root)
{
  return is234(root);
}

bool is234(KeyMap *node)
{
  if (node == NULL)
    return true;
  if (isRed(node->right))
    return false;
  if (isRed(node))
    if (isRed(node->left))
      if (isRed(node->left->left))
        return false;
  return is234(node->left) && is234(node->right);
}

bool isBalancedRoot(KeyMap *root)
{
  int black = 0;
  KeyMap *node = root;
  while (node != NULL)
  {
    if (!isRed(node))
      black++;
    node = node->left;
  }
  return isBalanced(root, black);
}

bool isBalanced(KeyMap *node, int black)
{
  if (node == NULL && black == 0)
    return true;
  else if (node == NULL && black != 0)
    return false;
  if (!isRed(node))
    black--;
  return isBalanced(node->left, black) && isBalanced(node->right, black);
}

KeyMap *test(KeyMap **root)
{
  for (int i = 0; i < 100; i++)
  {
    ValueNode value = getRoot(*root, i);
    if (value == NULL)
    {
      value = (ValueNode)malloc(sizeof(value));
    }
    putRoot(root, i, value);
  }
  return *root;
}