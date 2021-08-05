//序列化和反序列化
//增删改查

#define M (4) 
#define MIN (M % 2 ? M / 2 + 1 : M/2) 

#include<stdio.h>
#include <stdlib.h>
#include<time.h>

typedef int keyType;
typedef struct BPlusNode *BPlusTree, *Position;
struct BPlusNode{
	bool isLeaf;
	int keynum;
	keyType key[M + 1];
	BPlusTree children[M + 1];
	BPlusTree next;
	BPlusTree prior;
};


//生成新B+树节点
BPlusTree NewBPlusNode(){
	BPlusTree p;
	int i;
	p = (BPlusTree)malloc(sizeof(BPlusNode));
	if (p == NULL){
		printf("???????????\n");
		return p;
	}

	i = 0;
	for (int i = 0; i < M + 1; i++){
		p->key[i] = -1;
		p->children[i] = NULL;
	}
	p->isLeaf = true;
	p->keynum = 0;
	p->next = NULL;
	p->prior = NULL;
	return p;
}


//初始化
BPlusTree Initialize(){
	BPlusTree root;
	root = NewBPlusNode();
	return root;
}


//将e插入哪个分支  例如将3插入1 2 4  则i= 1,也就是放在2里面
int findPosition(BPlusTree p, keyType e){
	int i;
	for (i = p->keynum - 1; i >= 0; i--){
		if (p->key[i] == e)
			return -2;
		if (p->key[i] < e)
			break;
	}
	return i;
}


//结点分裂
BPlusTree Split(BPlusTree p, BPlusTree parent){
	BPlusTree q;
	int mid = p->keynum / 2, i, j;
	q = NewBPlusNode();        
	q->isLeaf = p->isLeaf;
	for (i = mid, j = 0; i < M + 1; i++, j++){
		q->key[j] = p->key[i];
		if (p->isLeaf == false){
			q->children[j] = p->children[i];
			p->children[i] = NULL;
		}
		p->keynum--;
		q->keynum++;
	}
	if (p->isLeaf == true){
		q->prior = p;
		if (p->next == NULL){
			p->next = q;
			q->next = NULL;
		}
		else{
			q->next = p->next;
			p->next = q;
			q->next->prior = q;
		}
	}

	if (parent == NULL){
		//没有父节点，需要加层
		parent = NewBPlusNode();
		parent->keynum = 2;
		parent->isLeaf = false;
		parent->key[0] = p->key[0];
		parent->key[1] = p->key[mid];
		parent->children[0] = p;
		parent->children[1] = q;
	}
	else{   //有父节点，直接加进去
		for (i = parent->keynum - 1; i > findPosition(parent, q->key[0]); --i){
			parent->key[i + 1] = parent->key[i];
			parent->children[i + 1] = parent->children[i];
		}
		parent->key[i + 1] = q->key[0];
		parent->children[i + 1] = q;
		parent->keynum++;
	}
	return parent;    //***
}

BPlusTree InsertT(BPlusTree p, keyType e, BPlusTree parent){
	int mid = p->keynum / 2;
	int i;
	int inpos = findPosition(p, e);
	if (inpos == -2)
	{
		printf("存在%d\n", e);
		if(parent != NULL)
			return parent;
		else
			return p;
	}
	if (p->isLeaf == true){
		//先直接插入
		for (i = p->keynum - 1; i > inpos; i--)
			p->key[i + 1] = p->key[i];
		p->key[i + 1] = e;
		p->keynum++;

		if (p->keynum > M)
			//结点已满，需要分裂
			return Split(p, parent);
		if (parent == NULL)
			return p;
		else
			return parent;
	}
	else{
		if (inpos == -1)
			inpos = 0;
		//parent = p;
		p = InsertT(p->children[inpos], e, p);     //甩给子结点插入，返回更新后的p
		for(i = 0;i < p->keynum;i++){
			if(p->key[i] != p->children[i]->key[0])
				p->key[i] = p->children[i]->key[0];
		}
		//插入后父节点是否需要继续分裂
		if (p->keynum > M)
			p = Split(p, parent);    //***
		if (parent != NULL)
			return parent;
	}
	return p;
}


