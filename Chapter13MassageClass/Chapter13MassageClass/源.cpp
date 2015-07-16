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
	void moveFolders(Message * m);
	void save(Folder &);
	void remove(Folder &);
	friend void swap(Message & lhs, Message & rhs);
	Message::Message(Message && m) :contents(std::move(m.contents))
	{
		moveFolders(&m);
	}
	Message & operator=(Message && msg);
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
	Folder(Folder && f);
	Folder & operator=(Folder && f);
	Folder& operator=(const Folder &);
	void addMessage(Message &);
	void removeMessage(Message &);
	friend void swap(Folder & lhs, Folder & rhs);
	void moveMessage(Folder * folder);
	~Folder();
	
private:
	set<Message*> messages;
	void removeFromMessage();
	void addToMessage(const Folder & );
};


void Message::moveFolders(Message * m)
{
	folders = std::move(m->folders);
	for (auto p : m->folders){
		p->removeMessage(*m);
		p->addMessage(*this);
	}
	m->folders.clear();
}

/**
�����ⲿ����
message
��ʵ�ִ���
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
	using std::swap;//��һ������Ҫ
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

Message & Message::operator=(Message && msg)
{
	if (this != &msg){
		removeFromFolders(*this);
		contents = msg.contents;
		moveFolders(&msg);
	}
	return *this;
}
/*
������Folder
����
�Ĵ���
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

void Folder::moveMessage(Folder * folder)
{
	messages = std::move(folder->messages);
	for (auto p : messages){
		p->remove(*folder);
		p->save(*this);
	}
	folder->messages.clear();
}


Folder::Folder(Folder && folder)
{
	moveMessage(&folder);
}

void Folder::removeFromMessage()
{
	for (auto p : messages){
		p->remove(*this);
	}
	messages.clear();
}

void Folder::addToMessage(const Folder & f)
{
	for (auto p : f.messages){
		p->save(*this);
	}
}

Folder & Folder:: operator=(Folder && folder)
{
	if (this != &folder){
		removeFromMessage();
		addToMessage(folder);
		messages = std::move(folder.messages);
		folder.messages.clear();
	}
	return *this;
}
