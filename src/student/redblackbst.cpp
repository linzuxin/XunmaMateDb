
#include "redblackbst.h"
#include <stdlib.h>

redBlackBSTNode *root;
int species = TD234;
int heightBLACK;

int size()
{
  return size(root);
}
int size(redBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  else
    return node->N;
}
int sizeRedBlackBST(redBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  else
    return node->N;
}
int rootRank()
{
  if (root == NULL)
    return 0;
  else
    return size(root->left);
}
int rootRank(redBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  else
    return size(node->left);
}
int height()
{
  return height(root);
}
int height(redBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  else
    return node->height;
}
bool contains(uint64_t key)
{
  return (get(root, key) != 0);
}
uint64_t get(uint64_t key)
{
  return get(root, key);
}
uint64_t get(redBlackBSTNode *node, uint64_t key)
{
  if (node == NULL)
    return 0;
  if (key == node->key)
    return node->value;
  else if (key < node->key)
    return get(node->left, key);
  else
    return get(node->right, key);
}
uint64_t min()
{
  if (root == NULL)
    return 0;
  else
    return min(root);
}
uint64_t max()
{
  if (root == NULL)
    return 0;
  else
    return max(root);
}
uint64_t min(redBlackBSTNode *node)
{
  if (node->left == NULL)
    return node->key;
  else
    return min(node->left);
}
uint64_t max(redBlackBSTNode *node)
{
  if (node->right == NULL)
    return node->key;
  else
    return max(node->right);
}

int heightB()
{
  return heightBLACK;
}

void put(uint64_t key, uint64_t value)
{
  root = insert(root, key, value);
  if (isRed(root))
    heightBLACK++;
  root->color = BLACK;
}

redBlackBSTNode *insert(redBlackBSTNode *node, uint64_t key, uint64_t value)
{
  if (node == NULL)
  {
    redBlackBSTNode *nodeTemp = (redBlackBSTNode *)malloc(sizeof(redBlackBSTNode));
    nodeTemp->key = key;
    nodeTemp->value = value;
    nodeTemp->color = RED;
    nodeTemp->N = 1;
    nodeTemp->height = 1;
    return nodeTemp;
  }

  if (species == TD234)
    if (isRed(node->left) && isRed(node->right))
      colorFlip(node);

  if (key == node->key)
    node->value = value;
  else if (key < node->key)
    node->left = insert(node->left, key, value);
  else
    node->right = insert(node->right, key, value);

  if (species == BST)
    return setN(node);

  if (isRed(node->right))
    node = rotateLeft(node);

  if (isRed(node->left) && isRed(node->left->left))
    node = rotateRight(node);

  if (species == BU23)
    if (isRed(node->left) && isRed(node->right))
      colorFlip(node);

  return setN(node);
}

void deleteNodeMin()
{
  root = deleteNodeMin(root);
  root->color = BLACK;
}

redBlackBSTNode *deleteNodeMin(redBlackBSTNode *node)
{
  if (node->left == NULL)
    return NULL;

  if (!isRed(node->left) && !isRed(node->left->left))
    node = moveRedLeft(node);

  node->left = deleteNodeMin(node->left);

  return fixUp(node);
}

void deleteNodeMax()
{
  root = deleteNodeMax(root);
  root->color = BLACK;
}

redBlackBSTNode *deleteNodeMax(redBlackBSTNode *node)
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

void deleteNode(uint64_t key)
{
  root = deleteNode(root, key);
  root->color = BLACK;
}

redBlackBSTNode *deleteNode(redBlackBSTNode *node, uint64_t key)
{
  if (key < node->key)
  {
    if (!isRed(node->left) && !isRed(node->left->left))
      node = moveRedLeft(node);
    node->left = deleteNode(node->left, key);
  }
  else
  {
    if (isRed(node->left))
      node = rotateRight(node);
    if (key < node->key && (node->right == NULL))
      return NULL;
    if (!isRed(node->right) && !isRed(node->right->left))
      node = moveRedRight(node);
    if (key == node->key)
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

bool isRed(redBlackBSTNode *node)
{
  if (node == NULL)
    return false;
  return (node->color == RED);
}

void colorFlip(redBlackBSTNode *node)
{
  node->color = !node->color;
  node->left->color = !node->left->color;
  node->right->color = !node->right->color;
}

redBlackBSTNode *rotateLeft(redBlackBSTNode *node)
{
  redBlackBSTNode *rnode = node->right;
  node->right = rnode->left;
  rnode->left = setN(node);
  rnode->color = rnode->left->color;
  rnode->left->color = RED;
  return setN(rnode);
}

redBlackBSTNode *rotateRight(redBlackBSTNode *node)
{
  redBlackBSTNode *rnode = node->left;
  node->left = rnode->right;
  rnode->right = setN(node);
  rnode->color = rnode->right->color;
  rnode->right->color = RED;
  return setN(rnode);
}

redBlackBSTNode *moveRedLeft(redBlackBSTNode *node)
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

redBlackBSTNode *moveRedRight(redBlackBSTNode *node)
{
  colorFlip(node);
  if (isRed(node->left->left))
  {
    node = rotateRight(node);
    colorFlip(node);
  }
  return node;
}

redBlackBSTNode *fixUp(redBlackBSTNode *node)
{
  if (isRed(node->right))
    node = rotateLeft(node);

  if (isRed(node->left) && isRed(node->left->left))
    node = rotateRight(node);

  if (isRed(node->left) && isRed(node->right))
    colorFlip(node);

  return setN(node);
}

redBlackBSTNode *setN(redBlackBSTNode *node)
{
  node->N = size(node->left) + size(node->right) + 1;
  if (height(node->left) > height(node->right))
    node->height = height(node->left) + 1;
  else
    node->height = height(node->right) + 1;
  return node;
}

int ipl()
{
  return ipl(root);
}

int ipl(redBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  return size(node) - 1 + ipl(node->left) + ipl(node->right);
}

int sizeRed()
{
  return sizeRed(root);
}

int sizeRed(redBlackBSTNode *node)
{
  if (node == NULL)
    return 0;
  if (isRed(node))
    return 1 + sizeRed(node->left) + sizeRed(node->right);
  else
    return sizeRed(node->left) + sizeRed(node->right);
}

bool check()
{
  return isBST() && is234() && isBalanced();
}

bool isBST()
{
  return isBST(root, min(), max());
}

bool isBST(redBlackBSTNode *node, uint64_t min, uint64_t max)
{
  if (node == NULL)
    return true;
  if (node->key < min || max < node->key)
    return false;
  return isBST(node->left, min, node->key) && isBST(node->right, node->key, max);
}

bool is234()
{
  return is234(root);
}

bool is234(redBlackBSTNode *node)
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

bool isBalanced()
{
  int black = 0;
  redBlackBSTNode *node = root;
  while (node != NULL)
  {
    if (!isRed(node))
      black++;
    node = node->left;
  }
  return isBalanced(root, black);
}

bool isBalanced(redBlackBSTNode *node, int black)
{
  if (node == NULL && black == 0)
    return true;
  else if (node == NULL && black != 0)
    return false;
  if (!isRed(node))
    black--;
  return isBalanced(node->left, black) && isBalanced(node->right, black);
}

redBlackBSTNode *test()
{
  for (int i = 0; i < 100; i++)
  {
    put(i, i);
    put(i, i + get(i));
  }
  return root;
}