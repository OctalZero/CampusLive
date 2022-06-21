#include "Password.h"
#include <iostream>
#include <string>
#include <fstream>

Password::Password()
{

}

void Password::ReadAllAccount(std::vector<Message>& g)
{
	std::ifstream ifs; //open file
	ifs.open("./userMessage.txt", std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "open input file faile\n";
		return;
	}
	while (1) {
		Message f;
		ifs >> f.account;
		if (!ifs.eof()) {
			ifs >> f.password >> f.flag;
			g.push_back(f);  //read file
		}
		else {
			break;
		}
	}

	std::cin.clear();
	ifs.close();
}

void Password::WriteAllAccount(std::vector<Message>& g)
{
	std::ofstream ofs;
	ofs.open("./userMessage.txt", std::ios::out);
	if (!ofs.is_open()) {
		std::cout << "open output file faile\n";
		return;
	}
	for (auto mid : g) {
		ofs << mid.account << "\t" << mid.password << "\t" << mid.flag << std::endl;
	}
	ofs.close();
}

Message Password::AcquireDefaultAccount()
{
	std::vector<Message> accounts;
	ReadAllAccount(accounts);
	Message tmp;
	if (accounts.size() == 0) {
		tmp.flag = -1;
		return  tmp;
	}
	tmp.account = accounts[0].account;
	tmp.password = accounts[0].password;
	tmp.flag = accounts[0].flag;
	return tmp;
}

Message Password::SearchAccount(std::string account)
{
	std::vector<Message> accounts;
	ReadAllAccount(accounts);
	Message tmp;
	if (accounts.size() == 0) {
		tmp.flag = -1;
		return tmp;
	}
	for (auto mid : accounts) {
		if (mid.account == account) {
			tmp.account = mid.account;
			tmp.password = mid.password;
			tmp.flag = mid.flag;
			break;
		}
	}

	return tmp;
}

void Password::ChangeDefault(std::vector<Message>& g, Message& data)
{
	Message tmp;
	int len = g.size();
	for (int i = 0; i < len; i++) {
		if (g[i].account == data.account) {
			if (i == 0) {
				g[0].account = data.account;
				g[0].password = data.password;
				g[0].flag = data.flag;
				return;
			}
			tmp.account = g[0].account;
			tmp.password = g[0].password;
			tmp.flag = g[0].flag;
			g[0].account = data.account;
			g[0].password = data.password;
			g[0].flag = data.flag;
			g[i].account = tmp.account;
			g[i].password = tmp.password;
			if (tmp.flag == 2) tmp.flag = 1;
			g[i].flag = tmp.flag;
			return;
		}
	}
	if (len > 0) {

		AddAccount(g, g[0]);
		std::cout << "enter add  " << g.size() << std::endl;
		g[0].account = data.account;
		g[0].password = data.password;
		g[0].flag = data.flag;
	}
	else {
		AddAccount(g, data);
	}
}

int Password::AddAccount(std::vector<Message>& g, Message& data)
{
	g.push_back(data);
	return 0;
}

void Password::DeleteAccount(std::vector<Message>& g, std::string account)
{
	for (int i = 0; i < g.size(); i++) {
		if (g[i].account == account) {
			g.erase(g.begin() + 1);
			return;
		}
	}
}

void Password::SaveMessage(std::string account, std::string password, int flag)
{
	std::vector<Message> accounts;
	ReadAllAccount(accounts);
	Message data;
	data.account = account;
	data.flag = flag;
	data.password = password;
	ChangeDefault(accounts, data);
	WriteAllAccount(accounts);
}
