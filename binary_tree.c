#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

/*
 * Traverse a node in order and store values.
 */
static void node_traverse_in_order(struct node_t *node, struct node_t **nodes, int *i)
{
	if (!node)
		return;

	/* traverse left child */
	node_traverse_in_order(node->left, nodes, i);

	/* store value */
	nodes[*i] = node;
	*i += 1;

	/* traverse right child */
	node_traverse_in_order(node->right, nodes, i);
}

/*
 * Insert a value in a node.
 */
static struct node_t *node_insert(struct tree_t *tree, struct node_t *node, int val)
{
	/* leaf : insert node */
	if (!node) {
		/* create node */
		node = node_create(val);
		if (!node)
			goto out;

		/* update tree size */
		tree->size++;
		goto out;
	}

	/* find subtree */
	if (val < node->val)
		node->left = node_insert(tree, node->left, val);
	else if (val > node->val)
		node->right = node_insert(tree, node->right, val);

out:
	return node;
}

/*
 * Find minimum value in a node.
 */
static struct node_t *node_min(struct node_t *node)
{
	if (!node)
		return NULL;

	if (!node->left)
		return node;

	return node_min(node->left);
}

/*
 * Delete a node.
 */
static struct node_t *node_delete(struct tree_t *tree, struct node_t *node, int val)
{
	struct node_t *tmp;

	if (!node)
		return NULL;

	/* delete in children */
	if (val < node->val) {
		node->left = node_delete(tree, node->left, val);
		return node;
	} else if (val > node->val) {
		node->right = node_delete(tree, node->right, val);
		return node;
	}

	/* this node must be deleted */

	/* only one child or no child : replace this node with this child */
	if (!node->left || !node->right) {
		tmp = node->left ? node->left : node->right;
		free(node);
		tree->size--;
		return tmp;
	}

	/* find minimum value in right child */
	tmp = node_min(node->right);

	/* set this node with minimum value */
	node->val = tmp->val;

	/* delete minimum value in right child */
	node->right = node_delete(tree, node->right, node->val);

	return node;
}

/*
 * Make a balanced node/tree.
 */
static struct node_t *node_make_balanced(struct tree_t *tree, struct node_t **nodes, int start, int end)
{
	struct node_t *root;
	int mid;

	/* no more nodes */
	if (start > end)
		return NULL;

	/* make middle node as root */
	mid = (start + end) / 2;
	root = nodes[mid];

	/* insert nodes in left child */
	root->left = node_make_balanced(tree, nodes, start, mid - 1);

	/* insert nodes in right child */
	root->right = node_make_balanced(tree, nodes, mid + 1, end);

	return root;
}

/*
 * Insert a value in a tree.
 */
static void tree_insert(struct tree_t *tree, int val)
{
	if (!tree)
		return;

	tree->root = node_insert(tree, tree->root, val);
}

/*
 * Delete a value in a tree.
 */
static void tree_delete(struct tree_t *tree, int val)
{
	if (!tree)
		return;

	tree->root = node_delete(tree, tree->root, val);
}

/*
 * Balance a tree.
 */
static void tree_balance(struct tree_t *tree)
{
	struct node_t **nodes;
	int i = 0;

	/* emptry tree */
	if (!tree)
		return;

	/* no need to balance */
	if (tree->size <= 2)
		return;

	/* create an array to store all nodes */
	nodes = (struct node_t **) malloc(sizeof(struct node_t *) * tree->size);
	if (!nodes)
		return;

	/* store nodes, in order */
	node_traverse_in_order(tree->root, nodes, &i);

	/* insert nodes one by one, in a balanced tree */
	tree->root = node_make_balanced(tree, nodes, 0, tree->size - 1);

	/* free nodes array */
	free(nodes);
}

/*
 * Binary tree operations.
 */
struct tree_operations_t binary_tree_ops = {
	.height			= generic_tree_height,
	.find			= generic_tree_find,
	.insert			= tree_insert,
	.delete			= tree_delete,
	.balance		= tree_balance,
	.free			= generic_tree_free,
};
