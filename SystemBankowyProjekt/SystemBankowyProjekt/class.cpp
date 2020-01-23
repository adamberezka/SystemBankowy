#include "class.h"

using namespace std;

#if defined _WIN32
void clear() {
	system("cls");
}
#elif
void clear() {
	system("clear");
}
#endif

void waitForInput() {
	cout << endl << "Nacisnij dowolny klawisz aby kontynuowac...";
	_getch();
}

void encrypt(char tab[RECORD_SIZE], int key) {
	for (int i = 0; i < RECORD_SIZE; i++) {
		tab[i] += key;
	}
}

void decrypt(char tab[RECORD_SIZE], int key){
	for (int i = 0; i < RECORD_SIZE; i++) {
		tab[i] -= key;
	}
}


string getDate() {
	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);

	string seconds = 1 + ltm.tm_sec < 9 ? '0' + to_string(1 + ltm.tm_sec) : to_string(1 + ltm.tm_sec);
	string months = 1 + ltm.tm_mon < 9 ? '0' + to_string(1 + ltm.tm_mon) : to_string(1 + ltm.tm_mon);
	string days = 1 + ltm.tm_mday < 9 ? '0' + to_string(1 + ltm.tm_mday) : to_string(1 + ltm.tm_mday);
	string hours = 1 + ltm.tm_hour < 9 ? '0' + to_string(1 + ltm.tm_hour) : to_string(1 + ltm.tm_hour);
	string minutes = 1 + ltm.tm_min < 9 ? '0' + to_string(1 + ltm.tm_min) : to_string(1 + ltm.tm_min);

	string date = to_string(1900 + ltm.tm_year) + '.' + months + '.' + 
		days + ' ' + hours + ':' + minutes + ':' + seconds;

	return date;
}

User* BankSystem::findUser(string accountNumber_) {
	for (int i = 0; i < amountOfUsers; i++)
		if (users[i]->getAccNum() == accountNumber_)
			return users[i];
	return nullptr;
}


BankSystem::BankSystem(string fileName_) {
	clear();

	fileName = fileName_ + ".bin";

	ifstream dataFile;

	users = nullptr;
	loggedUser = nullptr;
	amountOfUsers = 0;

	dataFile.open(fileName, ios::binary);

	if (dataFile.is_open() == false) {
		cout << "Nie mozna otworzyc pliku!" << endl;
		waitForInput();
	} else {
		dataFile.read((char*)&amountOfUsers, sizeof(int));

		users = new User*[amountOfUsers];

		auto userData = new char[amountOfUsers][RECORD_SIZE]();

		for (int i = 0; i < amountOfUsers; i++) {
			dataFile.read((char*)userData[i], RECORD_SIZE);
			decrypt(userData[i], amountOfUsers);
			stringstream ss(userData[i]);
			char userType;
			string login, number, password, balance;
			ss >> userType;
			if (userType == 'c') {
				ss >> login;
				ss >> password;
				ss >> number;
				ss >> balance;
				users[i] = new Client(this, login, password, number, stof(balance));
			} else if (userType == 'a') {
				ss >> login;
				ss >> password;
				users[i] = new Administrator(this, login, password);
			}

		}

		dataFile.close();
		start();
	}
}

void BankSystem::start() {
	char ans = '0';
	while (logIn()) {
		while (loggedUser != nullptr) {
			clear();
			loggedUser->printMenu();
			ans = _getch();
			loggedUser->option(ans);
		}
	}
}

bool BankSystem::logIn() {  
	string username, password;
	char ch;

	do {
		clear();
		cout << "1. Zaloguj sie." << endl;
		cout << "2. Dodaj uzytkownika." << endl;
		cout << "3. Wyjdz." << endl;
		ch = _getch();
	} while (ch != '1' && ch != '2' && ch != '3');

	if (ch == '3')
		return false;
	else if (ch == '2') {
		addUser();
		return true;
	}

	clear();

	cout << "Nazwa uzytkownika: ";
	cin >> username;

	cout << endl << "Haslo: ";
	while ((ch = _getch()) != 13) { 
		password += ch;
		cout << '*';
	}

	for (int i = 0; i < amountOfUsers; i++)
		if (users[i]->login == username && users[i]->password == password)
			loggedUser = users[i];

	if (loggedUser == nullptr) {
		cout << endl << endl << "Niepoprawna nazwa uzytkownika lub haslo." << endl;
		waitForInput();
	}

	return true;
}

void BankSystem::logOut() {
	loggedUser = nullptr;
}

void BankSystem::deleteUser() {
	clear();
	cout << "Podaj numer konta uzytkownika do usuniecia: ";
	string num;
	cin >> num;

	User* target = findUser(num);

	if (target == nullptr) {
		cout << "Nie znaleziono uzytkownika o podanym numerze konta.";
		waitForInput();
		return;
	}

	amountOfUsers--;

	User** newTab = new User * [amountOfUsers];

	int i = 0;
	while (users[i] != target) {
		newTab[i] = users[i];
		i++;
	}
	delete users[i];
	i++;
	for (int j = i; j < amountOfUsers; j++) {
		newTab[j] = users[j + 1];
	}

	delete[] users;
	users = newTab;
}