BPlusTree Insert(BPlusTree T, keyType e){
	return InsertT(T, e, NULL);
}

BPlusTree Search(BPlusTree p, keyType e){
	int inpos;
	if (p->isLeaf == true){
		inpos = findPosition(p, e);
		if (inpos == -2)
			return p;
		else
			return NULL;
	}
	else{
		for (inpos = p->keynum - 1; inpos >= 0; inpos--){
			if (p->key[inpos] <= e)
				break;
		}
		if(inpos == -1)
			return NULL;
		else
			return Search(p->children[inpos], e);
	}
}

BPlusTree RemoveElement(BPlusTree p, BPlusTree parent, int inpar){
	int i;
	for(i = inpar + 1;i < parent->keynum;i++){
		parent->key[i-1] = parent->key[i];
		parent->children[i-1] = parent->children[i];
	}
	parent->children[i] = NULL;
	parent->keynum--;
	if(p->isLeaf == true){
		if(p->next != NULL){
			p->prior->next = p->next;
			p->next->prior = p->prior;
		}else
			p->prior->next = NULL;
	}
	free(p);
	return parent;
}


BPlusTree Merge(BPlusTree p, BPlusTree q , BPlusTree parent, int inpar){
	int i, j;
	if(parent->children[inpar - 1] == q){
		for(i = q->keynum,j = 0;j < p->keynum;i++, j++){
			q->key[i] = p->key[j];
			if(p->isLeaf == false)
				q->children[i] = p->children[j];
			q->keynum++;
		}
		parent = RemoveElement(p, parent, inpar);
	}
	else if(parent->children[inpar + 1] == q){
		for(i = p->keynum, j = 0;j < q->keynum;i++, j++){
			p->key[i] = q->key[j];
			if(p->isLeaf == false)
				p->children[i] = q->children[j];
			p->keynum++;
		}
		parent = RemoveElement(q, parent, inpar+1);
	}
	return parent;
}

//寻找兄弟
BPlusTree FindSiblingAndLendOrMerge(BPlusTree p, BPlusTree parent, int inpar){
	BPlusNode *q;
	int i;
	if(inpar > 0 && parent->children[inpar-1]->keynum > MIN)
		q = parent->children[inpar-1];
	else if(inpar < parent->keynum - 1 && parent->children[inpar+1]->keynum > MIN)
		q =  parent->children[inpar+1];
	//找兄弟
	else if(inpar > 0)
		q = parent->children[inpar-1];
	else if(inpar < parent->keynum - 1)
		q = parent->children[inpar+1];
	if(q->keynum > MIN){
		if(inpar - 1 >= 0){
			if(parent->children[inpar - 1] == q){    //判断拿的是左兄弟还是右兄弟 
			//拿的左
			for(i = p->keynum - 1;i >= 0;i--){
				p->key[i + 1] = p->key[i];
				if(p->isLeaf == false){
					p->children[i + 1] = p->children[i];
				}
			}
			p->key[0] = q->key[q->keynum - 1];
			q->key[q->keynum - 1] = -1;
			if(p->isLeaf == false){
				p->children[0] = q->children[q->keynum - 1];
				q->children[q->keynum - 1] = NULL;
			}
			p->keynum++;
			q->keynum--;
			//往上p的所有结点都要改成q->key[0];
			}
		}
		if(parent->children[inpar + 1] == q){ 
		//拿的右
			p->key[p->keynum] = q->key[0];
			if(p->isLeaf == false)
				p->children[p->keynum] = q->children[0];
			for(i = 0;i < q->keynum - 1;i++){
				q->key[i] = q->key[i + 1];
				if(p->isLeaf == false)
					q->children[i] = q->children[i + 1];
			}
			q->key[q->keynum - 1] = -1;
			if(p->isLeaf == false)
				q->children[q->keynum - 1] = NULL;
			p->keynum++;
			q->keynum--;
			//往上q的所有结点都要改成p->key[0];
		}
		return parent;
	}else{
		return Merge(p, q, parent, inpar);
	}
}


