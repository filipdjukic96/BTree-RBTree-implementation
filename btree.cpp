#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <queue>

using namespace std;


//klasa nizstringa koja predstavlja niz stringova
class NizString {
public:
	NizString() {
		s = nullptr;
		duz = nullptr;
		kap = pop = 0;
	}

	void CreateNizString();
	int InsertNizString(const string&);
	void DeleteNizString(const string&);
	~NizString() { brisi(); };
	friend ostream& operator<<(ostream& it, const NizString& ns);
	friend class BTree;
private:
	string *s;
	int *duz;
	int kap, pop;
	void brisi() {
		delete[] s;
		delete[] duz;
		s = nullptr;
		duz = nullptr;
	}
};

//stvaranje niza stringova
void NizString::CreateNizString() {
	cout << "Kako zelite unositi stringove:" << endl << "a.Putem datoteke" << endl << "b.Putem standardnom ulaza" << endl;
	char c;
	cin >> c;
	string rijec;
	ifstream file;
	string filename;

	if (s != nullptr) brisi();
	s = new string[5000];
	duz = new int[5000];
	kap = 5000;

	switch (c) {
	case 'a':


		cout << "Unesite ime fajla iz kog zelite da citate:";
		cin >> filename;
		file.open(filename);
		if (!file.is_open()) exit(1);
		else {

			while (!file.eof()) {
				file >> rijec;
				InsertNizString(rijec);
			}
		}

		file.close(); break;


	case'b':
		cout << "Unesite stringove:" << endl;
		while (true) {
			cin >> rijec;
			cin.get();
			if (rijec == "kraj") break;
			InsertNizString(rijec);
		}


		break;
	default:cout << "Niste izabrali dobru opciju!!" << endl; break;


	}


}

//ubacivanje u niz stringova,vraca poziciju umetnutog stringa
int NizString::InsertNizString(const string& rijec) {
	//pronalazi poziciju u nizu gdje treba umetnuti zadati string rijec
	int i;
	int position;//pozicija umetnutog stringa
	//ako je niz pun
	if (kap == pop) {
		string *novi_s = new string[5000 + kap];
		int *novi_duz = new int[5000 + kap];

		i = 0;
		while (duz[i] < rijec.length() && i<pop) {
			novi_s[i] = s[i];
			novi_duz[i] = duz[i];
			i++;
		}

		while (duz[i] == rijec.length() && i < pop && s[i] < rijec) {
			novi_s[i] = s[i];
			novi_duz[i] = duz[i];
			i++;
		}

		novi_s[i] = rijec;
		novi_duz[i] = rijec.length();
		position = i;

		while (i < pop) {
			novi_s[i + 1] = s[i];
			novi_duz[i + 1] = duz[i];
			i++;
		}

		brisi();

		s = novi_s;
		duz = novi_duz;
		pop++;
		kap += 5000;
	}
	else { //ako niz nije pun
		int i = pop;
		while (i > 0 && (duz[i - 1] > rijec.length() || (duz[i - 1] == rijec.length() && s[i - 1] > rijec))) {
			s[i] = s[i - 1];
			duz[i] = duz[i - 1];
			i--;
		}

		s[i] = rijec;
		duz[i] = rijec.length();
		position = i;
		pop++;
	}
	return position;
}


//brisanje stringa iz niza stringova
void NizString::DeleteNizString(const string& rijec) {

	if (pop == 0) return;
	int i = 0;
	while (i < pop && s[i] != rijec) i++;

	if (i < pop) {
		while (i < pop - 1) {
			s[i] = s[i + 1];
			duz[i] = duz[i + 1];
			i++;
		}
		pop--;
	}

	if (pop < kap / 2) {
		string *novi_s = new string[pop];
		int *novi_duz = new int[pop];
		i = 0;
		while (i < pop) {
			novi_s[i] = s[i];
			novi_duz[i] = duz[i];
			i++;
		}

		kap /= 2;
	}
}


//ispisivanje niza stringova
ostream& operator<<(ostream& it, const NizString& ns) {
	int i = 0;
	while (i < ns.pop) {
		it << ns.s[i] << endl;
		i++;
	}
	return it;
}


//struktura kljuca binarnog stabla
typedef struct Key {
	int key;
	int word;
	Key() {//stvara 'prazan' kljuc(duzina 0 i word je -1)
		key = 0;
		word = -1;
	};

};

//klasa cvor B-stabla
class Node {
public:
	Node(int m, Node* parent_);
	~Node() { brisi(); } //destruktor za cvor stabla,poziva privatnu metodu brisi()
	int binarySearch(int key) const;//binarna pretraga kljuca u cvoru,vraca indeks kljuca koji je isti trazenom ili gdje zavrsi binarna pretraga
	friend void inorder(Node *root_);//fja inorder koja obilazi stablo po inorderu,mora biti prijateljska za klasu Node
	int getNumKeys()const { return num_keys; }
	friend ostream& operator<<(ostream& it, const Node& node);//ispis cvora 

