#include <iostream>
#include <queue>
#include <stack>
#include <Windows.h>
using namespace std;


struct Key {//struktura kljuca,sadrzi i flag da li je validan kljuc i boju kljuca
	int key;
	bool f;
	Key() {
		f = false;
	}
};

class Node {
private:
	Node() {//podrazumijevani konstruktor za cvor
		for (int i = 0; i < 3; i++) keys[i].f = false;
		for (int i = 0; i < 4; i++) p[i] = nullptr;
		valid_keys = 0;
	}

	friend ostream& operator<<(ostream& it, const Node& node);//ispis cvora
	friend class BTree;
public:
	Key keys[3];
	Node* p[4];
	Node* parent;
	int valid_keys;
};

//friend fja za ispis cvora
ostream& operator<<(ostream& it, const Node& node) {
	for (int i = 0; i < 3; i++) {
		if (!node.keys[i].f) it << "*";
		else it << node.keys[i].key;
		if (i < 2) it << "|";
	}
	return it;
}

class BTree {
public:
	BTree() {//podrazumijevani konstruktor za stablo
		root = nullptr;
		numberOfKeys = 0;
	}
	~BTree();
	bool search(int key)const;//metod za pronalazak cvora
	Node* searchForInsert(int key) const;//metod koji pronalazi cvor gdje se umece novi kljuc;
	void insert(int key);//metoda za umetanje kljuca u stablo
	int getNumKeys()const { return numberOfKeys; }//vraca br kljuceva u stablu
	int getHeigh()const;//metoda koja vraca visinu stabla
	friend ostream& operator<<(ostream& it, const BTree& b);//ispis stabla
	friend class Node;
	friend class RedBlackTree;
private:
	Node* root;
	int numberOfKeys;

};


//destruktor za objekat stabla
BTree::~BTree() {
	if (root){
		queue<Node*> Queue;//koriscenje template reda
		Queue.push(root);
		while (!Queue.empty()) {
			Node* p = Queue.front();
			Queue.pop(); 
			for (int i = 0; i <=3; i++)if (p->p[i]) Queue.push(p->p[i]);
			delete p;
		}
		root = nullptr;
	}
}

//metoda za pretragu kljuca u stablu
bool BTree :: search(int key)const {
	Node* pom = root;
	while (pom) {
		if (pom->keys[1].key == key) return true;//uporedjujemo sa srednjim kljucem u cvoru
		else if (pom->keys[1].key > key) {//ako je trazeni kljuc manji gleda se li je kljuc na poziciji 0 validan
			if (pom->keys[0].f) {//ako je validan silazimo u odgovarajuce podstablo
				if (pom->keys[0].key == key) return true;
				else if (pom->keys[0].key > key) pom = pom->p[0];
				else pom = pom->p[1];
			}
			else pom = pom->p[1];//ako nije validan silazimo u cvor lijevo od jedinog kljuca(moguce da on postoji)
		}
		else {//ako je trazni kljuc veci od srednjeg gleda se da li je kljuc na poziciji 2 validan
			if (pom->keys[2].f) {//ako jeste silazimo u odgovarajuce podstablo
				if (pom->keys[2].key == key) return true;
				else if (pom->keys[2].key > key) pom = pom->p[2];
				else pom = pom->p[3];

			}
			else pom=pom->p[2];//ako nije validan silazi se u cvor desno od jedinog kljuca(moguce da on postoji)
		}
	}
	return false;//nije nadjen kljuc

}


