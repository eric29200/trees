#ifndef _TREE_H_
#define _TREE_H_

#define TREE_TYPE_BINARY		1
#define TREE_TYPE_AVL			2

#define UNUSED(x)			((void) x)

/*
 * Tree node structure.
 */
struct node_t {
	int				val;
	int				height;
	struct node_t *			left;
	struct node_t *			right;
};

/*
 * Tree structure.
 */
struct tree_t {
	struct node_t *			root;
	int				size;
	struct tree_operations_t *	ops;
};

/*
 * Tree operations.
 */
struct tree_operations_t {
	int				(*height)(struct tree_t *);
	int				(*find)(struct tree_t *, int);
	void	 			(*insert)(struct tree_t *, int);
	void 				(*delete)(struct tree_t *, int);
	void 				(*balance)(struct tree_t *);
	void				(*free)(struct tree_t *);
};

/* tree operations */
extern struct tree_operations_t binary_tree_ops;
extern struct tree_operations_t avl_tree_ops;

/* node prototypes */
struct node_t *node_create(int val);
void node_free(struct node_t *node);

/* tree prototypes */
struct tree_t *tree_create(int type);
void generic_tree_free(struct tree_t *tree);
int generic_tree_height(struct tree_t *tree);
int generic_tree_find(struct tree_t *tree, int val);

/*
 * Utility function to compute maximum int.
 */
static inline int max(int a, int b)
{
	return a > b ? a : b;
}


#endif