	friend class BTree;
private:
	Key *keys;
	Node** p;
	Node*parent;
	int num_keys;
	void brisi() {//privatna metoda koja se poziva u destruktoru,unistava cvor
		delete[]keys;
		delete[] p;
	}

};

//konstruktor za objekat klase Node
Node::Node(int m, Node* parent_ = nullptr) :parent(parent_) {
	keys = new Key[m - 1];
	p = new Node*[m];
	for (int i = 0; i < m; i++) p[i] = nullptr;
	num_keys = 0;
}

//binarna pretraga u cvoru
int Node::binarySearch(int key)const {
	int low = 0, high = num_keys - 1;
	int mid;
	while (low <= high) {
		mid = (low + high) / 2;
		if (keys[mid].key == key) return mid;
		else if (keys[mid].key>key) high = mid - 1;
		else low = mid + 1;
	}
	if (keys[mid].key < key && mid != num_keys - 1) return mid + 1;
	else return mid;
}

//preklopljena operatorska fja za ispis cvora
ostream& operator<<(ostream& it, const Node& node) {


	for (int i = 0; i < node.num_keys; i++)
	{
		it << node.keys[i].key;
		if (i == node.num_keys - 1) it << ""; else it << "|";
	}
	return it;
}

//klasa B-stabla
class BTree {
public:
	BTree() :root(nullptr) { //konstruktor,stvara stablo zadatog reda stvaranjem korjena(root) i inicijalizacijom njegove djece na nullptr
		m = 5;
		numberOfKeys = 0;
	}
	~BTree();//destruktor za brisanje objekta stabla
	void createTree(const NizString& ns);//kreira stablo na osnovu strukture podataka NizString
	void setM(int m_) { if (m_ < 3) m_ = 3; m = m_; }
	bool searchKey(int key)const;//pretraga kljuca u stablu,vraca true ako se nadje,false ako se ne nadje
	Node* searchForInsert(int key) const;//vraca pokazivac na list gdje se umece cvor,koristi se u insertKey metodi
	Node* searchForDelete(int key, int& position)const;//vraca pokazivac na cvor odakle se brise kljuc i preko ref. njegovu poziciju
	bool insertKey(int key, int word);//umece kljuc i pokazivac na 'njegovu' rijec
	bool deleteKey(int key);//brise kljuc iz stabla

	int successor(const Node* q, int key) const;//trazi sljedbenika u stablu,tj vraca poziciju na kojoj se on nalazi u strukturi
	int searchString(const string& s, const NizString& ns,bool flag)const;//pronalazi string u nizu stringova
	void updateTree(int key,int value);//azurira stablo-dodaje
	void insertString(const string& s, NizString& ns);//umece string u niz stringova i azurira stablo
	void deleteString(const string&s, NizString& ns);//brise string iz niza stringova i azurira stablo
	void printLength(int length, const NizString& ns)const;//ispisuje sve stringove zadate duzine

	friend void inorder(Node *root_);//rekurzivni inorder-sluzi za provjeru!
	Node* getRoot()const {//metoda za vracanje korjena stabla,koristi se prilikom rekurzivnog inordera
		return root;
	}
	void numKeys()const { cout << "Broj kljuceva u stablu:" << numberOfKeys << endl; }
	int height()const; //metoda za odredjivanje visine stabla,spusta se po lijevim pokazivacima
	friend ostream& operator<<(ostream& it, const  BTree& b);//preklopljen operator << za ispis stabla

	friend class NizString;
private:
	Node* root;
	int m, numberOfKeys;
	void pisi(ostream& it)const;
};

//metoda za kreiranje stabla na osnovu strukture podataka
void BTree::createTree(const NizString& ns) {
	int i = 0;
	int current_length = 0;//pocetna duzina(stavlja se 0 kako bi se upisala i prva rijec)

	while (i < ns.pop) {
		if (ns.duz[i] != current_length) {
			current_length = ns.duz[i];
			insertKey(ns.duz[i], i);
			i++;
		}
		else i++;
	}
}

//destruktor za stablo
BTree::~BTree() {

	if (root) {//ako root nije nullptr,u suprotnom smo brisanjima sveli stablo na 0 kljuceva pa nema potrebe za destruktorom
		queue<Node*> Queue;//koriscenje template reda
		Queue.push(root);//umetanje korjena u red
		while (!Queue.empty()) {//
			Node* p = Queue.front();
			Queue.pop(); //umetanje svih sinova trenutnog cvora
			for (int i = 0; i <= p->num_keys; i++)if (p->p[i]) Queue.push(p->p[i]);
			delete p;//brisanje cvora
		}
		root = nullptr;
	}

}

