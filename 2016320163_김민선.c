#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <string.h>

int BN = 0;

typedef struct NODE* NodePtr;

struct NODE {
	int key;
	struct NODE *left;
	struct NODE *right;
	struct NODE *parent;

	enum { RED = 0, BLACK = 1 } color;
};

typedef struct RBT* RBTPtr;
struct RBT {
	NodePtr r;
	NodePtr nil;
};

NodePtr node_alloc(int val) {
	NodePtr self = (NodePtr)malloc(sizeof(struct NODE));
	self->key = val;
	self->left = NULL;
	self->right = NULL;
	self->parent = NULL;

	return self;
}

RBTPtr rbt_alloc() {
	RBTPtr self = (RBTPtr)malloc(sizeof(struct RBT));
	self->nil = (NodePtr)malloc(sizeof(struct NODE));
	(self->nil)->color = BLACK;
	self->r = self->nil;

	return self;
}

void left_rotate(RBTPtr r, NodePtr x) {
	NodePtr temp;

	temp = x->right;

	x->right = temp->left;

	if (temp->left != r->nil) {
		(temp->left)->parent = x;
	}

	temp->parent = x->parent;

	if (x->parent == r->nil) {
		r->r = temp;
	}
	else if (x == (x->parent)->left) {
		(x->parent)->left = temp;
	}
	else {
		(x->parent)->right = temp;
	}

	temp->left = x;
	x->parent = temp;
}

void right_rotate(RBTPtr r, NodePtr y) {
	NodePtr temp;
	temp = y->left;
	y->left = temp->right;

	if (temp->right != r->nil) {
		(temp->right)->parent = y;
	}

	temp->parent = y->parent;

	if (y->parent == r->nil) {
		r->r = temp;
	}
	else if (y == (y->parent)->left) {
		(y->parent)->left = temp;
	}
	else {
		(y->parent)->right = temp;
	}

	temp->right = y;
	y->parent = temp;
}

void insert_fixup(RBTPtr r, NodePtr x) {
	NodePtr u = NULL; //uncle node

	while ((x->parent)->color == RED) {
		if (x->parent == (x->parent->parent)->left) {
			u = (x->parent->parent)->right;

			if (u->color == RED) {
				(x->parent)->color = BLACK;
				u->color = BLACK;

				(x->parent->parent)->color = RED;

				x = x->parent->parent;
			}
			else {
				if (x == (x->parent)->right) {
					x = x->parent;
					left_rotate(r, x);

				}

				(x->parent)->color = BLACK;
				(x->parent->parent)->color = RED;

				right_rotate(r, (x->parent->parent));

			}
		}
		else {
			u = (x->parent->parent)->left;

			if (u->color == RED) {
				(x->parent)->color = BLACK;
				u->color = BLACK;

				(x->parent->parent)->color = RED;

				x = x->parent->parent;
			}
			else {
				if (x == (x->parent)->left) {
					x = (x->parent);

					right_rotate(r, x);
				}
				(x->parent)->color = BLACK;
				(x->parent->parent)->color = RED;

				left_rotate(r, (x->parent->parent));
			}
		}
	}

	(r->r)->color = BLACK;
}

void insert_node(RBTPtr r, int key) {

	NodePtr n = (NodePtr)malloc(sizeof(struct NODE));
	NodePtr temp = r->r;
	NodePtr p = r->nil;

	n->left = r->nil;
	n->right = r->nil;
	n->color = RED;
	n->key = key;


	while (temp != r->nil) {
		p = temp;

		if (key > temp->key) {
			temp = temp->right;
		}
		else {
			temp = temp->left;
		}
	}

	n->parent = p;

	if (p == r->nil) {
		r->r = n;
	}
	else if (key > p->key) {
		p->right = n;
	}
	else {
		p->left = n;
	}

	insert_fixup(r, n);

}

void tree_transplant(RBTPtr r, NodePtr t, NodePtr c) {
	if (t->parent == r->nil) {
		r->r = c;
	}
	else if (t == (t->parent)->left) {
		(t->parent)->left = c;
	}
	else {
		(t->parent)->right = c;
	}

	c->parent = t->parent;
}