//metod za pretragu cvora gdje treba da bude umetnut kljuc
Node* BTree::searchForInsert(int key)const {

	Node* pom = root, *q = nullptr;
	while (pom) {
		q = pom;
		if (pom->keys[1].key == key) return nullptr;//uporedjujemo sa srednjim kljucem u cvoru,ako je jednak nullptr
		else if (pom->keys[1].key > key) {//ako je trazeni kljuc manji gleda se li je kljuc na poziciji 0 validan
			if (pom->keys[0].f) {//ako je validan silazimo u odgovarajuce podstablo
				if (pom->keys[0].key == key) return nullptr;
				else if (pom->keys[0].key > key) pom = pom->p[0];
				else pom = pom->p[1];
			}
			else pom = pom->p[1];//ako nije validan silazimo u cvor lijevo od jedinog kljuca(moguce da on postoji)
		}
		else {//ako je trazni kljuc veci od srednjeg gleda se da li je kljuc na poziciji 2 validan
			if (pom->keys[2].f) {//ako jeste silazimo u odgovarajuce podstablo
				if (pom->keys[2].key == key) return nullptr;
				else if (pom->keys[2].key > key) pom = pom->p[2];
				else pom = pom->p[3];

			}
			else pom = pom->p[2];//ako nije validan silazi se u cvor desno od jedinog kljuca(moguce da on postoji)
		}
	}

	return q;//vraca se pok na oca ako se sidje u nullptr
}