//pretraga kljuca u cvoru
bool BTree::searchKey(int key)const {
	Node *pom = root;
	while (pom) {
		int key_ind = pom->binarySearch(key);
		if (pom->keys[key_ind].key == key) return true;
		else if (pom->keys[key_ind].key > key) pom = pom->p[key_ind];
		else pom = pom->p[key_ind + 1];
	}
	return false;
}

//vraca pokazivac na list gdje se umece kljuc
Node* BTree::searchForInsert(int key)const {
	Node*pom = root;
	Node *q = nullptr;

	while (pom) {
		q = pom;
		int key_ind = pom->binarySearch(key);
		if (pom->keys[key_ind].key == key) return nullptr;
		else if (pom->keys[key_ind].key > key) pom = pom->p[key_ind];
		else pom = pom->p[key_ind + 1];
	}
	return q;
}

//metoda koja umece kljuc u stablo
bool BTree::insertKey(int key, int word) {
	if (!root) { //ako je stablo prazno stvaramo korjen
		root = new Node(m);
		root->keys[0].key = key;
		root->keys[0].word = word;
		root->num_keys++;
		numberOfKeys++;//ubacen kljuc,povecava se brojac
		return true;
	}
	else { //stablo nije prazno
		Node *q = searchForInsert(key); //vraca pokazivac na list gdje se umece kljuc
		if (!q) return false;//ako kljuc postoji u stablu vracamo false,u suprotnom ubacujemo ga

		if (q->num_keys < m - 1) {//slucaj kada u listu ima mjesta(nema potrebe premjestati pokazivace)
			int i = q->num_keys;
			while (i>0 && q->keys[i - 1].key>key) {//premjestamo sve kljuceve vece od zadatog i trazimo mu poziciju
				q->keys[i] = q->keys[i - 1]; i--;//ne premjestamo pokazivace jer se radi o listu
			}
			q->keys[i].key = key; //upis novog kljuca(duzine rijeci) na pronadjenu poziciju
			q->keys[i].word = word;//pokazivac na rijec se
			q->num_keys++;
			numberOfKeys++;//ubacen kljuc,povecava se brojac
			return true;
		}
		else {
			Node* l = nullptr;//pokazivac na cvor koji se prenosi
			Key up;
			up.key = key;
			up.word = word;


			while (true) {



				Node* help = new Node(m + 1, nullptr);//pomocni niz koji ce da se 'prelomi' oko srednjeg clana
				int i = 0;

				//formiranje pomocnog niza
				help->p[0] = q->p[0];
				help->num_keys = m;
				//upisivanje svih kljuceva koji su manji od ovog sto se umece
				while (q->keys[i].key < key && i < q->num_keys) {
					help->keys[i] = q->keys[i];
					help->p[i + 1] = q->p[i + 1];
					i++;
				}

				//umetanje novog kljuca i njegovog pokazivaca q u pomocni niz 
				help->keys[i] = up;
				help->p[i + 1] = l;//l je pok na cvor koji se prenosi

								   //umetanje ostatka kljuceva iz lista(ako postoje)
				while (i < q->num_keys) {
					help->keys[i + 1] = q->keys[i];
					help->p[i + 2] = q->p[i + 1];
					i++;
				}


				int position = (int)ceil(double(m - 1) / 2);//pozicija kljuca koji se propagira

				up = help->keys[position];//promjena cvora koji se umece u cvor na nivou iznad

				Node *novi = new Node(m, q->parent);//novi cvor koji nastaje cijepanjem starog(parent mu se postavlja na isti kao kod q)
													//->num_keys = m - 1 - position - 1;//broj kljuceva u novom cvoru

				q->num_keys = position;//broj kljuceva u starom cvoru


									   //upis kljuceva i pokazivaca u stari cvor;
				int j = 0;
				q->p[0] = help->p[0];//najljevlji pokazivac
				while (j < position) {
					q->keys[j] = help->keys[j];
					q->p[j + 1] = help->p[j + 1];
					j++;
				}


				//prebacivanje da pok na oca(parent) od 'novih' sinova starog cvora pokazuje na stari cvora
				int g = 0;
				while (g < q->num_keys) {
					if (q->p[g]) q->p[g]->parent = q;
					g++;
				}
				if (q->p[g]) q->p[g]->parent = q;


				j++;//preskakanje kljuca koji se propagira
				l = help->p[j];//l postaje desni pokazivac od kljuca koji se propagira-kasnije ce postati najljevlji u novom cvoru!


							   //upis kljuceva i pokazivaca u novi cvor
				novi->p[0] = l;//l postaje najljevlji pokazivac u novom cvoru
				while (j < help->num_keys) {
					novi->keys[j - position - 1] = help->keys[j];
					novi->p[j - position] = help->p[j + 1];
					j++;
					novi->num_keys++;//broj kljuceva u novom cvoru inkrementiramo za 1
				}

				//prebacivanje da pok na oca(parent) od sinova novog cvora pokazuje na novi cvor
				g = 0;
				while (g < novi->num_keys) {
					if (novi->p[g]) novi->p[g]->parent = novi;
					g++;
				}
				if (novi->p[g]) novi->p[g]->parent = novi;

				l = novi;

				q = q->parent;//q prelazi na oca,a otac od novog cvora postaje otac od q

				if (!q) {//otac od q je nullptr-prepolovili smo korjen,treba nam novi korjen!
					Node* newroot = new Node(m);//novi korjen

					newroot->keys[0] = up;//up se upisuje u novi korjen
					newroot->p[0] = root;//novi korjen uzima kao lijevog sina stari korjen
					newroot->p[1] = l;//kao desnog sina uzima cvor pokazan sa l
					newroot->num_keys++;//br kljuceva u novom korjenu postan 1
					root->parent = l->parent = newroot;
					root = newroot;//stari korjen postaje novi
					numberOfKeys++;//br kljuceva se povecava za jedan(tek kada se zavrsi propagacija)
					return true;
				}
				else if (q->num_keys < m - 1) {//ako je u sljedecem cvoru(u koji se propagira up) ima mjesta
					int k = q->num_keys;
					while (q->keys[k - 1].key>up.key) {//pomjeranje svih kljuceva koji su veci od njega udesno
						q->keys[k] = q->keys[k - 1];
						q->p[k + 1] = q->p[k];
						k--;
						if (k == 0) break;
					}
					q->keys[k] = up;			//upis kljuca 'up' na njegovo mjesto i njegovog desnog pokazivaca 'l'
					q->p[k + 1] = l;
					l->parent = q;//otac cvora l postaje cvor koji njega preuzima kao desnog
					q->num_keys++;
					numberOfKeys++;//br kljuceva se povecava za jedan(tek kada se zavrsi propagacija)
					return true;
				}
			}
		}
	}
}


