
#include "redblackbst.h"
#include <stdlib.h>
#include <stdio.h>

int sizeRoot(RedBlackBSTNode *root)
{
  return size(root);
}
int size(RedBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  else
    return node->N;
}
int sizeRedBlackBST(RedBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  else
    return node->N;
}
int rootRankRoot(RedBlackBSTNode *root)
{
  if (root == NULL)
    return 0;
  else
    return size(root->left);
}
int rootRank(RedBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  else
    return size(node->left);
}
int heightRoot(RedBlackBSTNode *root)
{
  return height(root);
}
int height(RedBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  else
    return node->height;
}
bool containsRoot(RedBlackBSTNode *root, BstNodeKey key)
{
  return (get(root, key) != NULL);
}
BstNodeValue getRoot(RedBlackBSTNode *root, BstNodeKey key)
{
  return get(root, key);
}
BstNodeValue get(RedBlackBSTNode *node, BstNodeKey key)
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
BstNodeKey minRoot(RedBlackBSTNode *root)
{
  if (root == NULL)
    return 0;
  else
    return min(root);
}
BstNodeKey maxRoot(RedBlackBSTNode *root)
{
  if (root == NULL)
    return 0;
  else
    return max(root);
}
BstNodeKey min(RedBlackBSTNode *node)
{
  if (node->left == NULL)
    return node->key;
  else
    return min(node->left);
}
BstNodeKey max(RedBlackBSTNode *node)
{
  if (node->right == NULL)
    return node->key;
  else
    return max(node->right);
}

void putRoot(RedBlackBSTNode **root, BstNodeKey key, BstNodeValue value)
{
  *root = insert(*root, key, value);
  (*root)->color = BLACK;
}

RedBlackBSTNode *insert(RedBlackBSTNode *node, BstNodeKey key, BstNodeValue value)
{
  if (node == NULL)
  {
    RedBlackBSTNode *nodeTemp = (RedBlackBSTNode *)malloc(sizeof(RedBlackBSTNode));
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

void deleteNodeMinRoot(RedBlackBSTNode **root)
{
  (*root) = deleteNodeMin(*root);
  (*root)->color = BLACK;
}

RedBlackBSTNode *deleteNodeMin(RedBlackBSTNode *node)
{
  if (node->left == NULL)
    return NULL;

  if (!isRed(node->left) && !isRed(node->left->left))
    node = moveRedLeft(node);

  node->left = deleteNodeMin(node->left);

  return fixUp(node);
}

void deleteNodeMaxRoot(RedBlackBSTNode **root)
{
  (*root) = deleteNodeMax((*root));
  (*root)->color = BLACK;
}
bool less(BstNodeKey a, BstNodeKey b)
{
  return a < b;
}
bool eq(BstNodeKey a, BstNodeKey b)
{
  return a == b;
}
RedBlackBSTNode *deleteNodeMax(RedBlackBSTNode *node)
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

void deleteNodeRoot(RedBlackBSTNode **root, BstNodeKey key)
{
  (*root) = deleteNode((*root), key);
  (*root)->color = BLACK;
}

RedBlackBSTNode *deleteNode(RedBlackBSTNode *node, BstNodeKey key)
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

bool isRed(RedBlackBSTNode *node)
{
  if (node == NULL)
    return false;
  return (node->color == RED);
}

void colorFlip(RedBlackBSTNode *node)
{
  node->color = !node->color;
  node->left->color = !node->left->color;
  node->right->color = !node->right->color;
}

RedBlackBSTNode *rotateLeft(RedBlackBSTNode *node)
{
  RedBlackBSTNode *rnode = node->right;
  node->right = rnode->left;
  rnode->left = setN(node);
  rnode->color = rnode->left->color;
  rnode->left->color = RED;
  return setN(rnode);
}

RedBlackBSTNode *rotateRight(RedBlackBSTNode *node)
{
  RedBlackBSTNode *rnode = node->left;
  node->left = rnode->right;
  rnode->right = setN(node);
  rnode->color = rnode->right->color;
  rnode->right->color = RED;
  return setN(rnode);
}

RedBlackBSTNode *moveRedLeft(RedBlackBSTNode *node)
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

RedBlackBSTNode *moveRedRight(RedBlackBSTNode *node)
{
  colorFlip(node);
  if (isRed(node->left->left))
  {
    node = rotateRight(node);
    colorFlip(node);
  }
  return node;
}

RedBlackBSTNode *fixUp(RedBlackBSTNode *node)
{
  if (isRed(node->right))
    node = rotateLeft(node);

  if (isRed(node->left) && isRed(node->left->left))
    node = rotateRight(node);

  if (isRed(node->left) && isRed(node->right))
    colorFlip(node);

  return setN(node);
}

RedBlackBSTNode *setN(RedBlackBSTNode *node)
{
  node->N = size(node->left) + size(node->right) + 1;
  if (height(node->left) > height(node->right))
    node->height = height(node->left) + 1;
  else
    node->height = height(node->right) + 1;
  return node;
}

int iplRoot(RedBlackBSTNode *root)
{
  return ipl(root);
}

int ipl(RedBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  return size(node) - 1 + ipl(node->left) + ipl(node->right);
}

int sizeRedRoot(RedBlackBSTNode *root)
{
  return sizeRed(root);
}

int sizeRed(RedBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  if (isRed(node))
    return 1 + sizeRed(node->left) + sizeRed(node->right);
  else
    return sizeRed(node->left) + sizeRed(node->right);
}

bool checkRoot(RedBlackBSTNode *root)
{
  return isBSTRoot(root) && is234Root(root) && isBalancedRoot(root);
}

bool isBSTRoot(RedBlackBSTNode *root)
{
  return isBST(root, minRoot(root), maxRoot(root));
}

bool isBST(RedBlackBSTNode *node, uint64_t min, uint64_t max)
{
  if (node == NULL)
    return true;
  if (less(node->key, min) || less(max, node->key))
    return false;
  return isBST(node->left, min, node->key) && isBST(node->right, node->key, max);
}

bool is234Root(RedBlackBSTNode *root)
{
  return is234(root);
}

bool is234(RedBlackBSTNode *node)
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

bool isBalancedRoot(RedBlackBSTNode *root)
{
  int black = 0;
  RedBlackBSTNode *node = root;
  while (node != NULL)
  {
    if (!isRed(node))
      black++;
    node = node->left;
  }
  return isBalanced(root, black);
}

bool isBalanced(RedBlackBSTNode *node, int black)
{
  if (node == NULL && black == 0)
    return true;
  else if (node == NULL && black != 0)
    return false;
  if (!isRed(node))
    black--;
  return isBalanced(node->left, black) && isBalanced(node->right, black);
}

RedBlackBSTNode *test(RedBlackBSTNode **root)
{
  for (int i = 0; i < 100; i++)
  {
    BstNodeValue value = getRoot(*root, i);
    if (value == NULL)
    {
      value = (BstNodeValue)malloc(sizeof(value));
    }
    putRoot(root, i, value);
  }
  return *root;
}