//metod za umetanje kljuca
void BTree::insert(int key) {
	if (!root) {//stablo je prazno
		root = new Node();//stvara se korjen
		root->parent = nullptr;//otac na nullptr
		root->keys[1].key = key;//
		root->keys[1].f = true;//srednji kljuc je validan
		root->valid_keys++;//broj validnih kljuceva je 1
		numberOfKeys++;
		return;
	}
	else {
		Node *q = searchForInsert(key);//pok na cvor u koji se umece
		if (!q) return;//kljuc vec postoji,ne moze se umetnuti
		if (q->valid_keys < 3) {//umece se u LIST koji nije pun
			if (q->valid_keys == 1) {//list ima jedan kljuc(srednji)
				if (q->keys[1].key < key) {//umece se desno od srednjeg kljuca
					q->keys[2].key = key;
					q->keys[2].f = true;
					q->valid_keys++;
					numberOfKeys++;
					return;
				}
				else if (q->keys[1].key > key) {//umece se  lijevo od srednjeg kljuca
					q->keys[0].key = key;
					q->keys[0].f = true;
					q->valid_keys++;
					numberOfKeys++;
					return;
				}
			}
			else {//list ima 2 kljuca(treba sortirati kljuceve)
				if (q->keys[1].key<key && !q->keys[2].f) {//kljuc veci od srednjeg i pozicija desno od srednjeg slobodna
					q->keys[2].key = key;
					q->keys[2].f = true;
					q->valid_keys++;
					numberOfKeys++;
					return;
				}
				else if (q->keys[1].key>key && !q->keys[0].f) {//kljuc manji od srednjeg i pozicija lijevo od srednjeg slobodna
					q->keys[0].key = key;
					q->keys[0].f = true;
					q->valid_keys++;
					numberOfKeys++;
					return;
				}
				else if (q->keys[1].key > key) {	//umece se lijevo od srednjeg kljuca
					if (q->keys[0].key < key) {//umece se izmedju prvog i srednjeg
						q->keys[2].key = q->keys[1].key;
						q->keys[2].f = true;
						q->keys[1].key = key;
						q->valid_keys++;
						numberOfKeys++;
						return;
					}
					else {//umece se na mjesto prvog kljuca koje je zauzeto,pomjeranje prvog i srednjeg kljuca udesno
						q->keys[2].key = q->keys[1].key;
						q->keys[2].f = true;
						q->keys[1].key = q->keys[0].key;
						q->keys[0].key = key;
						q->valid_keys++;
						numberOfKeys++;
						return;;

					}
				}
				else {//umece se desno od srednjeg kljuca
					if (q->keys[2].key > key) {//umece se izmedju srednjeg i treceg kljuca
						q->keys[0].key = q->keys[1].key;
						q->keys[0].f = true;
						q->keys[1].key = key;
						q->valid_keys++;
						numberOfKeys++;
						return;

					}
					else {//umece se na mjesto treceg kljuca,srednji i treci se pomjeraju ulijevo
						q->keys[0].key = q->keys[1].key;
						q->keys[0].f = true;
						q->keys[1].key = q->keys[2].key;
						q->keys[2].key = key;
						q->valid_keys++;
						numberOfKeys++;
						return;

					}
				}

			}

		}
		else {//LIST PUN-PRELAMANJE!!!
			Node *l = nullptr;//pok na cvor koji se prenosi iz nivoa ispod(koji je nastao cijepanjem na nivou ispod),u pocetku je nullptr


			

			while (true) {

				int keyup = q->keys[1].key;//kljuc koji se propagira gore,srednji kljuc u cvoru u koji se umece

				Node* newnode = new Node();//pok na cvor koji nastaje cijepanjem;

				newnode->keys[1] = q->keys[2];//saljemo desni kljuc iz starog cvora kao lijevi,kao i njegov flag
				newnode->valid_keys++;
				//prebacivanje pokazivaca u novi cvor i azuriranje oceva
				newnode->p[2] = q->p[3];
				q->p[3] = nullptr;
				if (newnode->p[2]) newnode->p[2]->parent = newnode;

				newnode->p[1] = q->p[2];
				q->p[2] = nullptr;
				if (newnode->p[1]) newnode->p[1]->parent = newnode;

				//prebacivanje kljuca u starom cvoru kao srednjeg i azuriranje pokazivaca(oca ne treba
				q->keys[2].f = false;//ponistavanje desnog kljuca koji je prebacen u novi cvor
				q->valid_keys -= 2;//smanjivanje broja validnih kljuceva
				q->keys[1] = q->keys[0];//prebacivanje lijevog kljuca u starom cvoru kao srednjeg(prebacuje se i flag)
				q->keys[0].f = false;//ponistavanje pozicije sa koje je prebacen

				//pomjeranje pokazivaca za jedno mjesto udesno
				q->p[2] = q->p[1];
				q->p[1] = q->p[0];
				q->p[0] = nullptr;

				//upis kljuca na odgovarajuce mjesto,poredi se sa kljucem koji se propagira i upisuje u odgovarajuci cvor
				if (keyup > key) {//kljuc se upisuje u stari cvor
					if (key > q->keys[1].key) {//uspiuje se desno od srednjeg kljuca
						q->keys[2].key = key;
						q->keys[2].f = true;
						q->valid_keys++;

						q->p[3] = l;
						if (l) l->parent = q;
					}
					else {//upisuje se lijevo od  srednjeg kljuca
						q->keys[0].key = key;
						q->keys[0].f = true;
						q->valid_keys++;

						q->p[0] = q->p[1];
						q->p[1] = l;
						if (l) l->parent = q;
					}

				} else {//kljuc se upisuje u novi cvor
					if (key > newnode->keys[1].key) {//upisuje se desno od srednjeg kljuca u novom cvoru
						newnode->keys[2].key = key;
						newnode->keys[2].f = true;
						newnode->valid_keys++;

						newnode->p[3] = l;
						if (l) l->parent = newnode;
					}
					else {//uspisuje je lijevo od srednjeg kljuca u novom cvoru
						newnode->keys[0].key = key;
						newnode->keys[0].f = true;
						newnode->valid_keys++;

						newnode->p[0] = newnode->p[1];
						newnode->p[1] = l;
						if (l) l->parent = newnode;

					}


				}

				//izvrsena podjela cvorova i upis odgovarajucih kljuceva
				//prebacivanje pokazivaca q na njegovog oca(u njega se upisuje kljuc up)
				//l postaje newnode(cvor koji se prenosi kao desni)

				q = q->parent;
				l = newnode;
				key = keyup;

				if (!q) {//otac od q je nullptr,prepolovljen je korjen
					Node *newroot = new Node();
					newroot->keys[1].key = key;
					newroot->keys[1].f = true;
					newroot->valid_keys++;
					newroot->parent = nullptr;

					newroot->p[1] = root;//stari korjen postaje lijevi sin novog
					root->parent = newroot;

					newroot->p[2] = l;//cvor koji se prenosi je desni sin novog korjena
					l->parent = newroot;

					root = newroot;//korjen postaje novi korjen
					numberOfKeys++;
					return;


				}
				else if (q->valid_keys < 3) {//u cvoru grananja u koji se ubacuje kljuc koji se propagira ima dovoljno mjesta
					if (q->valid_keys == 1) {//u cvoru grananja u koji se ubacuje kljuc koji se propagira ima 1 mjesto
						if (key < q->keys[1].key) {//kljuc manji od srednjeg,ubacuje se lijevo
							q->keys[0].key = key;
							q->keys[0].f = true;
							q->valid_keys++;

							q->p[0] = q->p[1];
							q->p[1] = l;
							if (l) l->parent = q;
							numberOfKeys++;
							return;
						}
						else {//kljuc je veci od srednjeg,ubacuje se desno
							q->keys[2].key = key;
							q->keys[2].f = true;
							q->valid_keys++;

							q->p[3] = l;
							if (l) l->parent = q;
							numberOfKeys++;
							return;
						}
					}
					else {//u cvoru grananja u koji se ubacuje kljuc koji se propagira imaju 2 mjesta,treba sortirati kljuceve i pokazivace

						if (q->keys[1].key<key && !q->keys[2].f) {//kljuc veci od srednjeg i pozicija desno od srednjeg slobodna
							q->keys[2].key = key;
							q->keys[2].f = true;
							q->valid_keys++;

							q->p[3] = l;
							if (l) l->parent = q;
							numberOfKeys++;
							return;
						}
						else if (q->keys[1].key>key && !q->keys[0].f) {//kljuc manji od srednjeg i pozicija lijevo od srednjeg slobodna
							q->keys[0].key = key;
							q->keys[0].f = true;
							q->valid_keys++;

							q->p[0] = q->p[1];
							q->p[1] = l;
							if (l) l->parent = q;
							numberOfKeys++;
							return;
						}
						else if (q->keys[1].key > key) {	//umece se lijevo od srednjeg kljuca
							if (q->keys[0].key < key) {//umece se izmedju prvog i srednjeg
								q->keys[2].key = q->keys[1].key;
								q->keys[2].f = true;
								q->keys[1].key = key;
								q->valid_keys++;

								q->p[3] = q->p[2];
								q->p[2] = l;
								if (l) l->parent = q;
								numberOfKeys++;
								return;
							}
							else {//umece se na mjesto prvog kljuca koje je zauzeto,pomjeranje prvog i srednjeg kljuca udesno
								q->keys[2].key = q->keys[1].key;
								q->keys[2].f = true;
								q->keys[1].key = q->keys[0].key;
								q->keys[0].key = key;
								q->valid_keys++;

								q->p[3] = q->p[2];
								q->p[2] = q->p[1];
								q->p[1] = l;
								if (l) l->parent = q;
								numberOfKeys++;
								return;

							}
						}
						else {//umece se desno od srednjeg kljuca
							if (q->keys[2].key > key) {//umece se izmedju srednjeg i treceg kljuca
								q->keys[0].key = q->keys[1].key;
								q->keys[0].f = true;
								q->keys[1].key = key;
								q->valid_keys++;

								q->p[0] = q->p[1];
								q->p[1] = q->p[2];
								q->p[2] = l;
								if (l) l->parent = q;
								numberOfKeys++;
								return;

							}
							else {//umece se na mjesto treceg kljuca,srednji i treci se pomjeraju ulijevo
								q->keys[0].key = q->keys[1].key;
								q->keys[0].f = true;
								q->keys[1].key = q->keys[2].key;
								q->keys[2].key = key;
								q->valid_keys++;

								q->p[0] = q->p[1];
								q->p[1] = q->p[2];
								q->p[2] = q->p[3];
								q->p[3] = l;
								if (l) l->parent = q;
								numberOfKeys++;
								return;

							}
						}
					}
				}
			}
		}
	}
}