//metoda koja vraca pok na cvor gdje se nalazi kljuc i preko reference njegovu poziciju
Node* BTree::searchForDelete(int key, int& position)const {
	Node*pom = root;
	Node *q = nullptr;

	while (pom) {
		q = pom;
		int key_ind = pom->binarySearch(key);
		if (pom->keys[key_ind].key == key) {
			position = key_ind;
			return pom;
		}
		else if (pom->keys[key_ind].key > key) pom = pom->p[key_ind];
		else pom = pom->p[key_ind + 1];
	}
	return nullptr;
}

//metoda koja brise kljuc iz stabla
bool BTree::deleteKey(int key) {
	int position;
	int min = ceil(double(m) / 2) - 1;
	int check = 0;

	Node *parent = nullptr;
	int position_separator;

	Node* q = searchForDelete(key, position);//vraca pok na cvor gdje je kljuc,i preko ref. njegovu poziciju
	if (!q) return false;//cvor ne postoji u stablu,vraca se false

	if (q->p[position + 1]) {//kljuc  je u cvoru grananja,trazimo mu sljedbenika
		Node* r = q->p[position + 1];

		while (r->p[0]) { //trazimo pokazivac na cvor gdje je sljedbenik,sljedbenik je sigurno na poziciji 0 u tom cvoru!
			r = r->p[0];
		}

		check = 1;
		if (r->parent == q)//ako je sljedbenik u cvoru sinu(na nivou ispod)
			position_separator = q->binarySearch(r->keys[0].key);//trazenje razdvojnog kljuca(trazenje sljedbenika u cvoru iz kog se brise)
		else position_separator = r->parent->binarySearch(r->keys[0].key);//u suprotnom,razdvojni kljuc trazimo u ocu sljedbenika(trazimo u njemu sljedbenika,tj. kljuc koji ce zamijeniti onaj koji se brise)

		q->keys[position] = r->keys[0];//saljemo sljedbenika na njegovo mjesto
		q = r;//cvor iz kog se brise postaje list
		position = 0;//pozicija cvora koji se brise je 0


	}

	while (true) {

		if (numberOfKeys == 1) {
			delete root;
			root = nullptr;
			numberOfKeys = 0;
			return true;
		}

		//brisemo kljuc iz cvora,OVO SE IZVRSAVA U SVIM SLUCAJEVIMA
		for (int j = position; j < q->num_keys - 1; j++) {//pomjeramo sve kljuceve za jedno mjesto unazad pocev od onog koji se brise
			q->keys[j] = q->keys[j + 1];
			q->p[j + 1] = q->p[j + 2];
		}
		//q->p[q->num_keys - 1] = q->p[q->num_keys];//pomjerimo posljednji pokazivac
		q->num_keys--;

		if (q->num_keys >= min || (q == root && q->num_keys >= 1)) {  //u listu iz kog je izbrisano ostalo dovoljno kljuceva ili je korjen a ostao 1 kljuc ili vise
			numberOfKeys--;
			return true;
		}
		else {//u listu iz kog je izbrisano nije ostalo dovoljno kljuceva

			parent = q->parent;//pok na cvor oca


			if (!check) {
				position_separator = parent->binarySearch(key);//trazenje razdvojnog kljuca cvora iz kog se brise kljuc i njegovog brata
				check = 0;
			}

			if (!(position_separator == parent->num_keys - 1 && key>parent->keys[parent->num_keys - 1].key)) {//ako cvor iz kog se brise ima desnog brata(nije najdesniji)
				Node *right_brother = parent->p[position_separator + 1];//pok na desnog brata
				if (right_brother->num_keys > min) {//desni brat ima dovoljno kljuceva-PRELIVANJE IZ DESNOG BRATA!!!!!!!
					q->keys[q->num_keys] = parent->keys[position_separator];//razdvojni kljuc ide u cvor iz kog je izbrisano
					q->num_keys++;//u cvor je dodat kljuc,povecava se njihov broj
					parent->keys[position_separator] = right_brother->keys[0];//najljevlji kljuc desnog brata ide kao razdvojni
					q->p[q->num_keys] = right_brother->p[0];//cvor iz kog se brise preuzima najljevlji pokazivac iz desnog brata ZBOG PRELIVANJA
					if (q->p[q->num_keys]) q->p[q->num_keys]->parent = q;//azurira se otac prebacenog cvora

																		 //azuriranje kljuceva i pokazivaca u desnom bratu iz kog je izbacen kljuc u oca
					for (int j = 0; j < right_brother->num_keys - 1; j++) {
						right_brother->keys[j] = right_brother->keys[j + 1];
						right_brother->p[j] = right_brother->p[j + 1];

					}//na kraju azuriranje posljednjeg pokazivaca u desnom bratu(nije obuhvacen petljom)
					right_brother->num_keys--;
					right_brother->p[right_brother->num_keys] = right_brother->p[right_brother->num_keys + 1];
					numberOfKeys--;
					return true;
				}
			}

			if (q != parent->p[0]) {//cvor ima lijevog brata!(nije najljevlji u ocu)
				int decrease = 0;//promjenljiva za provjeru da li smo smanjili position_separator a nismo prelivali iz lijevog brata
				if (q != parent->p[parent->num_keys]) {//ako cvor iz kog se brise nije najdesniji sin dekrementira se pozicija separatora,u suprotnom ostaje ista!!
					position_separator--;
					decrease = 1;//oznaka da smo poziciju separatora smanjili
				}
				Node *left_brother = parent->p[position_separator];//lijevi brat!

				if (left_brother->num_keys > min) {//lijevi brat ima dovoljno kljuceva-PRELIVANJE IZ LIJEVOG BRATA!!!!

												   //ispomjeraju se kljucevi i pokazivaci iz cvora koji se brise(da se oslobodi pozicija 0)
					q->p[q->num_keys + 1] = q->p[q->num_keys];//prevezivanje posljednjeg pokazivaca

															  //prevezivanje ostalih pokazivaca
					for (int j = q->num_keys; j > 0; j--) {
						q->keys[j] = q->keys[j - 1];
						q->p[j] = q->p[j - 1];
					}

					q->keys[0] = parent->keys[position_separator];//spustanje razdvojnog kljuca

					q->p[0] = left_brother->p[left_brother->num_keys];//prelazak najdesnijeg pokazivaca iz lijevog brata kao najljevljeg u cvoru iz kog se obrisao kljuc
					if (q->p[0]) q->p[0]->parent = q;//azuriranje oca prebacenog cvora ako on postoji

													 //dizanje najveceg kljuca u lijevom bratu kao razdvojnog i azuriranje broja kljuceva
					parent->keys[position_separator] = left_brother->keys[left_brother->num_keys - 1];
					left_brother->num_keys--;//iz brata posudjen jedan kljuc koji ide kao razdvojni
					q->num_keys++;//u cvor iz kog se obrisalo dodao se razdvojni kljuc
					numberOfKeys--;
					return true;

				}
				else if (decrease) position_separator++;//ako smo dekrementirali poziciju separatora(jer cvor iz kog se brise 
														//nije najdesniji sin oca,a pritom nismo prelili iz lijevog brata
														//potrebno je da vratimo poziciju separatora na originalnu vrijednost

			}

			Node* right;//cvor iz kog prelaze svi u lijevi i koji se dealocira
			if (parent->p[parent->num_keys] == q) {//q je najdesniji sin,iz njega se brise ali se cvorovi prelivaju u njegovog lijevog brata
				right = q;//cvor iz kog se brise postaje onaj koji ce se dealocirati
				q = parent->p[position_separator];//u lijevog brata se unacuju ostali 
			}
			else right = parent->p[position_separator + 1];//cvor iz kog se dealocira je desni brat

			q->keys[q->num_keys] = parent->keys[position_separator];//spustanje razdvojnog kljuca u odredisni
			q->p[q->num_keys + 1] = right->p[0];//prebacivanje prvog pokazivaca iz desnog cvora kao desnog pokazivaca novoumetnutog kljuca u q
			if (q->p[q->num_keys + 1]) q->p[q->num_keys + 1]->parent = q;//azuriranje oca prebacenog pokazivaca
			q->num_keys++;//povecavnje broja kljuceva

						  //prebacivanje ostalih kljuceva i pokazivaca iz desnog cvora
			for (int j = 0; j < right->num_keys; j++) {
				q->keys[q->num_keys] = right->keys[j];//prebacivanje kljuca
				q->p[q->num_keys + 1] = right->p[j + 1];//prebacivanje pokazivaca
				if (q->p[q->num_keys + 1]) q->p[q->num_keys + 1]->parent = q;//azuriranje oca prebacenog cvora ako on postoji
				q->num_keys++;

			}


			delete right;//dealociranje cvora


			q = parent;//cvor iz kog se brise propagira se

			if (q == root && root->num_keys == 1) {//dosli smo do korjena,ostao u njemu jedan cvor,visina se smanjuje za 1
				root = root->p[0];
				root->parent = nullptr;
				delete q;
				numberOfKeys--;
				return true;
			}
			position = position_separator;//pozicija sa koje se brise je pozicija razdvojnog kljuca koji je prebacen u lijevog sina
			check = 0;

		}


	}



}