bool BankSystem::addUser() {
	string login, pass1, pass2, accountNumber;
	char ch, userType;
	clear();

	cout << "1. Uzytkownik." << endl
		<< "2. Administrator." << endl
		<< "3. Usun uzytkownika." << endl
		<< "4. Wroc.";
	userType = _getch();
	if (userType == '4')
		return false;
	if (userType == '3') {
		deleteUser();
		return true;
	}

	clear();

	cout << "Podaj nazwe uzytkownika: " ;
	cin >> login;
	for (int i = 0; i < amountOfUsers; i++) {
		if (users[i]->login == login) {
			cout << "Taki uzytkownik juz istnieje." << endl;
			waitForInput();
			return false;
		}
	}
	if (login.size() > 16) {
		cout << "Zbyt dluga nazwa uzytkownika.";
		waitForInput();
		return false;
	}

	cout << "Podaj i potwierdz haslo: " << endl;
	cout << "Haslo: ";
	while ((ch = _getch()) != 13) {
		pass1 += ch;
		cout << '*';
	}
	cout << endl;
	if (pass1.size() > 16) {
		cout << "Zbyt dlugie haslo." << endl;
		waitForInput();
		return false;
	}

	cout << "Potwierdz haslo: ";
	while ((ch = _getch()) != 13) {
		pass2 += ch;
		cout << '*';
	}
	cout << endl;

	if (pass1 != pass2) {
		cout << "Podane hasla roznia sie." << endl;
		waitForInput();
		return false;
	}
	if (userType == '1') {
		cout << "Podaj numer konta nowego uzytkownika: " << endl;
		cin >> accountNumber;
		if (findUser(accountNumber) != nullptr) {
			cout << "Uzytkownika o takim numerze konta juz istnieje." << endl;
			waitForInput();
			return false;
		}
		if (accountNumber.size() > 26) {
			cout << "Zbyt dlugi numer konta." << endl;
			waitForInput();
			return false;
		}
	}

	amountOfUsers++;

	User** newUsers = new User * [amountOfUsers];

	for (int i = 0; i < amountOfUsers - 1; i++) {
		newUsers[i] = users[i];
	}
	if (userType == '1') {
		newUsers[amountOfUsers - 1] = new Client(this, login, pass1, accountNumber, 0);
	} else if (userType == '2') {
		newUsers[amountOfUsers - 1] = new Administrator(this, login, pass1);
	}

	User** temp = users;
	delete[] temp;
	users = newUsers;

	return true;
}

BankSystem::~BankSystem(){
	ofstream dataFile;
	dataFile.open(fileName, ios::trunc | ios::binary);

	dataFile.write((char*)&amountOfUsers, sizeof(int));

	for (int i = 0; i < amountOfUsers; i++) {
		string out = users[i]->dataString();

		char data[RECORD_SIZE];

		strcpy_s(data, out.c_str());
		encrypt(data, amountOfUsers);
		dataFile.write((char*)&data, RECORD_SIZE);
	}

	for (int i = 0; i < amountOfUsers - 1; i++)
		delete users[i];
	delete[] users;

	dataFile.close();
}

User::User(){
	system = nullptr;
}

User::User(BankSystem* system_, string login_, string password_) {
	system = system_;
	login = login_;
	password = password_;
}

Client::Client(BankSystem* system_, string login_, string password_, string accountnumber_, float balance_) : User(system_, login_, password_) {
	balance = balance_;
	accountNumber = accountnumber_;
}

string Client::dataString() {
	string sbalance;
	stringstream ss;
	ss.precision(2);
	ss << fixed << balance;
	string output =  "c\n" + login + '\n' + password + '\n' + accountNumber + '\n' + ss.str();

	return output;
}

void Client::printMenu() {
	clear();
	cout << "Zalogowany uzytkownik: " << this->login << endl << endl;
	cout << "1. Przelew" << endl
		<< "2. Sprawdz stan konta" << endl
		<< "3. Sprawdz historie operacji" << endl
		<< "4. Wyloguj" << endl;
}

void Client::option(char ans) {
	switch (ans){
	case '1': this->transfer();
		break;
	case '2': this->checkBalance();
		break;
	case '3': this->printHistory();
		break;
	case '4': system->logOut();
		break;
	default: break;
	}
}

