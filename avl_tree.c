#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tree.h"

/*
 * Create a node.
 */
static struct avl_node_t *node_create(int val)
{
	struct avl_node_t *node;

	/* allocate a node */
	node = (struct avl_node_t *) malloc(sizeof(struct avl_node_t));
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
 * Free a node.
 */
static void node_free(struct avl_node_t *node)
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
 * Compute a node height.
 */
static int node_full_height(struct avl_node_t *node)
{
	int height_l, height_r;

	if (!node)
		return 0;

	/* compute left/right heights */
	height_l = 1 + node_full_height(node->left);
	height_r = 1 + node_full_height(node->right);

	/* return max left/right height */
	return max(height_l, height_r);
}

/*
 * Find a node.
 */
static struct avl_node_t *node_find(struct avl_node_t *node, int val)
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
 * Get node height.
 */
static int node_height(struct avl_node_t *node)
{
	if (!node)
		return 0;

	return node->height;
}

/*
 * Find minimum value in a node.
 */
static struct avl_node_t *node_min(struct avl_node_t *node)
{
	if (!node)
		return NULL;

	if (!node->left)
		return node;

	return node_min(node->left);
}

/*
 * Right rotate subtree rooted with y.
 */
struct avl_node_t *right_rotate(struct avl_node_t *y)
{
	struct avl_node_t *x = y->left;
	struct avl_node_t *t2 = x->right;

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
struct avl_node_t *left_rotate(struct avl_node_t *x)
{
	struct avl_node_t *y = x->right;
	struct avl_node_t *t2 = y->left;

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
static int node_balance(struct avl_node_t *node)
{
	if (!node)
		return 0;

	return node_height(node->left) - node_height(node->right);
}

/*
 * Insert a value in a node.
 */
static struct avl_node_t *node_insert(struct tree_t *tree, struct avl_node_t *node, int val)
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
 * Delete a value in a node.
 */
static struct avl_node_t *node_delete(struct tree_t *tree, struct avl_node_t *node, int val)
{
	struct avl_node_t *tmp;
	int balance;

	if (!node)
		return NULL;

	/* delete in left child */
	if (val < node->val) {
		node->left = node_delete(tree, node->left, val);
	/* delete in right child */
	} else if (val > node->val) {
		node->right = node_delete(tree, node->right, val);
	/* this node must be deleted */
	} else {
		/* only one child or no child */
		if (!node->left || !node->right) {
			tmp = node->left ? node->left : node->right;

			/* no child : just free this node */
			if (!tmp) {
				tmp = node;
				node = NULL;
				goto free_this_node;
			}

			/* one child : replace this node with it */
			*node = *tmp;
free_this_node:
			/* free this node */
			tree->size--;
			free(tmp);
			goto balance_this_node;
		}

		/* find minimum value in right child */
		tmp = node_min(node->right);

		/* set this node with minimum value */
		node->val = tmp->val;

		/* delete minimum value in right child */
		node->right = node_delete(tree, node->right, node->val);
	}

balance_this_node:
	if (!node)
		return NULL;

	/* update node height */
	node->height = 1 + max(node_height(node->left), node_height(node->right));

	/* compute node balance */
	balance = node_balance(node);

	/* left left case */
	if (balance > 1 && node_balance(node->left) >= 0)
		return right_rotate(node);

	/* left right case */
	if (balance > 1 && node_balance(node->left) < 0) {
		node->left = left_rotate(node->left);
		return right_rotate(node);
	}

	/* right right case */
	if (balance < -1 && node_balance(node->right) <= 0)
		return left_rotate(node);

	/* right left case */
	if (balance < -1 && node_balance(node->right) > 0) {
		node->right = right_rotate(node->right);
		return left_rotate(node);
	}