//pomocna metoda za odredjivanje inordera,za provjeru je li dobro stablo
void inorder(Node* root_) {
	if (root_) {
		Node *p = root_;
		int i = 0;
		for (i = 0; i < p->num_keys; i++) {
			inorder(p->p[i]);
			cout << p->keys[i].key << "   ";
		}
		inorder(p->p[i]);
	}
}

//metoda za odredjivanje visine,ide po lijevim pokazivacima
int BTree::height() const {
	Node*p = root;
	int h = 0;
	while (p->p[0]) {
		p = p->p[0];
		h++;
	}
	return h;
}


//struktura koja se stavlja u red prilikom ispisa stabla
typedef struct QueueElem {
	Node* node;
	int height;
	QueueElem(Node *pok = nullptr, int h = -1) :node(pok) { height = h; }
};

//privatna metoda pisi() klase BTree koja ispisuje stablo
void BTree::pisi(ostream& it) const {

	queue <QueueElem> Queue;//red koji koristimo za level order obilazak


	QueueElem pom;
	Queue.push(QueueElem(root, 0));//guramo korjen i njegov nivo u red
	while (!Queue.empty()) {
		pom = Queue.front();//uzimanje elementa iz reda
		Queue.pop();//izbacivanje elementa iz reda

					//stavljanje svih postojecih sinova trenutnog cvora nu red
		for (int j = 0; j < pom.node->num_keys; j++)if (pom.node->p[j]) Queue.push(QueueElem(pom.node->p[j], pom.height + 1));
		if (pom.node->p[pom.node->num_keys]) Queue.push(QueueElem(pom.node->p[pom.node->num_keys], pom.height + 1));

		int currentheight = pom.height;//trenutna visina na kojoj se ispisuju cvorovi 
		while (true) {
			int i;
			cout << *pom.node;//ispis tekuceg cvora

							  //ispis nedostajucih kljuceva kao *
			i = pom.node->num_keys;
			while (i < m - 1) {
				it << "|*";
				i++;
			}

			//ako je ispisan posljednji cvor izlazimo iz while(i iz okruzujuceg)
			if (Queue.empty()) break;
			else pom = Queue.front();//u suprotnom 'pogledamo' sljedeci element

			if (pom.height != currentheight) break; //ako je taj element na nizem nivou prekidamo sa ispisom tekuceg nivoa
			else {
				Queue.pop();//u suprotnom uzimamo ga da ispisemo i njega,njegove validne sinove ubacimo u red
				for (int j = 0; j < pom.node->num_keys; j++)if (pom.node->p[j]) Queue.push(QueueElem(pom.node->p[j], pom.height + 1));
				if (pom.node->p[pom.node->num_keys]) Queue.push(QueueElem(pom.node->p[pom.node->num_keys], pom.height + 1));

			}
			cout << "     ";//kad zavrsi sa ispisom cvora odmaknemo za sljedeci cvor
		}
		cout << endl;//kad zavrsi ispis nivoa prelazi se na sljedeci,tj. u novu liniju

	}

}


