/*********************************************************************************
  *Date:  2022.06.19
  *Description:  密码类，
  *				 主要用于保存账号和密码，进行自动登录。
**********************************************************************************/
#pragma once
#include <iostream>
#include <vector>



struct Message {
	std::string account;
	std::string password;
	int flag;
};


class Password
{
public:
	Password();

	void ReadAllAccount(std::vector<Message>& g);
	void WriteAllAccount(std::vector<Message>& g);
	Message AcquireDefaultAccount();
	Message SearchAccount(std::string account);
	void ChangeDefault(std::vector<Message>& g, Message& data);
	int AddAccount(std::vector<Message>& g, Message& data);
	void DeleteAccount(std::vector<Message>& g, std::string account);
	void SaveMessage(std::string account, std::string password, int flag);
};
