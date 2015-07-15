#include <iostream>
#include <string>
#include <set>
using namespace std;

class Folder;
class Message{
	friend class Folder;
public:
	Message(const string & s = "") :contents(s){}
	Message(const Message & );
	Message & operator=(const Message &);
	~Message();
	void save(Folder &);
	void remove(Folder &);
	friend void swap(Message & lhs, Message & rhs);
private:
	string contents;
	set <Folder*> folders;
	void addToFolders(const Message &);
	void removeFromFolders(const Message &);
};

class Folder{
	friend class Message;
public:
	Folder(const Folder & );
	Folder& operator=(const Folder &);
	void addMessage(Message &);
	void removeMessage(Message &);
	friend void swap(Folder & lhs, Folder & rhs);
	~Folder();
private:
	set<Message*> messages;
};


/**
下面这部分是
message
的实现代码
*/
Message::Message(const Message & rhs)
	:contents(rhs.contents), folders(rhs.folders)
{
	addToFolders(rhs);
}

Message & Message::operator=(const Message & rhs)
{
	if (this != &rhs)
	{
		removeFromFolders(*this);
		contents = rhs.contents;
		folders = rhs.folders;
		addToFolders(rhs);
	}
	return *this;
}

void Message::save(Folder & f)
{
	folders.insert(&f);
	f.addMessage(*this);
}

void Message::remove(Folder & f)
{
	folders.erase(&f);
	f.removeMessage(*this);
}

void Message::addToFolders(const Message& msg)
{
	for (auto it = msg.folders.begin(); it != msg.folders.end(); ++it)
	{
		(*it)->addMessage(*this);
	}
}

void Message::removeFromFolders(const Message& msg)
{
	for (auto it = msg.folders.begin(); it != msg.folders.end(); ++it)
	{
		(*it)->removeMessage(*this);
	}
}

Message::~Message()
{
	for (auto p : folders)
	{
		p->removeMessage(*this);
	}

}

void swap(Message & lhs, Message & rhs){
	using std::swap;//这一步很重要
	for (auto p : lhs.folders){
		p->removeMessage(lhs);
	}
	for (auto p : rhs.folders){
		p->removeMessage(rhs);
	}
	swap(lhs.folders, rhs.folders);
	swap(lhs.contents, rhs.contents);
	for (auto p : lhs.folders){
		p->addMessage(lhs);
	}
	for (auto p : rhs.folders){
		p->addMessage(rhs);
	}

}

/*
下面是Folder
部分
的代码
*/

Folder::Folder(const Folder & rhs)
	:messages(rhs.messages){
	for (auto f : messages){ 
		f->save(*this);
	}
}

Folder & Folder::operator=(const Folder & rhs)
{
	if (this != &rhs){
		messages = rhs.messages;
		for (auto p : messages){
			p->save(*this);
		}
	}
}


void Folder::addMessage(Message & msg)
{
	messages.insert(&msg);
	msg.save(*this);
}


void Folder::removeMessage(Message & msg)
{
	messages.erase(&msg);
	msg.remove(*this);
}

Folder::~Folder()
{
	for (auto p : messages)
	{
		p->remove(*this);
	}
}

void swap(Folder & lhs, Folder & rhs)
{
	using std::swap;
	for (auto p : lhs.messages){
		p->remove(lhs);
	}
	for (auto p : rhs.messages){
		p->remove(rhs);
	}
	swap(lhs.messages, rhs.messages);
	for (auto p : lhs.messages){
		p->save(lhs);
	}
	for (auto p : rhs.messages){
		p->save(rhs);
	}
}