//preklopljen operator<< za ispis stabla
ostream& operator<<(ostream& it, const BTree& b) {
	b.pisi(it);
	return it;
}

//nalazi sljedbenika zadatog kljuca u stablu
int BTree::successor(const Node* q, int key)const {//metoda koja trazi sljedbenika zadatog cvora,vraca njegovu poziciju u nizu stringova

	int position = q->binarySearch(key);//trazimo poziciju kljuca ciji se sljedbenik trazi u njegovom cvoru

	if (q->p[position + 1]) {//ako ima desni pokazivac,sljedbenik mu je najljevlji u desnom podstablu
		Node *r = q->p[position + 1];
		while (r->p[0]) r = r->p[0];
		return r->keys[0].word;//vracamo poziciju sljebenika u nizu stringova
	}
	else if (position < q->num_keys - 1) return q->keys[position + 1].word; //ako je u listu ali nije posljednji kljuc u cvoru
	else {//nema desno podstablo i posljednji je u listu
		Node *parent = q->parent;//pokazivac na oca,idemo po precima i trazimo prvi veci od zadatog
		while (parent) {
			int position_succ = parent->binarySearch(key);//trazenje u ocu ovog kljuca,ako nas uputi na neki veci vracamo njegovu poziciju
			if (parent->keys[position_succ].key>key) return parent->keys[position_succ].word;
			else parent = parent->parent;//ako nas ne uputi na veci onda idemo na sljedeceg pretka

		}

	}

	return -1;//kljuc nema sljedbenika sto znaci da je posljednji u stablu
}

