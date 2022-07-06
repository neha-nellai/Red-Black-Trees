#include <stdio.h>
#include <stdlib.h>

enum COLOR {Red, Black};

typedef struct tree_node {
    int data;
    struct tree_node *right;
    struct tree_node *left;
    struct tree_node *parent;
    enum COLOR color;
}tree_node;

typedef struct red_black_tree {
    tree_node *root;
    tree_node *NIL;
}red_black_tree;

tree_node* new_tree_node(int data) {
    tree_node* n = malloc(sizeof(tree_node));
    n->left = NULL;
    n->right = NULL;
    n->parent = NULL;
    n->data = data;
    n->color = Red;
    
    return n;
}

red_black_tree* new_red_black_tree() {
    red_black_tree *t = malloc(sizeof(red_black_tree));
    tree_node *nil_node = malloc(sizeof(tree_node));
    nil_node->left = NULL;
    nil_node->right = NULL;
    nil_node->parent = NULL;
    nil_node->color = Black;
    nil_node->data = 0;
    t->NIL = nil_node;
    t->root = t->NIL;
    
    return t;
}

tree_node* search(tree_node* root, int x){
    if(root==NULL || root->data==x) //if root->data is x then the element is found
        return root;
    else if(x>root->data) // x is greater, so we will search the right subtree
        return search(root->right, x);
    else //x is smaller than the data, so we will search the left subtree
        return search(root->left,x);
}

tree_node* find_minimum(tree_node* root,red_black_tree *t){
    if(root == NULL)
        return NULL;
    else if(root->left!= t->NIL) // node with minimum value will have no left child
        return find_minimum(root->left,t); // left most element will be minimum
    return root;
}

tree_node* find_maximum(tree_node* root,red_black_tree *t){
    if(root == NULL)
        return NULL;
    else if(root->right!= t->NIL) // node with maximum value will have no right child
        return find_maximum(root->right,t); // Right most element will be maximum
    return root;
}

