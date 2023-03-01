#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tree.h"

/*
 * Create a node.
 */
static struct binary_node_t *node_create(int val)
{
	struct binary_node_t *node;

	/* allocate a node */
	node = (struct binary_node_t *) malloc(sizeof(struct binary_node_t));
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
static void node_free(struct binary_node_t *node)
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
static int node_height(struct binary_node_t *node)
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
static struct binary_node_t *node_find(struct binary_node_t *node, int val)
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
 * Traverse a node in order and store values.
 */
static void node_traverse_in_order(struct binary_node_t *node, struct binary_node_t **nodes, int *i)
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
static struct binary_node_t *node_insert(struct tree_t *tree, struct binary_node_t *node, int val)
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
static struct binary_node_t *node_min(struct binary_node_t *node)
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
static struct binary_node_t *node_delete(struct tree_t *tree, struct binary_node_t *node, int val)
{
	struct binary_node_t *tmp;

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
static struct binary_node_t *node_make_balanced(struct tree_t *tree, struct binary_node_t **nodes, int start, int end)
{
	struct binary_node_t *root;
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
 * Init a tree.
 */
static void tree_init(struct tree_t *tree)
{
	if (!tree)
		return;

	tree->size = 0;
	tree->root.binary = NULL;
}
/*
 * Free a tree.
 */
static void tree_free(struct tree_t *tree)
{
	if (!tree)
		return;

	node_free(tree->root.binary);
	free(tree);
}

/*
 * Compute a tree height.
 */
static int tree_height(struct tree_t *tree)
{
	if (!tree)
		return 0;

	return node_height(tree->root.binary);
}

/*
 * Find a node in a tree.
 */
static int tree_find(struct tree_t *tree, int val)
{
	if (!tree)
		return 0;

	return node_find(tree->root.binary, val) != NULL;
}

/*
 * Insert a value in a tree.
 */
static void tree_insert(struct tree_t *tree, int val)
{
	if (!tree)
		return;

	tree->root.binary = node_insert(tree, tree->root.binary, val);
}

/*
 * Delete a value in a tree.
 */
static void tree_delete(struct tree_t *tree, int val)
{
	if (!tree)
		return;

	tree->root.binary = node_delete(tree, tree->root.binary, val);
}

/*
 * Balance a tree.
 */
static void tree_balance(struct tree_t *tree)
{
	struct binary_node_t **nodes;
	int i = 0;

	/* emptry tree */
	if (!tree)
		return;

	/* no need to balance */
	if (tree->size <= 2)
		return;

	/* create an array to store all nodes */
	nodes = (struct binary_node_t **) malloc(sizeof(struct binary_node_t *) * tree->size);
	if (!nodes)
		return;

	/* store nodes, in order */
	node_traverse_in_order(tree->root.binary, nodes, &i);

	/* insert nodes one by one, in a balanced tree */
	tree->root.binary = node_make_balanced(tree, nodes, 0, tree->size - 1);

	/* free nodes array */
	free(nodes);
}

/*
 * Draw a node value.
 */
static void node_draw_value(struct binary_node_t *node, cairo_t *cr, int x, gint y)
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
static void node_draw(struct binary_node_t *node, cairo_t *cr, int x, int y, int space_sibling)
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
	node_draw(tree->root.binary, cr, x, y, space_sibling);
}

/*
 * Binary tree operations.
 */
struct tree_operations_t binary_tree_ops = {
	.init			= tree_init,
	.height			= tree_height,
	.find			= tree_find,
	.insert			= tree_insert,
	.delete			= tree_delete,
	.balance		= tree_balance,
	.free			= tree_free,
	.draw			= tree_draw,
};
