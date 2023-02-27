#include <gtk/gtk.h>
#include <math.h>

#include "tree.h"

#define NODE_SIZE_X			20
#define NODE_SIZE_Y			20
#define NODE_FONT			"Arial"
#define MAX_NODE_VALUE			99
#define TREE_TYPE			TREE_TYPE_AVL

/*
 * Tree window.
 */
struct tree_window_t {
	GtkWidget *			window;
	GtkWidget *			main_box;
	GtkWidget *			drawing_area;
	GtkWidget *			grid_controls;
	GtkWidget *			button_add;
	GtkWidget *			entry_spin_add;
	GtkAdjustment *			entry_spin_add_adj;
	GtkWidget *			button_delete;
	GtkWidget *			entry_spin_delete;
	GtkAdjustment *			entry_spin_delete_adj;
	GtkWidget *			button_add_random;
	GtkWidget *			button_balance;
	GtkWidget *			button_clear;
	cairo_surface_t *		drawing_surface;
	struct tree_t *			tree;
};

/*
 * Draw a node value.
 */
static void draw_node_value(struct node_t *node, cairo_t *cr, gint x, gint y)
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
static void draw_node(struct node_t *node, cairo_t *cr, gint x, gint y, gint space_sibling)
{
	gint x_child, y_child;

	if (!node)
		return;

	/* draw value */
	draw_node_value(node, cr, x, y);

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
		draw_node(node->left, cr, x_child, y_child, space_sibling / 2);
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
		draw_node(node->right, cr, x_child, y_child, space_sibling / 2);
	}
}

/*
 * Draw a tree.
 */
static void draw_tree(struct tree_window_t *tree_window)
{
	struct tree_t *tree = tree_window->tree;
	GtkAllocation *alloc;
	int space_sibling;
	cairo_t *cr;
	gint x, y;

	/* destroy surface */
	if (tree_window->drawing_surface)
		cairo_surface_destroy(tree_window->drawing_surface);

	/* create surface */
	tree_window->drawing_surface = gdk_window_create_similar_surface(gtk_widget_get_window(tree_window->drawing_area),
			CAIRO_CONTENT_COLOR,
			gtk_widget_get_allocated_width(tree_window->drawing_area),
			gtk_widget_get_allocated_height(tree_window->drawing_area));

	/* create empty surface */
	cr = cairo_create(tree_window->drawing_surface);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);

	/* draw tree */
	if (tree) {
		/* compute space between sibling */
		space_sibling = pow(2, tree->ops->height(tree) - 1) / 2;

		/* get drawing area size */
		alloc = g_new(GtkAllocation, 1);
		gtk_widget_get_allocation(tree_window->drawing_area, alloc);

		/* start at middle x */
		x = alloc->width / 2;
		y = 100;

		/* free drawing area size */
		g_free(alloc);

		/* draw root node */
		draw_node(tree->root, cr, x, y, space_sibling);
	}

	/* destroy cairo */
	cairo_destroy(cr);
}

/*
 * Exit callback.
 */
static void exit_cb(GtkWidget *widget, struct tree_window_t *tree_window)
{
	struct tree_t *tree;

	g_assert(widget != NULL);

	/* free tree */
	if (tree_window) {
		tree = tree_window->tree;
		if (tree)
			tree->ops->free(tree);
		tree_window->tree = NULL;
	}

	/* destroy surface */
	if (tree_window->drawing_surface)
		cairo_surface_destroy(tree_window->drawing_surface);

	gtk_main_quit();
}

/*
 * Draw callback.
 */
static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, struct tree_window_t *tree_window)
{
	g_assert(widget != NULL);

	cairo_set_source_surface(cr, tree_window->drawing_surface, 0, 0);
	cairo_paint(cr);

	return FALSE;
}

/*
 * Configure draw area.
 */
static gboolean configure_event_cb(GtkWidget *widget, GdkEvent *event, struct tree_window_t *tree_window)
{
	g_assert(widget != NULL);
	g_assert(event != NULL);

	/* draw tree */
	draw_tree(tree_window);

	return TRUE;
}

/*
 * Add a value callback.
 */
static void add_cb(GtkWidget *widget, struct tree_window_t *tree_window)
{
	struct tree_t *tree = tree_window->tree;
	int val;

	g_assert(widget != NULL);

	/* create tree if needed */
	if (!tree)
		tree_window->tree = tree = tree_create(TREE_TYPE);

	/* insert not implemented */
	if (!tree || !tree->ops || !tree->ops->insert)
		return;

	/* insert value */
	val = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(tree_window->entry_spin_add));
	tree->ops->insert(tree, val);

	/* draw tree */	
	draw_tree(tree_window);
	gtk_widget_queue_draw(tree_window->drawing_area);
}

/*
 * Delete a value callback.
 */
static void delete_cb(GtkWidget *widget, struct tree_window_t *tree_window)
{
	struct tree_t *tree = tree_window->tree;
	int val;

	g_assert(widget != NULL);

	/* delete not implemented */
	if (!tree || !tree->ops || !tree->ops->delete)
		return;

	/* delete a value */
	val = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(tree_window->entry_spin_delete));
	tree->ops->delete(tree, val);

	/* draw tree */	
	draw_tree(tree_window);
	gtk_widget_queue_draw(tree_window->drawing_area);
}

/*
 * Add a random value callback.
 */