bool Client::transfer() {
	clear();

	cout << "Podaj numer konta odbiorcy: " << endl;
	string accountnum;
	cin >> accountnum;

	User* reciever = nullptr;

	reciever = system->findUser(accountnum);

	if (reciever == nullptr) {
		cout << "Nie mozna znalezc odbiorcy!" << endl;
		waitForInput();
		return false;
	}

	cout << "Podaj kwote przelewu: " << endl;
	float amount;
	cin >> amount;

	if (amount > this->balance) {
		cout << "Nie wystarczjace fundusze na koncie!" << endl;
		waitForInput();
		return false;
	}

	cout << "Tytul przelewu: " << endl;
	string title;
	cin >> title;
	waitForInput();

	this->removeFunds(amount, reciever->getAccNum(), reciever->getLogin(), title);
	reciever->addFunds(amount, accountNumber, login, title);

	return true;
}

void Client::addFunds(float amount, string fromNr, string from, string title) {
	balance += amount;
	ofstream history;
	history.open(this->accountNumber + ".txt", ios::app);
	history << "Przelew przychodzacy" << endl << getDate() << endl << "Nadawca: " << from << endl
		<< "Numer konta nadawcy: " << fromNr << endl << "Kwota: " << amount << endl << "Stan konta po przelewie: " << balance << endl << endl;
	history.close();
}

void Client::removeFunds(float amount, string recieverNr, string reciever, string title) {
	balance -= amount;
	ofstream history;
	history.open(this->accountNumber + ".txt", ios::app);
	history << "Przelew wychodzacy" << endl << getDate() << endl << "Odbiorca: " << reciever << endl
		<< "Numer konta odbiorcy: " << recieverNr << endl << "Kwota: " << amount << endl << "Stan konta po przelewie: " << balance << endl << endl;
	history.close();
}

string Client::getAccNum() {
	return this->accountNumber;
}

string Client::getLogin() {
	return this->login;
}

void Client::checkBalance() {
	cout <<endl << "Nr konta: " << accountNumber << endl;
	cout << "Stan konta: " << balance << endl;
	waitForInput();
}

void Client::printHistory() {
	clear();
	ifstream history;
	history.open(this->accountNumber + ".txt");
	string output;
	while (getline(history, output))
		cout << output << endl;
	history.close();
	waitForInput();
}

void Client::print() {
	cout << login << endl;
	cout << password << endl;
	cout << accountNumber << endl;
	cout << fixed << setprecision(2) << balance << endl << endl;
}

Administrator::Administrator(BankSystem* system_, string login_, string password_) : User(system_, login_, password_) {}


string Administrator::dataString() {
	string output = "a\n" + login + '\n' + password;

	return output;
}

string Administrator::getAccNum() {
	return "";
}

void Administrator::printMenu() {
	clear();
	cout << "Zalogowany uzytkownik: " << this->login << endl << endl;
	cout << "1. Wyswietl wszystkich uzytkownikow." << endl
		<< "2. Wyswietl jednego uzytkownika." << endl
		<< "3. Sprawdz historie uzytkownika." << endl
		<< "4. Dodaj fundusze do konta uzytkownika." << endl
		<< "5. Wyloguj.";
}

void Administrator::option(char ans) {
	switch (ans) {
	case '1': this->printAllUsers();
		break;
	case '2': this->printUser();
		break;
	case '3': this->printHistory();
		break;
	case '4': this->addFunds();
		break;
	case '5': system->logOut();
		break;
	default: break;
	}
}

void Administrator::printAllUsers() {
	clear();
	for (int i = 0; i < system->amountOfUsers; i++)
		system->users[i]->print();
	waitForInput();
}

void Administrator::printUser() {
	clear();
	string accountNum;
	cout << endl << "Podaj numer konta uzytkownika do wyswietlenia: " << endl;
	cin >> accountNum;
	User* target = system->findUser(accountNum);
	if (target == nullptr) {
		cout << "Nie znaleziono konta o podanym numerze." << endl;
		waitForInput();
	} else {
		target->print();
		waitForInput();
	}
}

void Administrator::printHistory() {
	clear();
	string accountNum;
	cout << "Podaj numer konta do wyswietlenia historii: " << endl;
	cin >> accountNum;
	User* target = system->findUser(accountNum);
	if (target == nullptr) {
		cout << "Nie znaleziono konta o podanym numerze." << endl;
		waitForInput();
	} else {
		target->printHistory();
	}
}

void Administrator::addFunds() {
	clear();
	string accountNum;
	cout << "Podaj numer konta do dodania funduszy: " << endl;
	cin >> accountNum;

	User* target = system->findUser(accountNum);
	if (target == nullptr) {
		cout << "Nie znaleziono konta o podanym numerze." << endl;
		waitForInput();
	}

	float amount = 0;
	cout << "Podaj kwote do dodanie funduszy: " << endl;
	cin >> amount;

	cout << "Tytul: " << endl;
	string title;
	cin >> title;

	target->addFunds(amount, "*ADMIN*", this->login, title);
	waitForInput();

}

void Administrator::print() {
	cout << login << endl;
	cout << password << endl << endl;
}

void Administrator::addFunds(float amount, string fromNr, string from, string title){}

void Administrator::removeFunds(float amount, string recieverNr, string reciever, string title) {}

string Administrator::getLogin() {
	return this->login;
}