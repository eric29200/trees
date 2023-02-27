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
	node->left = NULL;
	node->right = NULL;

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
		default:
			fprintf(stderr, "unknown tree type %d\n", type);
			free(tree);
			return NULL;
	}

	return tree;
}