//struktura koja se stavlja u red prilikom ispisa stabla
typedef struct Elem {
	Node* node;
	int height;
	Elem(Node *pok = nullptr, int h = -1) :node(pok) { height = h; }
};



//friend fja za ispis stabla
ostream& operator<<(ostream& it, const BTree& b) {
	queue<Elem> Queue;
	Elem pom = Elem(b.root, 1);

	Queue.push(pom);
	while (!Queue.empty()) {
		pom = Queue.front();
		Queue.pop();
		for (int i = 0; i <= 3; i++) if (pom.node->p[i]) Queue.push(Elem(pom.node->p[i], pom.height + 1));

		int currentheight = pom.height;
		while (true) {
			int i;
			cout << *pom.node;
			
			if (Queue.empty()) break;
			else pom = Queue.front();

			if (pom.height != currentheight) break;
			else {
				Queue.pop();
				for (int i = 0; i <= 3; i++)
					if (pom.node->p[i])
						Queue.push(Elem(pom.node->p[i], pom.height + 1));
			}
			cout << "      ";

		}
		cout << endl;
	}


	return it;
}

enum COLOR { RED = 0, BLACK = 1 };

//klasa cvora u RB stablu
class RBNode {
public:
	RBNode(int key_=-1,COLOR color_=BLACK) {
		color = color_;
		key = key_;
		left = right = nullptr;
	}
	friend ostream& operator<<(ostream& it, const RBNode& node) { //preklopljen operator za ispis cvora
		if (node.color == BLACK) {
			cout << "[" << node.key << "]";
		}
		else {
			system("color 00");
		    cout <<"\033[97;101m" <<"("<<node.key <<")"<<"\033[0m" ;
			//cout << "(" << node.key << ")";
		}
		return it;
	}
	friend ostream& operator<<(ostream& it, const RedBlackTree& rb);
	friend class RedBlackTree;
private:
	int key;
	COLOR color;
	RBNode *left, *right;
};

