#include "../include/Chat.h"

void Chat::tcpConnect()
{
#ifdef _WIN32
	result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != 0) {
		std::cerr << "Ошибка при инициализации Winsock" << std::endl;
		exit(1);
	}
#endif

	clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_address.sin_port = htons(8000);
	result = connect(clientsocket, (sockaddr*)&server_address, sizeof(server_address));
#ifdef _WIN32
	if (result == SOCKET_ERROR) {
		std::cerr << "Ошибка при подключении к серверу" << std::endl;
		closesocket(clientsocket);
		WSACleanup();
		exit(1);
	}
#else
    if (result == -1)
    {
        std::cerr << "Error connecting to server" << std::endl;
        close(clientsocket);
        exit(1);
    }
#endif

}

void Chat::startChat()
{
	_isChatWork = true;
}

void Chat::showLoginMenu()
{
	_currentUser = nullptr;
	char operation;
	std::cout << "\t\tChat 2.0 is run.\n\a";
	do
	{
		
		std::cout << "\n(1)SignUp\t";
		std::cout << "(2)Login\t";
		std::cout << "(0)ShutDown\n>> ";
		std::cin >> operation;

		switch (operation)
		{
		case '1':
			try
			{
				singUp();
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
			break;
		case '2':
			login();
			break;
		case'0':
			_isChatWork = false;
			break;
		default:
			std::cout << "1 or 2..." << std::endl;
			break;
		}
	} while (!_currentUser && _isChatWork);
}

void Chat::showUserMenu()
{
	char operation;
	std::cout << "Hi, " << _currentUser->getUserName()<<".\n";

	while (_currentUser)
	{
		std::cout << "Menu:(1)Show chat | (2)Add message | (3)Users | (0)Logout" << "\n>> ";
		std::cin >> operation;
		switch (operation)
		{
		case '1':
			showChat();
			break;
		case '2':
			addMessage();
			break;
		case '3':
			showAllUsersName();
			break;
		case '4':
			//deleteLastMessage();
			break;
		case '0':
			_currentUser = nullptr;
			break;
		default:
			std::cout << "Unknown choice.." << std::endl;
			break;
		}
	}
}

void Chat::showAllUsersName() const
{
#ifdef _WIN32
	std::locale::global(std::locale("en_US.UTF-8"));
	uint32_t Spades = 0x2642;
	uint32_t Spades1 = 0x2640;
#endif

	std::cout << "--- Users ---" << std::endl;

	std::ifstream user_file("users.txt");

	std::string login;
	std::string password;
	std::string name;
	std::string gender;

	while (user_file >> login >> password >> name >> gender)
	{
		User user(login, password, name, gender);

#ifdef _WIN32
		if (user.getUserGender() == "Male")
			std::wcout << (wchar_t)Spades << " ";
		else if (user.getUserGender() == "Female")
			std::wcout << (wchar_t)Spades1 << " ";
#else
		if (user.getUserGender() == "Male")
			std::cout << "M ";
		else if (user.getUserGender() == "Female")
			std::cout << "W ";
#endif

		

		std::cout << user.getUserName();

		if (login == _currentUser->getUserLogin())
			std::cout << "(me)";

		std::cout << std::endl;
	}

	std::cout << "----------" << std::endl;
}

void Chat::singUp()
{
	std::string login, name, gender, password;
	std::cout << "Login: ";
	std::cin >> login;
	std::cout << "Password: ";
	std::cin >> password;
	std::cout << "Name: ";
	std::cin >> name;
	std::cout << "Gender:";

	do
	{
		std::cout << "\n(Male,Female) ";
		std::cin >> gender;
		if (!(gender == "Male" || gender == "Female"))
			std::cout << "Enter Male or Female";
	} while (!(gender == "Male" || gender == "Female"));

	if (getUserByLogin(login) || login == "All")
		throw UserLoginExp();
	if (getUserByName(name) || name == "All")
		throw UserNameExp();

	User user(login, password, name, gender);
	_users.push_back(user);
	_currentUser = std::make_shared<User>(user);

	if (!user_file)
		user_file = std::fstream("users.txt", std::ios::in | std::ios::out | std::ios::trunc);

	if (user_file)
	{
		User obj(login, password, name, gender);
		user_file << obj << std::endl;
	}
	else
		std::cout << "Could not open file users.txt!" << std::endl;
}


void Chat::login()
{
	std::string login;
	std::string password;
	char operation;

	do
	{
		std::cout << "Login: ";
		std::cin >> login;

		_currentUser = getUserByLogin(login);
		if (_currentUser == nullptr)
		{
			std::cout << "Login invalid." << std::endl;
			std::cout << "(0)exit or any key to return menu: ";
			std::cin >> operation;

			if (operation == '0')
				break;

			continue;
		}

		std::cout << "Password: ";
		std::cin >> password;

		if (password != _currentUser->getUserPassword())
		{
			_currentUser = nullptr;

			std::cout << "Login invalid." << std::endl;
			std::cout << "(0)exit or any key to return menu: ";
			std::cin >> operation;

			if (operation == '0')
				break;
		}

	} while (!_currentUser);
}

void Chat::showChat() const
{
	std::string requestRecv = "recv";
	int requestLength = requestRecv.length();
	send(clientsocket, requestRecv.c_str(), requestLength, 0);

	std::string from, to, text;
	char buffer[1024] = {};

	std::cout << "____START____ "<< std::endl << std::endl;

	while (true) 
	{
		int recv_size = recv(clientsocket, buffer, sizeof(buffer), 0);
		if (recv_size < 0) {
			std::cout << "Ошибка при чтении сообщения" << std::endl;
			break;
		}
		else if (recv_size == 0) {
			std::cout << "Сервер закрыл соединение" << std::endl;
			break;
		}
		else 
		{
			std::cout << buffer << std::endl;
			break;
		}
	}
	std::cout << "____END____ " << std::endl << std::endl;
}

void Chat::sendMessage(SOCKET clientSocket, const std::string& login, const std::string& to, const std::string& text) 
{
	std::string message = _currentUser->getUserName() + " " + to + " " + text;
	int messageLength = message.length();

	// Отправляем сообщение на сервер
	send(clientSocket, message.c_str(), messageLength, 0);
}

void Chat::addMessage()
{
	std::string requestSend = "send";
	int requestLength = requestSend.length();
	send(clientsocket, requestSend.c_str(), requestLength, 0);

	std::string to, text;

	std::cout << "To (name or All): ";
	std::cin >> to;
	std::cout << "Text: ";
	std::cin.ignore();
	std::getline(std::cin, text);

	std::ifstream users_file("users.txt");

	if (strToUpper(to) == "ALL")
	{
		Message message(_currentUser->getUserLogin(), "All", text);
		_messages.push_back(message);
		sendMessage(clientsocket, _currentUser->getUserLogin(), "All", text); // Отправляем сообщение на сервер
	}
	else
	{
		std::string login, password, name, gender;

		bool found = false;
		while (users_file >> login >> password >> name >> gender)
		{
			if (strToUpper(name) == strToUpper(to))
			{
				found = true;
				Message message(_currentUser->getUserLogin(), login, text);
				_messages.push_back(message);
				sendMessage(clientsocket, _currentUser->getUserLogin(), getUserByLogin(login)->getUserName(), text); // Отправляем сообщение на сервер
				break;
			}
		}
		if (!found)
		{
			std::cout << "Error: user not found\n";
			return;
		}
	}
}

void Chat::deleteLastMessage()
{
	if (!(_messages.empty()))
	{
		Message s = _messages.back();

		if ((_currentUser->getUserName() == "Admin") || (s.getFrom() == _currentUser->getUserLogin()))
		{
			_messages.pop_back();
			std::cout << std::endl << "Message was deleted." << std::endl;
		}
		else
		{
			std::cout << "Not enough rights to delete." << std::endl;
		}
	}
	std::cout << "Chat is empty!" << std::endl;
}

std::shared_ptr<User> Chat::getUserByLogin(const std::string& login) const
{
	std::ifstream user_file("users.txt");
	if (!user_file.is_open())
	{
		std::cout << "Error opening users file." << std::endl;
		return nullptr;
	}

	std::string userLogin;
	std::string userPassword;
	std::string userName;
	std::string userGender;

	while (user_file >> userLogin >> userPassword >> userName >> userGender)
	{
		if (userLogin == login)
		{
			return std::make_shared<User>(userLogin, userPassword, userName, userGender);
		}
	}
	return nullptr;
}

std::shared_ptr<User> Chat::getUserByName(const std::string& name) const
{
	for (auto& user : _users)
	{
		if (name == user.getUserName())
			return std::make_shared<User>(user);
	}
	return nullptr;
}

std::fstream& operator >>(std::fstream& is, User& obj)
{
	is >> obj._name;
	is >> obj._login;
	is >> obj._password;
	is >> obj._gender;
	return is;
}
std::ostream& operator <<(std::ostream& os, const User& obj)
{
	os << obj._login;
	os << ' ';
	os << obj._password;
	os << ' ';
	os << obj._name;
	os << ' ';
	os << obj._gender;
	return os;
}

std::ostream& operator <<(std::ostream& os, const Message& msg)
{
	os << msg._from;;
	os << ' ';
	os << msg._to;
	os << ' ';
	os << msg._text;
	return os;
}

std::string Chat::strToUpper(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return toupper(c); });
	return str;
}