void left_rotate(red_black_tree *t, tree_node *x) {
    tree_node *y = x->right;
    x->right = y->left;
    if(y->left != t->NIL) {
      y->left->parent = x;
    }
    y->parent = x->parent;
    if(x->parent == t->NIL) { //x is root
      t->root = y;
    }
    else if(x == x->parent->left) { //x is left child
      x->parent->left = y;
    }
    else { //x is right child
      x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void right_rotate(red_black_tree *t, tree_node *x) {
    tree_node *y = x->left;
    x->left = y->right;
    if(y->right != t->NIL) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if(x->parent == t->NIL) { //x is root
        t->root = y;
    }
    else if(x == x->parent->right) { //x is left child
        x->parent->right = y;
    }
    else { //x is right child
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

void insertion_fixup(red_black_tree *t, tree_node *z) {
    while(z->parent->color == Red) {//Property voilated: RED-RED conflict
        if(z->parent == z->parent->parent->left) { //z.parent is the left child
    
            tree_node *y = z->parent->parent->right; //uncle of z
        
            if(y->color == Red) { //case 1: uncle is RED => Recolor
                z->parent->color = Black; //Parent is made BLACK
                y->color = Black;         //Parent's sibling is also made BLACK
                z->parent->parent->color = Red; //Parent's parent is made RED
                z = z->parent->parent;
            }
            else { //case2 or case3: Uncle is BLACK
                if(z == z->parent->right) { //case2: LR rotation(triangle case)
                    z = z->parent; //marked z.parent as new z
                    left_rotate(t, z);
                }
                //case3: LL Right rotation required (straight line)
                z->parent->color = Black; //made parent black
                z->parent->parent->color = Red; //made parent's parent red
                right_rotate(t, z->parent->parent);
            }
        }
        else { //z.parent is the right child
            tree_node *y = z->parent->parent->left; //uncle of z
        
            if(y->color == Red) {
                z->parent->color = Black;
                y->color = Black;
                z->parent->parent->color = Red;
                z = z->parent->parent;
            }
            else {
                if(z == z->parent->left) {
                    z = z->parent; //marked z.parent as new z
                    right_rotate(t, z);
                }
                z->parent->color = Black; //made parent black
                z->parent->parent->color = Red; //made parent red
                left_rotate(t, z->parent->parent);
            }
        }
    }
    t->root->color = Black;
}

void insert(red_black_tree *t, tree_node *z) {
    tree_node* y = t->NIL; //variable for the parent of the added node
    tree_node* temp = t->root;
    
    while(temp != t->NIL) {
        y = temp;
        if(z->data < temp->data)
            temp = temp->left;
        else
            temp = temp->right;
    }
    z->parent = y;
    
    if(y == t->NIL) { //newly added node is root
        t->root = z;
    }
    else if(z->data < y->data) //data of child is less than its parent, left child
        y->left = z;
    else
        y->right = z;
    
    z->right = t->NIL;
    z->left = t->NIL;
    
    insertion_fixup(t, z);
}

void rb_transplant(red_black_tree *t, tree_node *u, tree_node *v) {
    if(u->parent == t->NIL)
        t->root = v;
    else if(u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

tree_node* minimum(red_black_tree *t, tree_node *x) {
    while(x->left != t->NIL)
        x = x->left;
    return x;
}

void rb_delete_fixup(red_black_tree *t, tree_node *x) {
while(x != t->root && x->color == Black) {
    if(x == x->parent->left) {
        tree_node *w = x->parent->right;
        if(w->color == Red) {
            w->color = Black;
            x->parent->color = Red;
            left_rotate(t, x->parent);
            w = x->parent->right;
        }
        if(w->left->color == Black && w->right->color == Black) {
            w->color = Red;
            x = x->parent;
        }
        else {
            if(w->right->color == Black) {
                w->left->color = Black;
                w->color = Red;
                right_rotate(t, w);
                w = x->parent->right;
            }
            w->color = x->parent->color;
            x->parent->color = Black;
            w->right->color = Black;
            left_rotate(t, x->parent);
            x = t->root;
        }
    }
    else {
    tree_node *w = x->parent->left;
    if(w->color == Red) {
        w->color = Black;
        x->parent->color = Red;
        right_rotate(t, x->parent);
        w = x->parent->left;
    }
    if(w->right->color == Black && w->left->color == Black) {
        w->color = Red;
        x = x->parent;
    }
    else {
        if(w->left->color == Black) {
        w->right->color = Black;
        w->color = Red;
        left_rotate(t, w);
        w = x->parent->left;
      }
      w->color = x->parent->color;
      x->parent->color = Black;
      w->left->color = Black;
      right_rotate(t, x->parent);
      x = t->root;
    }
  }
}
x->color = Black;
}

void rb_delete(red_black_tree *t, tree_node *z) {
tree_node *y = z;
tree_node *x;
enum COLOR y_orignal_color = y->color;
if(z->left == t->NIL) {
  x = z->right;
  rb_transplant(t, z, z->right);
}
else if(z->right == t->NIL) {
  x = z->left;
  rb_transplant(t, z, z->left);
}
else {
  y = minimum(t, z->right);
  y_orignal_color = y->color;
  x = y->right;
  if(y->parent == z) {
    x->parent = z;
  }
  else {
    rb_transplant(t, y, y->right);
    y->right = z->right;
    y->right->parent = y;
  }
  rb_transplant(t, z, y);
  y->left = z->left;
  y->left->parent = y;
  y->color = z->color;
}
if(y_orignal_color == Black)
  rb_delete_fixup(t, x);
}

void inorder(red_black_tree *t, tree_node *n) {
if(n != t->NIL) {
  inorder(t, n->left);
  printf("%d ", n->data);
  inorder(t, n->right);
}
}

void preorder(red_black_tree *t, tree_node *n) {
if(n != t->NIL) {
  printf("%d ", n->data);
  preorder(t, n->left);
  
  preorder(t, n->right);
}
}

void postorder(red_black_tree *t, tree_node *n) {
if(n != t->NIL) {
  postorder(t, n->left);
  
  postorder(t, n->right);
  printf("%d ", n->data);
}
}


int main() {
red_black_tree *t = new_red_black_tree();
int ch,x,y,z;
tree_node *a,*b,*c,*min,*max;
printf("\nRed Black Tree Implementation");
while(1){
    printf("\n1-Insertion\n2-Delete\n3-Search\n4-Display\n5-Find minimum\n6-Find maximum\n7-Exit");
    printf("\nEnter your choice: ");
    scanf("%d",&ch);
    switch(ch){
        case 1: printf("\nEnter the data to be inserted: ");
        scanf("%d",&x);
        a = new_tree_node(x);
        insert(t,a);
        break;
        case 2: printf("\nEnter the data to be deleted: ");
        scanf("%d",&y);
        b=search(t->root,y);
        if(b!=NULL)
          rb_delete(t, b);
        else
          printf("\nData not found in tree");
        break;
        case 3: printf("\nEnter the data to be searched: ");
        scanf("%d",&z);
        c=search(t->root,z);
        if(c!=NULL){
            printf("\nThe data is found");
        }
        else{
            printf("\nThe data is not present in the tree");
        }
        break;
        case 4: printf("\nThe Inorder traversal of tree: ");
        inorder(t, t->root);
        printf("\nThe Preorder traversal of tree: ");
        preorder(t, t->root);
        printf("\nThe Postorder traversal of tree: ");
        postorder(t, t->root);
        break;
        case 5:min=find_minimum(t->root,t);
        printf("\nThe minimum element present in the tree: %d",min->data);
        break;
        case 6:max=find_maximum(t->root,t);
        printf("\nThe maximum element present in the tree: %d",max->data);
        break;
        case 7: exit(0);
        break;
        default: printf("\nInvalid choice");
    }
}
return 0;
}
