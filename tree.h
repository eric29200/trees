#ifndef _TREE_H_
#define _TREE_H_

#include <gtk/gtk.h>

#define NODE_SIZE_X			20
#define NODE_SIZE_Y			20
#define NODE_FONT			"Arial"

#define TREE_TYPE_BINARY		1
#define TREE_TYPE_AVL			2

#define UNUSED(x)			((void) x)

/*
 * Binary node structure.
 */
struct binary_node_t {
	int				val;
	struct binary_node_t *		left;
	struct binary_node_t *		right;
};

/*
 * AVL node structure.
 */
struct avl_node_t {
	int				val;
	int				height;
	struct avl_node_t *		left;
	struct avl_node_t *		right;
};

/*
 * Tree structure.
 */
struct tree_t {
	union {
		struct binary_node_t *	binary;
		struct avl_node_t *	avl;
	} root;
	int				size;
	struct tree_operations_t *	ops;
};

/*
 * Tree operations.
 */
struct tree_operations_t {
	void				(*init)(struct tree_t *);
	int				(*height)(struct tree_t *);
	int				(*find)(struct tree_t *, int);
	void	 			(*insert)(struct tree_t *, int);
	void 				(*delete)(struct tree_t *, int);
	void 				(*balance)(struct tree_t *);
	void				(*free)(struct tree_t *);
	void				(*draw)(struct tree_t *, GtkWidget *, cairo_t *);

};

/* tree operations */
extern struct tree_operations_t binary_tree_ops;
extern struct tree_operations_t avl_tree_ops;

/* tree prototypes */
struct tree_t *tree_create(int type);

/*
 * Utility function to compute maximum int.
 */
static inline int max(int a, int b)
{
	return a > b ? a : b;
}

#endif