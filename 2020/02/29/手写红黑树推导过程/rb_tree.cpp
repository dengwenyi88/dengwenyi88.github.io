// rb_tree.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <queue>



typedef int KEY;

enum COLOR {
	RED,
	BLACK,
};

struct RBNode {
	RBNode* parent;
	RBNode* left;
	RBNode* right;
	COLOR   color;
	KEY key;

	RBNode():parent(NULL)
		,left(NULL)
		,right(NULL)
		,color(RED)
	{}		
};

class RBTree {

public:
	RBTree() :m_root(NULL) {}
	RBNode* Insert(KEY key);
	void Print();
	bool Delete(RBNode* x);

private:
	bool LRotate(RBNode* x);
	bool RRotate(RBNode* x);
	bool InsertFixup(RBNode* z);
	bool DeleteFixup(RBNode* z);
	void Walk(RBNode* x);
	RBNode* GetMini(RBNode* x);
	RBNode* SuccessOR(RBNode* x);
private:
	RBNode* m_root;
};

void RBTree::Print() {
	this->Walk(this->m_root);
}

void RBTree::Walk(RBNode* x) {
	if (x == NULL) {
		return;
	}
	RBNode* tmp = x;
	
	//Walk(x->left);
	//Walk(x->right);
	std::queue<RBNode*> mq;
	mq.push(x);

	while (!mq.empty()) {
		tmp = mq.front();
		mq.pop();
		std::cout << tmp->key << " Color:[" << tmp->color << "]" << std::endl;
		if (tmp->left != NULL) {
			mq.push(tmp->left);
		}
		if (tmp->right != NULL) {
			mq.push(tmp->right);
		}
	}
}

bool RBTree::Delete(RBNode* z) {
	RBNode* x;
	RBNode* y;
	if (z == NULL) return false;
	if (z->left == NULL || z->right == NULL) {
		y = z;
	}
	else {
		y = this->SuccessOR(z);
	}
	// get y's only child
	if (y->left != NULL) {
		x = y->left;
	}
	else {
		x = y->right;
	}
	// remove y node
	if (x != NULL) {
		x->parent = y->parent;
	}
	// fixed y's parent
	if (y->parent == NULL) {
		m_root = x;
	} 
	else{
		if (y = y->parent->left) {
			y->parent->left = x;
		}
		else {
			y->parent->right = x;
		}
	}
	if (y != z) {
		z->key = y->key;
	}
	if (y->color == BLACK) {
		this->DeleteFixup(x);
	}
	if (y != NULL) {
		delete y;
	}
	return true;
}

RBNode* RBTree::SuccessOR(RBNode* x) {
	if (x == NULL) return NULL;
	if (x->right != NULL) {
		return this->GetMini(x->right);
	}
	RBNode* tmp = x;
	RBNode* y = tmp->parent;
	while (y != NULL && tmp == y->right) {
		x = y;
		y = y->parent;
	}
	return y;
}

RBNode* RBTree::GetMini(RBNode* x) {
	RBNode* tmp = x;
	if (tmp == NULL) return NULL;
	while (tmp->left != NULL) {
		tmp = tmp->left;
	}
	return tmp;
}

bool RBTree::DeleteFixup(RBNode* x) {
	if (x == NULL) return false;
	RBNode* w = NULL;

	while (x != m_root && x->color == BLACK) {
		if (x == x->parent->left) {
			w = x->parent->right;
			if (w->color == RED) {
				// 1. case 1
				x->parent->color = RED;
				LRotate(x->parent);
				w = x->parent->right;
			}
			if (w->left->color == BLACK && w->right->color == BLACK) {
				// 2. case 2
				w->color = RED;
				x = x->parent;
			}
			else {
				if (w->right->color == BLACK) {
					// 3. case 3
					w->left->color = BLACK;
					w->color = RED;
					RRotate(w);
					w = x->parent->right;
				}

				// 4. case 4
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				LRotate(x->parent);
				x = m_root;
			}
		}
		else {
			// if (x == x->parent->right)
			w = x->parent->left;
			// 1. case 1
			if (w->color == RED) {
				x->parent->color = RED;
				RRotate(x->parent);
				w = x->parent->left;
			}
			// 2. case 2
			if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->parent;
			}
			else {
				if (w->left->color == BLACK) {
					// 3. case 
					w->right->color = BLACK;
					w->color = RED;
					LRotate(w);
					w = x->parent->left;
				}

				// 4. case 4
				w->color = x->parent->color;
				x->parent->color = BLACK;
				x->left->color = BLACK;
				RRotate(x->parent);
				x = m_root;
			}
			
		}
		x->color = BLACK;
	}
	return true;
}


