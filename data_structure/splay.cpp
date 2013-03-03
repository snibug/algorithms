#include <vector>
#include <algorithm>

typedef int ValueType;

struct node {
	ValueType value;
	struct node *parent;
	struct node *child[2];
	node(const ValueType &_value) : value(_value), parent(NULL){child[0] = NULL; child[1] = NULL;}
};
struct splaytree{
	node *root;
	int cnt;
	splaytree() : root(NULL),cnt(0) {}
	splaytree(node *_root) : root(_root),cnt(0) {}
	// left : 0, right : 1
	inline node *cut(int idx, node *par) {node *child = par->child[idx]; if(child) child->parent = NULL;par->child[idx] = NULL; return child;}
	inline node *connect(int idx, node *par,node *child) {par->child[idx]=child; if(child) child->parent=par; return child;}
	void splay(const ValueType &dest) {
		if(root == NULL) return;
		node *LR[2] = {NULL,NULL}, *bLR[2] = {NULL,NULL};
		while(true) {
			int dir;
			bool zigzig = 0;
			node *next;
			if(root->value < dest) {
				if(root->child[1] == NULL) break;
				dir = 1;
				next = root->child[dir];
				zigzig = (next->value < dest);
			}else if(dest < root->value) {
				if(root->child[0] == NULL) break;
				dir = 0;
				next = root->child[dir];
				zigzig = (dest < next->value);
			}else {
				break;
			}
			if(zigzig && next->child[dir] != NULL){
				//zig-zig prepare
				next = next->child[dir];
				connect(dir,root,next->parent->child[dir^1]);
				connect(dir^1,next->parent,root);
			}
			//zig
			if(LR[dir^1] == NULL) LR[dir^1] = bLR[dir^1] = next->parent;
			else bLR[dir^1] = connect(dir,bLR[dir^1],next->parent);
			cut(dir,next->parent);
			root = next;
		}
		//reassemble tree
		for(int i = 0;i < 2;i ++){
			if(LR[i] && root->child[i]) connect(i^1,bLR[i],root->child[i]);
			if(LR[i]) connect(i,root,LR[i]);
		}
	}
	void insert(node *youralloc) {
		const ValueType &dest = youralloc->value;
		node *cur = root;
		if(cur == NULL) {
			cnt ++;
			root = youralloc;
			return;
		}
		while(true){
			int dir;
			if(dest < cur->value) {
				dir = 0;
			}else if(cur->value < dest) {
				dir = 1;
			}else{
				splay(dest);
				return;
			}
			if(cur->child[dir] == NULL) {
				cnt ++;
				connect(dir,cur,youralloc);
				splay(dest);
				return;
			}
			cur = cur->child[dir];
		}
	}

	node *getminmax(int idx) {
		node *cur = root;
		if(cur == NULL)
			return cur;
		while(cur->child[idx] != NULL)
			cur = cur->child[idx];
		splay(cur->value);
		return cur;
	}
	node *getmin() { return getminmax(0); }
	node *getmax() { return getminmax(1); }

	node *prevnext(int idx, node *cur) {
		node *ret = NULL;
		splay(cur->value);
		if(cur->child[idx] != NULL) {
			ret = splaytree(cut(idx,cur)).getminmax(idx^1);
			connect(idx,cur,ret);
			splay(ret->value);
		}
		return ret;
	}
	node *prev(node *cur) { return prevnext(0,cur); }
	node *next(node *cur) { return prevnext(1,cur); }

	node *popminmax(int idx) {
		node *ret = getminmax(idx);
		root = cut(idx^1,ret);
		cnt --;
		return ret;
	}
	node *popmin() { return popminmax(0); }
	node *popmax() { return popminmax(1); }

	node *find(const ValueType &dest) {
		if(root == NULL) return NULL;
		splay(dest);
		if(root->value == dest) return root;
		return NULL;
	}
	void erase(node *cur) {
		splay(cur->value);
		node *L = root->child[0], *R = root->child[1];
		if(L) cut(0,root); if(R) cut(1,root);
		cnt --;
		if(L) {
			splaytree leftSplay(L);
			root = leftSplay.getmax();
			if(R) connect(1,root, R);
		}else{
			root = R;
		}
	}
};



#include <cstdio>

int main()
{
	splaytree tmp;
	for(int i = 0;i < 10000000;i ++){
		tmp.insert(new node(i));
	}
	node *iter = tmp.getmin();
	for(int i = 0;i < 10000000;i ++){
		iter = tmp.next(iter);
	}
	iter = tmp.getmax();
	for(int i = 0;i < 10000000;i ++){
		iter = tmp.prev(iter);
	}
	for(int i = 0;i < 10000000;i ++){
		if(rand()%2){
			delete tmp.popmax();
		}else{
			delete tmp.popmin();
		}
	}
	tmp.insert(new node(3));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(4));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(7));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(2));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(5));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(15));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(9));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(1));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.erase(tmp.find(1));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.erase(tmp.find(7));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.erase(tmp.find(15));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.erase(tmp.find(3));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.erase(tmp.find(4));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.erase(tmp.find(2));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.erase(tmp.find(5));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.erase(tmp.find(9));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");


	tmp.insert(new node(3));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(4));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(7));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(2));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(5));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(15));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(9));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.insert(new node(1));
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.popmin();
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.popmin();
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.popmax();
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.popmax();
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.popmin();
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.popmax();
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.popmax();
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");
	tmp.popmin();
	for(node *I = tmp.getmin(); I != NULL; I = tmp.next(I))printf("%d,",I->value);printf("\n");

	return 0;
}