class RedBlackTree {
public:
	RedBlackTree() { root = nullptr; }
	~RedBlackTree();

	RBNode* searchForInsert(int key)const;//trazi i prolanazi cvor koji ce cvor sa kljucem key primiti kao sina
	RBNode* transformBTreeNode(const Node* pok)const;//razbija cvor B-stabla i vraca pokazivac na korjen novoformiranog malog stabla
	void transformBTree(const BTree&);//transforimise B-stablo u RB-stablo
	friend ostream& operator<<(ostream& it, const RedBlackTree& rb);//op. <<preklopljen za ispis stabla


private:
	RBNode *root;
};

//destruktor za RB-stablo,brise cvorove level-orderom
RedBlackTree::~RedBlackTree() {
	if(root){//ako nije stvoreno stablo
	queue<RBNode*>Queue;
	RBNode* p = root;
	Queue.push(p);
	while (!Queue.empty()) {
		p = Queue.front();
		Queue.pop();
		if (p->left) Queue.push(p->left);
		if (p->right) Queue.push(p->right);
		delete p;
	}
	}
}

//pronalazi cvor koji ce cvor sa kljucem key primiti kao sina
RBNode* RedBlackTree::searchForInsert(int key)const {
	RBNode* q = nullptr, *p = root;
	while (p) {
		q = p;
		if (p->key == key) return nullptr;
		else if (p->key > key) p = p->left;
		else p = p->right;
	}
	return q;
}

//razbija cvor B-stabla i vraca pokazivac na korjen novoformiranog malog stabla
RBNode* RedBlackTree::transformBTreeNode(const Node* pok)const {
	RBNode *root=new RBNode(pok->keys[1].key,BLACK);//stvara malo podstablo
	if (pok->keys[0].f) root->left = new RBNode(pok->keys[0].key,RED);//stvara lijevog sina ako ga ima
	if (pok->keys[2].f) root->right = new RBNode(pok->keys[2].key, RED);//stvara desnog sina ako ga ima
	return root;
}

