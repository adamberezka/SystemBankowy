#include <iostream>
#include <string>
#include <fstream>
#include <conio.h>
#include <sstream>
#include <iomanip>
#include <time.h>

#pragma once

using namespace std;

const int RECORD_SIZE = 100;

/*
17. Prosty system bankowy,
u¿ytkownik ma mo¿liwoœæ logowania,
istniej¹ 2 role administrator i u¿ytkownik,
dane przechowywane w pliku binarnym (szyfrowanym),
dla ka¿dego klienta mo¿e istnieæ plik zawieraj¹cy historiê
*/

string getDate();

void clear();

void waitForInput();

void encrypt(char tab[RECORD_SIZE], int key);

void decrypt(char tab[RECORD_SIZE], int key);

class User;
class Administrator;

class BankSystem {
private:
	friend class Client;
	friend class Administrator;

	User** users;
	int amountOfUsers;
	string fileName;
	User* loggedUser;

public:
	BankSystem(string fileName_);
	~BankSystem();

	bool addUser();
	void deleteUser();
	bool logIn();
	void logOut();
	void start();
	User* findUser(string accountNumber_);
};

class User {
protected:
	friend class BankSystem;

	BankSystem* system;
	string login;
	string password;

public:
	User();
	User(BankSystem* system_, string login_, string password_);

	virtual void printMenu() = 0;
	virtual void option(char ans) = 0;
	virtual void print() = 0;
	virtual string getAccNum() = 0;
	virtual void addFunds(float amount, string fromNr, string form, string title) = 0;
	virtual void removeFunds(float amount, string recieverNr, string reciever, string title) = 0;
	virtual void printHistory() = 0;
	virtual string dataString() = 0;
	virtual string getLogin() = 0;
};

class Client : public User {
private:

	string accountNumber;
	float balance;

public:
	Client(BankSystem* system_, string login_, string password_,string accountnumber_, float balance_);

	bool transfer();
	void checkBalance();
	void printMenu();
	void option(char ans);
	void print();
	void printHistory();
	string getAccNum();
	void addFunds(float amount, string fromNr, string from, string title);
	void removeFunds(float amount, string recieverNr, string reciever, string title);
	string dataString();
	string getLogin();
};

class Administrator : public User {
public:
	Administrator(BankSystem* system_, string login_, string password_);
	void printMenu();
	void option(char ans);
	void print();
	void printAllUsers();
	void printUser();
	void printHistory();
	void addFunds();
	void addFunds(float amount, string fromNr, string from, string title);
	void removeFunds(float amount, string recieverNr, string reciever, string title);
	string getAccNum();
	string getLogin();
	string dataString();
};
