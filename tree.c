#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

/*
 * Create a node.
 */
struct node_t *node_create(int val)
{
	struct node_t *node;

	/* allocate a node */
	node = (struct node_t *) malloc(sizeof(struct node_t));
	if (!node)
		return NULL;

	/* set node */
	node->val = val;
	node->height = 1;
	node->left = NULL;
	node->right = NULL;

	return node;
}

/*
 * Compute a node height.
 */
static int node_height(struct node_t *node)
{
	int height_l, height_r;

	if (!node)
		return 0;

	/* compute left/right heights */
	height_l = 1 + node_height(node->left);
	height_r = 1 + node_height(node->right);

	/* return max left/right height */
	return max(height_l, height_r);
}

/*
 * Find a node.
 */
static struct node_t *node_find(struct node_t *node, int val)
{
	if (!node)
		return NULL;

	if (val < node->val)
		return node_find(node->left, val);
	else if (val > node->val)
		return node_find(node->right, val);

	return node;
}

/*
 * Free a node.
 */
void node_free(struct node_t *node)
{
	if (!node)
		return;

	/* free children */
	node_free(node->left);
	node_free(node->right);

	/* free node */
	free(node);
}

/*
 * Free a tree.
 */
void generic_tree_free(struct tree_t *tree)
{
	if (!tree)
		return;

	node_free(tree->root);
	free(tree);
}

/*
 * Compute a tree height.
 */
int generic_tree_height(struct tree_t *tree)
{
	if (!tree)
		return 0;

	return node_height(tree->root);
}

/*
 * Find a node in a tree.
 */
struct node_t *generic_tree_find(struct tree_t *tree, int val)
{
	if (!tree)
		return NULL;

	return node_find(tree->root, val);
}

/*
 * Create a tree.
 */
struct tree_t *tree_create(int type)
{
	struct tree_t *tree;

	/* allocate a tree */
	tree = (struct tree_t *) malloc(sizeof(struct tree_t));
	if (!tree)
		return NULL;

	/* set tree */
	tree->root = NULL;
	tree->size = 0;

	switch (type) {
		case TREE_TYPE_BINARY:
			tree->ops = &binary_tree_ops;
			break;
		case TREE_TYPE_AVL:
			tree->ops = &avl_tree_ops;
			break;
		default:
			fprintf(stderr, "unknown tree type %d\n", type);
			free(tree);
			return NULL;
	}

	return tree;
}