void delete_fixup(RBTPtr r, NodePtr x) {
	NodePtr s = NULL; //sibling

	while ((x != r->r) && (x->color == BLACK)) {
		if (x == (x->parent)->left) {
			s = (x->parent)->right;

			if (s->color == RED) {
				s->color = BLACK;
				(x->parent)->color = RED;
				left_rotate(r, x->parent);

				s = (x->parent)->right;
			}

			if ((s->left)->color == BLACK && (s->right)->color == BLACK) {
				s->color = RED;
				x = x->parent;
			}
			else if ((s->left)->color == RED && (s->right)->color == BLACK) {
				s->color = RED;
				(s->left)->color = BLACK;
				right_rotate(r, s);

				s = (x->parent)->right;
			}

			if ((s->right)->color == RED) {
				s->color = (x->parent)->color;
				(s->right)->color = BLACK;
				(x->parent)->color = BLACK;
				left_rotate(r, x->parent);

				x = r->r;
			}
		}
		else {
			s = (x->parent)->left;

			if (s->color == RED) {
				s->color = BLACK;
				(x->parent)->color = RED;
				right_rotate(r, x->parent);

				s = (x->parent)->left;
			}

			if ((s->left)->color == BLACK && (s->right)->color == BLACK) {
				s->color = RED;
				x = x->parent;
			}

			else if ((s->right)->color == RED && (s->left)->color == BLACK) {
				s->color = RED;
				(s->right)->color = BLACK;
				left_rotate(r, s);

				s = (x->parent)->left;
			}

			if ((s->left)->color == RED) {
				s->color = (x->parent)->color;
				(s->left)->color = BLACK;
				(x->parent)->color = BLACK;
				right_rotate(r, x->parent);

				x = r->r;
			}
		}
	}

	x->color = BLACK;
}

void delete_node(RBTPtr r, int key) {
	NodePtr target = r->r;
	NodePtr temp = NULL;
	NodePtr x = NULL;
	char t_clr;


	while (key != target->key) {
		if (target->key > key) {
			target = target->left;
		}
		else {
			target = target->right;
		}
	}
	t_clr = target->color;

	if (target->left == r->nil) {
		x = target->right;
		tree_transplant(r, target, target->right);
	}
	else if (target->right == r->nil) {
		x = target->left;
		tree_transplant(r, target, target->left);
	}
	else {
		temp = target->right;

		while (temp->left != r->nil) {
			temp = temp->left;
		}

		t_clr = temp->color;
		x = temp->right;

		tree_transplant(r, temp, temp->right);
		temp->right = target->right;
		(temp->right)->parent = temp;

		tree_transplant(r, target, temp);
		temp->left = target->left;
		(temp->left)->parent = temp;
		temp->color = target->color;
	}

	if (t_clr == BLACK) {
		delete_fixup(r, x);
	}

	free(target);
}

int search(RBTPtr r, int target) {
	NodePtr current = r->r;
	int check = 1;

	if (current == r->nil) {
		check = 0;
		return check;
	}
	while (current->key != target) {
		if (current->key > target) {
			if (current->left != r->nil) {
				current = current->left;
			}
			else {
				check = 0;
				return check;
			}
		}
		else {
			if (current->right != r->nil) {
				current = current->right;
			}
			else {
				check = 0;
				return check;
			}
		}
	}

	return check = 1;
}

int BlackNode_Height(RBTPtr r, NodePtr tree) {
	int h = 0;

	while (tree != r->nil) {
		if (tree->color == BLACK) {
			h++;
		}
		tree = tree->right;
	}

	return h;
}

void inorder_traversal(RBTPtr r, NodePtr tree) {
	if (tree == r->nil) {
		return;
	}
	else {
		inorder_traversal(r, tree->left);
		printf("%d %c \n", tree->key, tree->color == 1 ? 'B' : 'R');
		inorder_traversal(r, tree->right);
	}
}

int HowManyBN(RBTPtr r, NodePtr tree) {
	if (tree == r->nil) {
		return 0;
	}
	else {
		if (tree->color == BLACK) {
			BN++;
		}
		HowManyBN(r, tree->left);
		HowManyBN(r, tree->right);
	}
	return BN;
}

NodePtr FIND(RBTPtr r, int target) {
	NodePtr current = r->r;

	if (current == r->nil)
		return current;

	while (current->key != target && current != r->nil) {
		if (current->key > target) {
			if (current->left != r->nil)
				current = current->left;
			else {
				break;
			}
		}
		else {
			if (current->right != r->nil)
				current = current->right;
			else {
				break;
			}
		}
	}

	return current;
}

