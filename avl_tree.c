#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

/*
 * Get node height.
 */
static int node_height(struct node_t *node)
{
	if (!node)
		return 0;

	return node->height;
}

/*
 * Right rotate subtree rooted with y.
 */
struct node_t *right_rotate(struct node_t *y)
{
	struct node_t *x = y->left;
	struct node_t *t2 = x->right;

	/* rotate */
	x->right = y;
	y->left = t2;

	/* update heights */
	y->height = max(node_height(y->left), node_height(y->right)) + 1;
	x->height = max(node_height(x->left), node_height(x->right)) + 1;

	return x;
}

/*
 * Left rotate subtree rooted with x.
 */
struct node_t *left_rotate(struct node_t *x)
{
	struct node_t *y = x->right;
	struct node_t *t2 = y->left;

	/* rotate */
	y->left = x;
	x->right = t2;

	/* update heights */
	x->height = max(node_height(x->left), node_height(x->right)) + 1;
	y->height = max(node_height(y->left), node_height(y->right)) + 1;

	return y;
}

/*
 * Compute node balance.
 */
static int node_balance(struct node_t *node)
{
	if (!node)
		return 0;

	return node_height(node->left) - node_height(node->right);
}

/*
 * Insert a value in a node.
 */
static struct node_t *node_insert(struct tree_t *tree, struct node_t *node, int val)
{
	int balance;

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
	else
		goto out;

	/* update node height */
	node->height = 1 + max(node_height(node->left), node_height(node->right));

	/* compute node balance */
	balance = node_balance(node);

	/* left left case */
	if (balance > 1 && val < node->left->val)
		return right_rotate(node);
	
	/* right right case */
	if (balance < -1 && val > node->right->val)
		return left_rotate(node);
 
	/* left right case */
	if (balance > 1 && val > node->left->val) {
        	node->left = left_rotate(node->left);
        	return right_rotate(node);
    	}
 
	/* right left case */
	if (balance < -1 && val < node->right->val) {
		node->right = right_rotate(node->right);
		return left_rotate(node);
	}

out:
	return node;
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
 * Balance a tree.
 */
static void tree_balance(struct tree_t *tree)
{
	/* nothing to do : AVL trees are always balanced */
	UNUSED(tree);
}

/*
 * AVL tree operations.
 */
struct tree_operations_t avl_tree_ops = {
	.height			= generic_tree_height,
	.insert			= tree_insert,
	.delete			= NULL,
	.balance		= tree_balance,
	.free			= generic_tree_free,
};