static void add_random_cb(GtkWidget *widget, struct tree_window_t *tree_window)
{
	struct tree_t *tree = tree_window->tree;

	g_assert(widget != NULL);

	/* create tree if needed */
	if (!tree)
		tree_window->tree = tree = tree_create(TREE_TYPE);

	/* insert not implemented */
	if (!tree || !tree->ops || !tree->ops->insert)
		return;

	/* insert a random value */
	tree->ops->insert(tree, rand() % MAX_NODE_VALUE);

	/* draw tree */	
	draw_tree(tree_window);
	gtk_widget_queue_draw(tree_window->drawing_area);
}

/*
 * Balance tree callback.
 */
static void balance_tree_cb(GtkWidget *widget, struct tree_window_t *tree_window)
{
	struct tree_t *tree = tree_window->tree;

	g_assert(widget != NULL);
	
	/* balance not implemented */
	if (!tree || !tree->ops || !tree->ops->balance)
		return;

	/* balance tree */
	tree->ops->balance(tree);

	/* draw tree */	
	draw_tree(tree_window);
	gtk_widget_queue_draw(tree_window->drawing_area);
}

/*
 * Clear tree callback.
 */
static void clear_tree_cb(GtkWidget *widget, struct tree_window_t *tree_window)
{
	struct tree_t *tree = tree_window->tree;
	cairo_t *cr;

	g_assert(widget != NULL);

	/* free not implemented */
	if (!tree || !tree->ops || !tree->ops->free)
		return;

	/* free tree */
	tree->ops->free(tree);
	tree_window->tree = NULL;

	/* clear surface */
	cr = cairo_create(tree_window->drawing_surface);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);
	cairo_destroy(cr);

	gtk_widget_queue_draw(tree_window->drawing_area);
}

/*
 * Create tree window.
 */
static struct tree_window_t *tree_window_create()
{
	struct tree_window_t *tree_window;

	/* allocate tree window */
	tree_window = (struct tree_window_t *) malloc(sizeof(struct tree_window_t));
	if (!tree_window)
		return NULL;

	/* set tree */
	tree_window->drawing_surface = NULL;
	tree_window->tree = NULL;

	/* create main window */
	tree_window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(tree_window->window), "Tree");
	gtk_container_set_border_width(GTK_CONTAINER(tree_window->window), 8);
	g_signal_connect(G_OBJECT(tree_window->window), "destroy", G_CALLBACK(exit_cb), tree_window);

	/* create main box */
	tree_window->main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	/* create drawing area */
	tree_window->drawing_area = gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(tree_window->drawing_area), "draw", G_CALLBACK(draw_cb), tree_window);
	g_signal_connect(G_OBJECT(tree_window->drawing_area),"configure-event", G_CALLBACK(configure_event_cb), tree_window);

	/* create controls grid */
	tree_window->grid_controls = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(tree_window->grid_controls), 5);
	gtk_grid_set_column_spacing(GTK_GRID(tree_window->grid_controls), 5);

	/* create add button */
	tree_window->button_add = gtk_button_new_with_label("Add item");
	tree_window->entry_spin_add_adj = gtk_adjustment_new(MAX_NODE_VALUE / 2, 0, MAX_NODE_VALUE, 1, 0, 0);
	tree_window->entry_spin_add = gtk_spin_button_new(tree_window->entry_spin_add_adj, 1, 0);
	g_signal_connect(G_OBJECT(tree_window->button_add), "clicked", G_CALLBACK(add_cb), tree_window);

	/* create delete button */
	tree_window->button_delete = gtk_button_new_with_label("Delete item");
	tree_window->entry_spin_delete_adj = gtk_adjustment_new(MAX_NODE_VALUE / 2, 0, MAX_NODE_VALUE, 1, 0, 0);
	tree_window->entry_spin_delete = gtk_spin_button_new(tree_window->entry_spin_delete_adj, 1, 0);
	g_signal_connect(G_OBJECT(tree_window->button_delete), "clicked", G_CALLBACK(delete_cb), tree_window);

	/* create add random button */
	tree_window->button_add_random = gtk_button_new_with_label("Add random item");
	g_signal_connect(G_OBJECT(tree_window->button_add_random), "clicked", G_CALLBACK(add_random_cb), tree_window);

	/* create balance button */
	tree_window->button_balance = gtk_button_new_with_label("Balance tree");
	g_signal_connect(G_OBJECT(tree_window->button_balance), "clicked", G_CALLBACK(balance_tree_cb), tree_window);

	/* create clear button */
	tree_window->button_clear = gtk_button_new_with_label("Clear tree");
	g_signal_connect(G_OBJECT(tree_window->button_clear), "clicked", G_CALLBACK(clear_tree_cb), tree_window);

	/* pack controls grid */
	gtk_grid_attach(GTK_GRID(tree_window->grid_controls), tree_window->button_add, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(tree_window->grid_controls), tree_window->entry_spin_add, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(tree_window->grid_controls), tree_window->button_delete, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(tree_window->grid_controls), tree_window->entry_spin_delete, 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(tree_window->grid_controls), tree_window->button_add_random, 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(tree_window->grid_controls), tree_window->button_balance, 0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(tree_window->grid_controls), tree_window->button_clear, 0, 4, 1, 1);

	/* pack window */
	gtk_box_pack_start(GTK_BOX(tree_window->main_box), tree_window->drawing_area, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(tree_window->main_box), tree_window->grid_controls, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(tree_window->window), tree_window->main_box);

	return tree_window;
}

/*
 * Main.
 */
int main(int argc, char **argv)
{
	struct tree_window_t *tree_window;

	/* init gtk */
	gtk_init(&argc, &argv);

	/* seed */
	srand(time(NULL));

	/* create tree window */
	tree_window = tree_window_create();
	if (!tree_window)
		return EXIT_FAILURE;

	/* show main window */
	gtk_widget_show_all(tree_window->window);
	gtk_main();

	return 0;
}