//pronalazi string,odnosno vraca njegovu poziciju u nizu stringova
//flag=0 trazi se string,flag=1 trazi se pozicija gdje se umece
int BTree::searchString(const string& s, const NizString& ns,bool flag) const {//metoda za pronalazenje stringa,vraca njegovu poziciju u nizu stringova
	int position_string, position_succ, position;
	Node* q = searchForDelete(s.length(), position);//pronalazi pokazivac na cvor gdje se nalazi kljuc i POZICIJU KLJUCA u cvoru
	if (!q) return -1;//string ne postoji(tj. ne postoji njegova duzina u rijetkom indeksu-stablu!!!)
	position_string = q->keys[position].word;//pozicija stringa u nizu stringova
	position_succ = successor(q, s.length());//pronalazi poziciju sljedbenika(po duzini) u nizu strigova
	if (position_succ == -1) position_succ = ns.pop - 1;//ako nema sljedbenika(po duzini) onda je granica za pretragu posljednji el. u nizu stringova


	//binarna pretraga na dati string u nizu stringova
	int low = position_string, high = position_succ;
	int mid;
	while (low <= high) {
		 mid = (low + high) / 2;
		if (ns.s[mid] == s) return mid;
		else if (ns.s[mid] > s) high = mid - 1;
		else low = mid + 1;
	}
	if (!flag) return -1;//u pitanju je pretraga,kljuc nije nadjen,vraca se -1
	else if (ns.s[mid] > s) return mid;//flag=1,u pitanju je umetanje,vraca se mid+1 ili mid kao pozicija na koju treba umetnuti
	else return mid + 1;
}

//ispisuje sve stringove zadate duzine 
void BTree::printLength(int length, const NizString& ns) const {
	int position_start, position_end;//pozicije pocetka i kraja intervala stringova zadatke duzine
	int position;
	Node* q = searchForDelete(length, position);
	if (!q) {
		cout << "Ne postoje stringovi zadate duzine" << endl;
		return;
	}
	position_start = q->keys[position].word;//pozicija odakle pocinje ispis

	position_end = successor(q, length);//fja vraca poziciju prvog stringa sa vecom duzinom u nizu stringova
	if (position_end == -1) position_end = ns.pop;//ako je string sa najvecom duzinom

												  //ispis stringova zadatke duzine
	for (int i = position_start; i < position_end; i++)
		cout << ns.s[i] << endl;
}


//metoda koja azurira stablo,tj inkrementira ili dekrementira pozicije svih kljuceva vecih od zadatok u zavinosti od operacije
void BTree::updateTree(int key, int value) {
	queue<Node*> Queue;
	Node *pom = root;

	Queue.push(pom);
	while (!Queue.empty()) {
		pom = Queue.front();
		Queue.pop();
		int i = 0;
		while (i < pom->num_keys) {
			if (pom->p[i]) Queue.push(pom->p[i]);
			if (pom->keys[i].key>key) pom->keys[i].word = pom->keys[i].word + value;//ako je kljuc veci od zadatog,azurira se njegova pozicija
			i++;
		}
		if (pom->p[i]) Queue.push(pom->p[i]);
	}
}