bool RBTree::InsertFixup(RBNode* z) {
	RBNode* y = NULL;
	if (z == NULL) return false;
	while (z != NULL && z->parent != NULL /*&& z->parent->parent!=NULL*/
		&& z->parent->color == RED) {
		if (z->parent == z->parent->parent->left) {
			// 1. z 's brother
			y = z->parent->parent->right;
			// 2. case 1
			if (y!=NULL && y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			}
			else {
				// BLACK
				if (z == z->parent->right) {
					// 3. case 2
					z = z->parent;
					this->LRotate(z);
				}

				// 4. case 3
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				//z = z->parent->parent;
				this->RRotate(z->parent->parent);	
			}
		}//if (z->parent == z->parent->parent->left)
		else if(z->parent == z->parent->parent->right) {			
			y = z->parent->parent->left;
			if (y!=NULL && y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			}
			else {				
				if (z == z->parent->left) {
					// 3. case 2
					z = z->parent;
					this->RRotate(z);
				}

				// 4. case 3
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				//z = z->parent->parent;
				this->LRotate(z->parent->parent);
			}
		}
	}
	if (m_root != NULL) {
		m_root->color = BLACK;
	}
	return true;
}

RBNode* RBTree::Insert(KEY key) {
	RBNode* y = NULL;
	RBNode* x = this->m_root;
	while (x != NULL) {
		y = x;
		if (key < x->key) {
			x = x->left;
		}
		else {
			x = x->right;
		}
	}

	RBNode* z = new RBNode;
	z->key = key;
	z->parent = y;
	if (y == NULL) {
		this->m_root = z;
	}
	else {
		if (z->key < y->key) {
			y->left = z;
		}
		else {
			y->right = z;
		}
	}
	z->left = NULL;
	z->right = NULL;
	z->color = RED;

	InsertFixup(z);
	return z;
}

bool RBTree::RRotate(RBNode* y) {
	RBNode* x;
	if (y == NULL) return false;
	x = y->left;
	if (x == NULL) return false;
	// 2. x's right
	y->left = x->right;
	if (x->right != NULL) {
		x->right->parent = y;
	}
	// 3. parent
	x->parent = y->parent;
	if (y->parent == NULL) {
		this->m_root = x;
	}
	else {
		if (y == y->parent->left) {
			y->parent->left = x;
		}
		else {
			y->parent->right = x;
		}
	}

	x->right = y;
	y->parent = x;
	return true;
}

bool RBTree::LRotate(RBNode* x) {
	RBNode* y;
	if (x == NULL) return false;
	// 1.
	y = x->right;
	if (y == NULL) return false;
	// 2.y's left
	x->right = y->left;
	if (y->left != NULL) {
		y->left->parent = x;
	}
	// 3. parent
	y->parent = x->parent;

	if (x->parent == NULL) {
		this->m_root = y;
	}
	else {
		if (x == x->parent->left) {
			x->parent->left = y;
		}
		else {
			x->parent->right = y;
		}
	}

	y->left = x;
	x->parent = y;
	return true;
}


int main()
{
	RBTree rb;

	rb.Insert(13);
	rb.Insert(8);
	RBNode* d = rb.Insert(17);
	rb.Insert(1);
	rb.Insert(11);
	rb.Insert(15);
	rb.Insert(25);
	rb.Insert(6);
	rb.Insert(22);
	rb.Insert(27);

	/*
	   A
	 C   F
	B D E G
	*/
	
	rb.Print();

	rb.Delete(d);
	rb.Print();
}