BPlusTree RemoveT(BPlusTree p, keyType e, BPlusTree parent){
	int inpos = 0, i, inpar = 0;
	BPlusNode *q;
	
	if(parent == NULL && p->isLeaf == true){
		for (i = inpos; i < p->keynum - 1; i++)
			p->key[i] = p->key[i + 1];
		--p->keynum;
		return p;
	}
	
	if(parent != NULL){
		for(inpar = 0;inpar < parent->keynum;inpar++){   //p在父节点中的位置
			if(parent->children[inpar] == p)
				break;
		}
	}
	
	if(p->isLeaf == true){
		//是叶子结点则开始删除
		while(p->key[inpos] != e && inpos < p->keynum)     //p中要删除的结点位置
			++inpos;
		if(inpos == p->keynum){
			printf("不存在%d\n", e);
			return parent;
		}else{    //直接删除 
			for (i = inpos; i < p->keynum - 1; i++)
				p->key[i] = p->key[i + 1];
			--p->keynum;
		}
		if(p->keynum < MIN){
			return FindSiblingAndLendOrMerge(p, parent, inpar);
		}else
			return parent;
	}else{    //不是子结点，在他的子结点去删除
		for (inpos = p->keynum - 1; inpos >= 0; inpos--){
			if (p->key[inpos] <= e)
				break;
		}
		if(inpos == -1){    //只可能在根节点最左的那个孩子出现这种情况
			printf("不存在数%d\n", e);
			return p;
		}
		p = RemoveT(p->children[inpos], e, p);
		for(i = 0;i < p->keynum;i++){
			if(p->key[i] != p->children[i]->key[0])
				p->key[i] = p->children[i]->key[0];
		}

		if(parent != NULL){
			if(p->keynum < MIN){
				p = FindSiblingAndLendOrMerge(p, parent, inpar);
			}
			else
				return parent;
		}
		if(parent == NULL){
			if(p->keynum < 2){
				//降层
				q = p->children[0];
				free(p);
				return q;
			}
		}
	}
	return p;
}


BPlusTree Remove(BPlusTree T, keyType e){
	return RemoveT(T, e, NULL);
}

int main(void){
	//	int choose;
	keyType e;
	double duration = 0;
	clock_t  start, stop;
	BPlusTree T;
	T = Initialize();
	//printf("1.插入    2.删除    3.查询\n");
	int r;
	//start = clock(); 

	for (int i = 0; i < 10000; i++){
		r = rand() % 1000;
		T = Insert(T, r);
	}

	for (int i= 0; i < 10000; i++){
		r = rand() % 1000;
		T = Remove(T, r);
	}

	//stop = clock();
	//duration = ((double)(stop - start)) / CLOCKS_PER_SEC;
	//printf("time:%f\n", duration);


	// //查询测试:q
	// if (Search(T, e) != NULL)
	// 	printf("树中有%d\n", e);
	// else
	// 	printf("树中没有%d\n", e);

	// //删除测试
	// T = Remove(T, 35);
	// T = Remove(T, 15); 
	// T = Remove(T, 5);
	// T = Remove(T, 72);
	// T = Remove(T, 83); 
	// T = Remove(T, 90); 



	//	scanf("%d", &choose);
	//	if (choose == 1){
	//		printf("请输入要插入的数");
	//		printf("\n");
	//		scanf("%d", &e);
	//		start = clock(); 
	//		T = Insert(T, e);
	//		stop = clock();
	//		duration = ((double)(stop - start)) / CLK_TCK;
	//		printf("time:%f\n", duration);
	//	}else if (choose == 2){
	//		T = Remove(T, e);
	//	}else if(choose == 3){
	//		if(Search(T, e))
	//			printf("树中有%d\n", e);
	//		else
	//			printf("树中没有%d\n", e); 
	//	}else{
	//		printf("不合法的输入！\n") 
	//	}
	return 0;
}
