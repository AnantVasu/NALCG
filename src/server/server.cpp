#include "server.h"
#include "serversocket.h"
#include "user.h"
#include "socketexception.h"
#include <list>
#include <pthread.h>
#include <string>
#include <iostream>

Server::Server()
{
    mSocket = new ServerSocket( 30000 );
    std::cout << "Server started at " << getTime(1) << ".\nRunning....\n";
    pthread_rwlock_init(&mLock, NULL);
}

Server::~Server()
{
    delete mSocket;
}

void Server::accept(ServerSocket& socket)
{
    mSocket->accept(socket);
}

User* Server::addUser()
{
    ServerSocket *new_sock = new ServerSocket();
    mSocket->accept ( *new_sock );
    std::string name;
    if( MAX > mClients.size() )
    {
        bool isValid = true;
        do
        {
            if(!isValid)
	    {
	        *new_sock << "The nickname you have chosen is already taken. Please choose another one.";
	    }
            *new_sock << "Nickname:";
            name = "";
            *new_sock >> name;
            isValid = checkName(name);
	 } while(!isValid);
         if(name.compare("MSG_Q") == 0)
	 {
             delete new_sock;
	     return NULL;
         }
	 
	std::string users = "\nOther users:"; 
	pthread_rwlock_rdlock(&mLock);
	std::list<User*>::iterator i;
        for ( i = mClients.begin(); i != mClients.end(); ++i)
	  {
            *((*i)->getSocket()) << getTime(1) + " New user " + name + " logged in.";
            users += "\n" + (*i)->getName();
	  }
        pthread_rwlock_unlock(&mLock);
        User *user = new User(new_sock, name);

	pthread_rwlock_wrlock(&mLock);
        mClients.push_front(user);
	pthread_rwlock_unlock(&mLock);
        *(user->getSocket()) << "Welcome to NALCG!" + users;
        return user;
    }
    else
    {
        *new_sock << "The server is full. Please try again later.";
        delete new_sock;
	return NULL;
    }  
}

void Server::quittedUsers()
{
    User *temp;
    bool found;
    while( true ) {
        pthread_rwlock_wrlock(&mLock);
        int k;
        found = false;

        std::list<User*>::iterator i;
        for ( i = mClients.begin(); i != mClients.end(); ++i)
        {
	    if(!(*i)->testConnection())
	    {
	        found = true;
	        temp = (*i);
	        break;
	    }
        }

        if(found)
        {
            mClients.remove(temp);
	    k = pthread_cancel(temp->getSid());
	    
	    for ( i = mClients.begin(); i != mClients.end(); ++i)
	    {
	        if((*i)->testConnection())
	        {
		    *((*i)->getSocket()) << getTime(1) + " User " + temp->getName() + " quitted.";
	        }
	    } 
	    delete temp;
        }
        pthread_rwlock_unlock(&mLock);
        sleep(1);
    }
}

std::string Server::getTime(int value)
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];
    std::string thisTime;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    if(value == 1) strftime (buffer,80,"%T",timeinfo);
    else strftime (buffer,80,"%T %d.%m.%Y",timeinfo);
    thisTime = buffer;
    return thisTime;
}

bool Server::checkName(std::string &name)
{
    bool cond = true;
    std::list<User*>::iterator i;
    pthread_rwlock_rdlock(&mLock);
    for ( i = mClients.begin(); i != mClients.end(); ++i) 
    {
        if(name.compare((*i)->getName()) == 0 ) cond = false;
    }
    pthread_rwlock_unlock(&mLock);
    return cond;
}

void Server::sendMsg(std::string& msg)
{
    std::list<User*>::iterator i;
    pthread_rwlock_rdlock(&mLock);
    for ( i = mClients.begin(); i != mClients.end(); ++i)
    {
        *((*i)->getSocket()) << msg;
    }
    pthread_rwlock_unlock(&mLock);
}

void Server::clientService(User* user)
{
    ServerSocket *sock = user->getSocket();
    std::string name = user->getName();
    std::string data, send;
    while(true)
    {
        try {
            data = "";
            send = "";
            *sock >> data;
	    if(isCtrlMsg(data))
	    {
	        doCtrl(data, user);
	        continue;
	    }
	    if(user->isPlaying())
	    {
	        sendMsgPlaying(data, user);
	    }
            else
	    {  
                send += getTime(1) + " " + name + ": " + data;
                sendMsg(send);
            }
        }
        catch ( SocketException& e ) { }
    }

}

bool Server::isCtrlMsg(std::string& msg)
{
    if(msg.substr(0, 3).compare("MSG") == 0)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

void Server::doCtrl(std::string& msg, User* user)
{
  std::string ctrl = msg.substr(4, 1);
  // Invitation: MSG_BopponentName
  if(ctrl.compare("B") == 0)
    {
      std::string opponentName = msg.substr(5);
      User *opponent = searchUser(opponentName);
      *(opponent->getSocket()) << "Accept invitation? (y/n)";
      user->setOpponent(opponent);
      opponent->setOpponent(user);
    }
  // Accept invitation: MSG_C
  else if(ctrl.compare("C") == 0)
    {
      user->setPlaying(true);
      user->getOpponent()->setPlaying(true);
      *(user->getOpponent()->getSocket()) << "Connection created!";
      *(user->getSocket()) << "Connection created!";
    }
  // Decline invitation or cancel connection: MSG_D
  else if(ctrl.compare("D") == 0)
    {
      *(user->getOpponent()->getSocket()) << "Connection cancelled!";
      *(user->getSocket()) << "Connection cancelled!";
      user->setPlaying(false);
      user->getOpponent()->setPlaying(false);
      user->getOpponent()->setOpponent(NULL);
      user->setOpponent(NULL);
    }
}

void Server::sendMsgPlaying(std::string& msg, User* user)
{
    std::string type = msg.substr(4, 1);
    // Message type 1: TPE_1 -> message to opponent
    if(type.compare("1") == 0)
    {
        std::string send = getTime(1) + " " + user->getName() + ": " + msg.substr(5);
        *(user->getOpponent()->getSocket()) << send;
    }
    // Message type 2: TPE_2 -> next move's data to opponent
    else if(type.compare("2") == 0)
    {
        *(user->getOpponent()->getSocket()) << msg.substr(5);
    }
    // Message type 3: TPE_3 -> message to all users
    else if(type.compare("3") == 0)
    {
        std::string send = getTime(1) + " " + user->getName() + ": " + msg.substr(5);
        sendMsg(send);
    }
}

User* Server::searchUser(std::string& name)
{
    std::list<User*>::iterator i;
    pthread_rwlock_rdlock(&mLock);
    for ( i = mClients.begin(); i != mClients.end(); ++i)
    {
        if(name.compare((*i)->getName()) == 0 ) {
	    pthread_rwlock_unlock(&mLock);
            return (*i);
        }
    }
    pthread_rwlock_unlock(&mLock);
    return NULL;
}