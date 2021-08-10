#include"BPlusTree.h"
using namespace std;

BPlusNode* BPlusTree::NewBPlusNode(){
    BPlusNode *p;
	int i;
    p = new BPlusNode;
	if (p == nullptr){
        std::cout << "空间分配错误" << std::endl;
		exit(1);
	}
	i = 0;
	for (int i = 0; i < M + 1; i++){
		p->key[i] = -1;
		p->children[i] = nullptr;
	}
	p->isLeaf = true;
	p->keynum = 0;
	p->next = nullptr;
	p->prior = nullptr;
	return p;
}

BPlusNode* BPlusTree::Initialize(){
	BPlusNode* root;
	root = NewBPlusNode();
	return root;
}

int BPlusTree::findPosition(BPlusNode *p, int e){
    int i;
	for (i = p->keynum - 1; i >= 0; i--){
		if (p->key[i] == e)
			return -2;
		if (p->key[i] < e)
			break;
	}
	return i;
}
        
BPlusNode* BPlusTree::Split(BPlusNode* p, BPlusNode* parent){
    BPlusNode *q;
	int mid = p->keynum / 2, i, j;
	q = NewBPlusNode();        
	q->isLeaf = p->isLeaf;
	for (i = mid, j = 0; i < M + 1; i++, j++){
		q->key[j] = p->key[i];
		if (p->isLeaf == false){
			q->children[j] = p->children[i];
			p->children[i] = nullptr;
		}
		p->keynum--;
		q->keynum++;
	}
	if (p->isLeaf == true){
		q->prior = p;
		if (p->next == nullptr){
			p->next = q;
			q->next = nullptr;
		}
		else{
			q->next = p->next;
			p->next = q;
			q->next->prior = q;
		}
	}

	if (parent == nullptr){
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
        
BPlusNode* BPlusTree::InsertT(BPlusNode* p, int e, BPlusNode* parent){
	int mid = p->keynum / 2;
	int i;
	int inpos = findPosition(p, e);
	if (inpos == -2)
	{
		//cout << "存在" << e << endl;
		if(parent != nullptr)
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
        
BPlusNode* BPlusTree::Insert(BPlusNode* T, int e){
    return InsertT(T, e, nullptr);
}
        
BPlusNode* BPlusTree::Search(BPlusNode* p, int e){
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
        
BPlusNode* BPlusTree::RemoveElement(BPlusNode* p, BPlusNode* parent, int inpar){
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
	delete p;
	return parent;
}
        
BPlusNode* BPlusTree::Merge(BPlusNode* p, BPlusNode* q , BPlusNode* parent, int inpar){
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
        
BPlusNode* BPlusTree::FindSiblingAndLendOrMerge(BPlusNode* p, BPlusNode* parent, int inpar){
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
				q->children[q->keynum - 1] = nullptr;
			p->keynum++;
			q->keynum--;
			//往上q的所有结点都要改成p->key[0];
		}
		return parent;
	}else{
		return Merge(p, q, parent, inpar);
	}
}
        
BPlusNode* BPlusTree::RemoveT(BPlusNode* p, int e, BPlusNode* parent){
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
				delete p;
				return q;
			}
		}
	}
	return p;
}
        
BPlusNode* BPlusTree::Remove(BPlusNode* T, int e){
    return RemoveT(T, e, nullptr);
}
        
std::string BPlusTree::Serialize(BPlusNode* p){
    int i, a;
	std::string str;
	(p->isLeaf == false) ? a = 0 : a = 1;
	a += p->keynum * 10;
	str = std::to_string(a);    //*将keynum和关键字分开
	for(i = 0;i < p->keynum;i++){
		str += "*" + std::to_string(p->key[i]);
	}
	str += '#';    //#表示一个结点结束
	if(p->isLeaf == false){
		for(i = 0;i < p->keynum;i++){
			str += Serialize(p->children[i]);
		}
	}
	return str;
}
        
std::queue<std::string> BPlusTree::SplitStr(std::string str, char ch){
    std::queue<std::string> que;
	std::string stc = str, s;
	int i;
	while(!stc.empty()){
		i = stc.find_first_of(ch);
		s = stc.substr(0, i);
		que.push(s);
		stc = stc.substr(i + 1);
	}
	return que;
}
        
BPlusNode* BPlusTree::DeSerialize(std::queue<std::string>& nodeQue){
    std::queue<std::string>  keyQue;
	std::string s;
	BPlusNode *p;
	int key = -1, i = 0;
	if(nodeQue.empty())
		return NULL;
	s = nodeQue.front();
	nodeQue.pop();
	s += '*';
	keyQue = SplitStr(s, '*');
	if(!keyQue.empty()){
		key = atoi(keyQue.front().c_str());
		keyQue.pop();
		p = NewBPlusNode();
		if(key % 10 == 0)
			p->isLeaf = false;
		p->keynum = key / 10;
	}
	while(!keyQue.empty()){
		key = atoi(keyQue.front().c_str());
		keyQue.pop();
		p->key[i] = key;
		if(p->isLeaf == false){
			p->children[i] = DeSerialize(nodeQue);
		}
			

		i++;
	}
	return p;
}

int main(void){
	int e;
	double duration = 0;
    int r;
	clock_t  start, stop;
	BPlusNode *T;
    BPlusTree bpt;
    //
	T = bpt.Initialize();
	start = clock(); 

	// for (int i = 0; i < 15; i++){
	// 	r = rand() % 100;
	// 	T = bpt.Insert(T, r);
	// }

	// // 插入测试
	// for (int i = 0; i < 10000; i++){
	// 	r = rand() % 10000;
	// 	T = bpt.Insert(T, r);
	// }

	// // 删除测试
	// for (int i= 0; i < 10000; i++){
	// 	r = rand() % 10000;
	// 	T = bpt.Remove(T, r);
	// }

	// //时间测试
	// stop = clock();
	// duration = ((double)(stop - start)) / CLOCKS_PER_SEC;
	// printf("time:%f\n", duration);

	// //查询测试:
	// if (bpt.Search(T, e) != NULL)
	// 	printf("树中有%d\n", e);
	// else
	// 	printf("树中没有%d\n", e);

	// // 序列化测试
	// ofstream fout;
	// string str = bpt.Serialize(T);
	// fout.open("BPTree/tree.txt");
	// fout << str;
	// fout.close();

	// // 反序列化测试
	// ifstream fin;
	// fin.open("BPTree/tree.txt");
	// string str;
	// getline(fin, str);
	// fin.close();
	// queue<string>  nodeQue;
	// nodeQue = SplitStr(str, '#');
	// T = DeSerialize(nodeQue);

	return 0;
}