#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

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

	/* set operations */
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

	/* init tree */
	tree->ops->init(tree);

	return tree;
}