//umece string u niz stringova i azurira stablo
void BTree::insertString(const string& s, NizString& ns) {
	if (searchKey(s.length())) {//postoji kljuc te duzine,moze binarno da se pretrazi pozicija za umetanje
		int position = searchString(s, ns, 1);//pretraga na poziciju gdje se umece
			
		//pomjeranje svih kljuceva od kraja do pozicije udesno
		int j = ns.pop;
		while (j > position) {
			ns.s[j] = ns.s[j - 1];
			ns.duz[j] = ns.duz[j - 1];
			j--;
		}
		//umetanje zadatog stringa
		ns.s[j] = s;
		ns.duz[j] = s.length();
		ns.pop++;
		updateTree(s.length(), 1);//azuriranje stabla
	}
	else {//ne postoji string te duzine,klasicno umetanje koje vraca poziciju umetnutog stringa
		int position = ns.InsertNizString(s);
		insertKey(s.length(), position);//duzina stringa(kljuc) i njegova pozicija se dodaju u stablo
		updateTree(s.length(), 1);//azuriranje stabla
	}
}

//metoda koja brise string i azurira stablo
void BTree::deleteString(const string& s, NizString& ns) {
	int position = searchString(s, ns, 0);//pretraga stringa u nizu stringova
	if (position == -1) return;//string nije nadjen,ne moze se obrisati

	//uklanjanje stringa iz niza stringova,tj pomjeranje svih od te pozicije ulijevo za jedno mjesto
	int i = position;
	//ako je izbrisani string jedini svoje duzine u nizu,u stablu se mora izbrisati kljuc sa njegovom duzinom
	//porede se duzine stringova pored onog koji se brise da bi se utvrdilo da li je on jedinstvene duzine
	if (ns.s[position + 1].length() != s.length() && ns.s[position - 1].length() != s.length()) deleteKey(s.length());
	while (i < ns.pop-1) {
		ns.s[i] = ns.s[i + 1];
		ns.duz[i] = ns.duz[i + 1];
		i++;
	}
	ns.pop--;//popunjenost se smanjuje
	updateTree(s.length(), -1);
}

int main() {
	int izbor;
	int m;
	bool kraj = false, stv = false;

	NizString ns;
	string rijec;
	BTree b;

	while (!kraj) {

		cout << "Izaberite opciju:" << endl;
		cout << "1.Stvaranje strukture podataka koja sadrzi stringove i rijedak indeks:" << endl;
		cout << "2.Umetanje stringa u strukturu podataka i azuriranje indeksa" << endl;
		cout << "3.Brisanje stringa iz strukture podataka i azuriranje indeksa" << endl;
		cout << "4.Ispisivanje sadrzaja strukture podataka" << endl;
		cout << "5.Ispis stabla:" << endl;
		cout << "6.Pronalazak stringa u strukturi podataka" << endl;
		cout << "7.Ispis stringova zadate duzine:" << endl;
		cout << "8.Kraj" << endl;
		cin >> izbor;
		switch (izbor) {
		case 1:if (!stv) {
			ns.CreateNizString(); stv = true;
			cout << "Unesite red stabla" << endl;
			int m;
			cin >> m;
			b.setM(m);
			b.createTree(ns);

		}else cout << "Struktura je prethodno stvorena!" << endl;break;
		case 2:if (stv) {
			string s;
			cout << "Unesite string koji zelite umetnuti:" << endl; 
			cin >> s;
			b.insertString(s,ns);
		}
			   else  cout << "Struktura nije stvorena!" << endl;break;

		case 3:if (stv) { string s;
			cout << "Unesite string koji zelite da izbrisete:" << endl; 
			cin >> s; 
			b.deleteString(s, ns);
			 }
	   else  cout << "Struktura nije stvorena!" << endl;break;

		case 4:if (stv) cout << ns; else cout << "Struktura nije stvorena!" << endl; break;
		case 5:if (stv) cout << b; else cout << "Struktura i stablo nisu stvoreni!" << endl; break;
		case 6:if (stv) {
			cout << "Unesite string koji zelite da pronadjete" << endl;
			string s;
			cin >> s;
			int position = b.searchString(s, ns,0);
			if (position == -1) cout << "String ne postoji!" << endl;
			else cout << "String je na poziciji:ns[" << position << "]" << endl;
		} else { 
			cout << "Struktura i stablo nisu stvoreni!" << endl; }break;
		case 7:if (stv) {
			int length;
			cout << "Unesite duzinu stringa:";
			cin >> length;
			b.printLength(length, ns);
		} else cout << "Struktura i stablo nisu stvoreni" << endl; break;
		case 8:kraj = true; break;
		default:kraj = true; break;
		}

	}
	return 0;
}