	return node;
}

/*
 * Init a tree.
 */
static void tree_init(struct tree_t *tree)
{
	if (!tree)
		return;

	tree->size = 0;
	tree->root.avl = NULL;
}

/*
 * Free a tree.
 */
static void tree_free(struct tree_t *tree)
{
	if (!tree)
		return;

	node_free(tree->root.avl);
	free(tree);
}

/*
 * Compute a tree height.
 */
static int tree_height(struct tree_t *tree)
{
	if (!tree)
		return 0;

	return node_full_height(tree->root.avl);
}

/*
 * Find a node in a tree.
 */
static int tree_find(struct tree_t *tree, int val)
{
	if (!tree)
		return 0;

	return node_find(tree->root.avl, val) != NULL;
}

/*
 * Insert a value in a tree.
 */
static void tree_insert(struct tree_t *tree, int val)
{
	if (!tree)
		return;

	tree->root.avl = node_insert(tree, tree->root.avl, val);
}

/*
 * Delete a value in a tree.
 */
static void tree_delete(struct tree_t *tree, int val)
{
	if (!tree)
		return;

	tree->root.avl = node_delete(tree, tree->root.avl, val);
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
 * Draw a node value.
 */
static void node_draw_value(struct avl_node_t *node, cairo_t *cr, int x, gint y)
{
	char val_string[64];
	int len;

	/* draw rectangle node */
	cairo_set_line_width(cr, 2.0);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, x, y, NODE_SIZE_X, NODE_SIZE_Y);
	cairo_stroke(cr);

	/* draw value */
	len = sprintf(val_string, "%d", node->val);
	cairo_select_font_face(cr, NODE_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 12);
	cairo_move_to(cr, len == 1 ? x + 6 : x + 3, y + 15);
	cairo_show_text(cr, val_string);
}

/*
 * Draw a node.
 */
static void node_draw(struct avl_node_t *node, cairo_t *cr, int x, int y, int space_sibling)
{
	int x_child, y_child;

	if (!node)
		return;

	/* draw value */
	node_draw_value(node, cr, x, y);

	/* draw left child */
	if (node->left) {
		/* compute x/y child */
		x_child = x - NODE_SIZE_X * space_sibling;
		y_child = y + NODE_SIZE_Y * 2;

		/* draw left arrow */
		cairo_move_to(cr, x + NODE_SIZE_X / 2, y + NODE_SIZE_Y);
		cairo_line_to(cr, x_child + NODE_SIZE_X / 2, y_child);
		cairo_stroke(cr);

		/* draw left node */
		node_draw(node->left, cr, x_child, y_child, space_sibling / 2);
	}

	/* draw right child */
	if (node->right) {
		/* compute x/y child */
		x_child = x + NODE_SIZE_X * space_sibling;
		y_child = y + NODE_SIZE_Y * 2;

		/* draw right arrow */
		cairo_move_to(cr, x + NODE_SIZE_X / 2, y + NODE_SIZE_Y);
		cairo_line_to(cr, x_child + NODE_SIZE_X / 2, y_child);
		cairo_stroke(cr);

		/* draw right node */
		node_draw(node->right, cr, x_child, y_child, space_sibling / 2);
	}
}

/*
 * Draw a tree.
 */
static void tree_draw(struct tree_t *tree, GtkWidget *drawing_area, cairo_t *cr)
{
	GtkAllocation *alloc;
	int space_sibling;
	int x, y;
	
	/* compute space between sibling */
	space_sibling = pow(2, tree->ops->height(tree) - 1) / 2;

	/* get drawing area size */
	alloc = g_new(GtkAllocation, 1);
	gtk_widget_get_allocation(drawing_area, alloc);

	/* start at middle x */
	x = alloc->width / 2;
	y = 100;

	/* free drawing area size */
	g_free(alloc);

	/* draw root node */
	node_draw(tree->root.avl, cr, x, y, space_sibling);
}

/*
 * AVL tree operations.
 */
struct tree_operations_t avl_tree_ops = {
	.init			= tree_init,
	.height			= tree_height,
	.find			= tree_find,
	.insert			= tree_insert,
	.delete			= tree_delete,
	.balance		= tree_balance,
	.free			= tree_free,
	.draw			= tree_draw,
};