NodePtr Minimum(RBTPtr r, NodePtr find) {
	while (find->left != r->nil) {
		find = find->left;
	}
	return find;
}

NodePtr Maximum(RBTPtr r, NodePtr find) {
	while (find->right != r->nil) {
		find = find->right;
	}
	return find;
}

NodePtr Successor(RBTPtr r, NodePtr find, int target) {
	if (find->right != r->nil)
		return Minimum(r, find->right);
	NodePtr y = find->parent;
	while (y != r->nil && find == y->right) {
		find = y;
		y = y->parent;
	}
	return y;
}

NodePtr Predecessor(RBTPtr r, NodePtr find, int target) {
	if (find->left != r->nil)
		return Maximum(r, find->left);

	NodePtr y = find->parent;
	while (y != r->nil && find == y->left) {
		find = y;
		y = y->parent;
	}
	return y;
}

int* ThreeThing(RBTPtr r, int target) {
	NodePtr find = FIND(r, target);
	//int MIN, MAX;
	NodePtr min = NULL;
	NodePtr max = NULL;

	static int result[3];

	if (find->key != target) {
		if (find->left == r->nil || find->right == r->nil) {
			if (target < find->key && target < find->parent->key) {
				//printf("1. ");
				min = r->nil;
				max = find;
			}

			if (target < find->key && target > find->parent->key) {
				//printf("2. ");
				min = Predecessor(r, find, target);
				max = find;
			}

			if (target > find->key && target < find->parent->key) {
				//printf("3. ");
				min = find;
				max = Successor(r, find, target);
			}

			if (target > find->key && target > find->parent->key) {
				//printf("4. ");
				min = find;
				max = r->nil;
			}
		}

	}
	else {
		//printf("5. ");
		min = Predecessor(r, find, target);
		max = Successor(r, find, target);
	}


	//printf("%d, %d \n", find->key, target);

	if (min == r->nil)
		result[0] = 0;
	else
		result[0] = min->key;

	if (search(r, target) == 0)
		result[1] = 0;
	else
		result[1] = find->key;

	if (max == r->nil)
		result[2] = 0;
	else
		result[2] = max->key;

	return result;
}

void rbt_print(RBTPtr self, NodePtr tree, int level) {
	if (tree->right != self->nil)
		rbt_print(self, tree->right, level + 1);
	for (int i = 0; i < level; i++)
		printf("    ");
	printf("%d", tree->key);
	if (tree->color == RED)
		printf("[R]\n");
	else
		printf("[B]\n");
	if (tree->left != self->nil)
		rbt_print(self, tree->left, level + 1);
}

void main(int argc, char **argv) {
	FILE *fp;
	FILE *sp;
	FILE *op;
	int data;

	int Node_num = 0;
	int insert = 0;
	int miss = 0;
	int del = 0;

	RBTPtr rbt = rbt_alloc();

	fp = fopen("test01.txt", "r");

	while (fscanf(fp, "%d", &data) != EOF) {
		if (data == 0) break;
		else if (data > 0) {
			insert_node(rbt, data);
			Node_num += 1;
			insert += 1;
		}
		else {
			data = data * (-1);

			if (search(rbt, data) == 1) {
				delete_node(rbt, data);
				Node_num -= 1;
				del += 1;
			}
			else {
				miss += 1;
			}
		}
	}
	fclose(fp);

	sp = fopen("search01.txt", "r");
	op = fopen("output.txt", "w");
	int *result;
	while (fscanf(sp, "%d", &data) != EOF) {
		if (data == 0) break;
		else {
			result = ThreeThing(rbt, data);
			for (int i = 0; i < 3; i++) {
				if (result[i] == 0) {
					fprintf(op, "NIL ");
				}
				else
					fprintf(op, "%d ", result[i]);
			}
		}
		fprintf(op ,"\n");
	}

	fclose(op);
	fclose(sp);

	printf("total = %d \n", Node_num);
	printf("insert = %d \n", insert);
	printf("deleted = %d \n", del);
	printf("miss = %d \n", miss);

	printf("nb = %d \n", HowManyBN(rbt, rbt->r));
	printf("bh = %d \n", BlackNode_Height(rbt, rbt->r));

	inorder_traversal(rbt, rbt->r);

	rbt_print(rbt, rbt->r, 0);

	return;
}