//transformise zadato B-stablo u RB-stablo
void RedBlackTree::transformBTree(const BTree& b) {
	Node* p = b.root;
	queue<Node*> Queue;
	Queue.push(p);

	while (!Queue.empty()) {
		p = Queue.front();
		Queue.pop();
		for (int i = 0; i <= 3; i++) if (p->p[i]) Queue.push(p->p[i]);//stavljanje u red djece tekuceg cvora B-stabla

		RBNode *rbp = transformBTreeNode(p);//razbijanje trenutnog cvora B-stabla
		if (p == b.root) {//ako je razbijen korjen B-stabla
			root = rbp;
		}
		else {//nije razbijen korjen  B-stabla
			RBNode *parent = searchForInsert(rbp->key);
			if (!parent) return;//ako je cvor kojim slucajem vec ubacen u RB-stablo
			if (parent->key > rbp->key) parent->left=rbp;//preveze se na odgovarajuceg sina oca
			else parent->right=rbp;
		}

	}
}



//preklopljen operator << za ispis RB-stabla
ostream& operator<<(ostream& it, const RedBlackTree& rb) {
	RBNode *p = rb.root;
	int level = 0;

	//strktura koja ce ici na stack
	typedef struct Elem {
		RBNode *node;
		int level;
		Elem(RBNode* node_ = nullptr, int level_ = -1) :node(node_) {
			level = level_;
		}
	};

	RBNode* node;
	stack<Elem> Stack;
	Elem pom(rb.root, 1);
	Stack.push(pom);

	while (!Stack.empty()) {
		pom = Stack.top();
		Stack.pop();
		level = pom.level;
		node = pom.node;
		while (node) {
			for (int i = 1; i <= level; i++) cout << "----";
			cout << (*node) << endl;
			if (node->right) Stack.push(Elem(node->right,level+1));
			node = node->left;
			level++;
		}
		
	}
	return it;
}


int main() {
	BTree b;
	RedBlackTree rb;
	int izbor;
	bool kraj = false;
	bool stv = false;


	while (!kraj) {
		cout << "Unesite izbor" << endl;
		cout << "1.Unos kljuceva u stablo" << endl;
		cout << "2.Ispis broja kljuceva u stablu" << endl;
		cout << "3.Ispis B-stabla"<< endl;
		cout << "4.Transformacija B-stabla u RB-stablo i njegov ispis" << endl;
		cout << "5.Pretraga kljuca u B-stablu" << endl;
		cout << "6.Kraj" << endl;
		cin >> izbor;
		switch (izbor) {
		case 1:if (!stv) {
			cout << "Unesite kljuceve(-1 za kraj):" << endl; int key;
			while (true) {
				cin >> key;
				if (key == -1) break;
				b.insert(key);
			}
			stv = true;
		}
			   else { cout << "Kljucevi su prethodno unijeti!"<<endl; }
			   break;

		case 2:if (stv)
		{
			cout << "Broj kljuceva u stablu:" << b.getNumKeys() << endl;
		}
			   else cout << "Stablo nije stvoreno!"<<endl; break;
		case 3: if (stv)  cout << b; else cout << "Stablo nije stvoreno!"<<endl; break;
		case 4:if (stv) {
			rb.transformBTree(b);
			cout << rb;
		}
			   else cout << "B stablo nije stvoreno!"<<endl; break;
		case 5:if (stv) { cout << "Unesite kljuc" << endl; int key; cin >> key; if (b.search(key)) cout << "Kljuc je nadjen!" << endl; else cout << "Kljuc nije nadjen!" << endl; }
			   else cout << "Stablo nije stvoreno!"<<endl;
		break;
		case 6: kraj = true; break;
		}
	}
	
	return 0;
}