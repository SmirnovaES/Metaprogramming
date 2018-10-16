#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class UserInterfaceA;
class UserInterfaceB;

class Proxy;
class Mediator;
class Observer;

template<class PatternType>
class PatternBuilder {
private:
    PatternType *pattern;
public:
    PatternBuilder(UserInterfaceA *userA, std::vector<UserInterfaceB *> usersB);

    void accept_request(const std::string &message) {
        if (std::is_same<PatternType, Proxy>::value || std::is_same<PatternType, Mediator>::value) {
            pattern->accept_request(message);
        }
    }

    void subscribe(UserInterfaceB *userB) {
        if (std::is_same<PatternType, Observer>::value) {
            pattern->subscribe(userB);
        }
    }

    void notify(UserInterfaceB *user, std::string message) {
        if (std::is_same<PatternType, Observer>::value) {
            pattern->notify(user, message);
        }
    }
};

class UserInterfaceA {
private:
    PatternBuilder<Proxy> *proxy;
    PatternBuilder<Mediator> *mediator;
    PatternBuilder<Observer> *observer;
public:

    void set_pattern(PatternBuilder<Proxy> *proxy) {
        this->proxy = proxy;
    }
    void set_pattern(PatternBuilder<Mediator> *mediator) {
        this->mediator = mediator;
    }
    void set_pattern(PatternBuilder<Observer> *observer) {
        this->observer = observer;
    }

    void send_proxy_request() {
        std::cout << "I'm A and I'm going to ask proxy for help\n";
        proxy->accept_request("My name is A and I want to say hello to B, please help\n");
    }
    void send_mediator_request() {
        std::cout << "I'm A and I'm going to ask mediator for help\n";
        mediator->accept_request("A says hello to B\n");
    }

    void subscribe(UserInterfaceB *userB) {
        std::cout << "A subscribed to some B\n";
        observer->subscribe(userB);
    }

    void get_request(const std::string &message) {
        std::cout << "I'm A and I recieved message from my subscribtions:\n" << message << std::endl;
    }

};

class UserInterfaceB {
private:
    PatternBuilder<Observer> *observer;
public:

    void set_pattern(PatternBuilder<Proxy> *proxy) {}
    void set_pattern(PatternBuilder<Mediator> *mediator) {}
    void set_pattern(PatternBuilder<Observer> *observer) {
        this->observer = observer;
    }

    void get_request(const std::string &message) {
        std::cout << "I'm B and I recieved this message from A, I guess:\n" << message << std::endl;
    }

    void update() {
        observer->notify(this, "I'm B and I have updated\n");
    }
};

class Proxy {
private:
    UserInterfaceB *clientB;
public:
    Proxy(UserInterfaceA *userA, std::vector<UserInterfaceB *> &usersB) {
        if (usersB.empty()) {
            throw std::invalid_argument("no B users\n");
        }
        clientB = usersB[0];
    }
    void accept_request(const std::string &message) {
        std::cout << "\n-----PROXY TIME-----\n";
        std::cout << "I'm proxy and I've recieved this message:\n" << message;
        std::cout << "I'm going to send it to B\n" << "-----END PROXY TIME-----\n\n";
        clientB->get_request("Hello from A\n");
    }

};

class Mediator {
private:
    std::vector<UserInterfaceB *> clientB;
public:
    Mediator(UserInterfaceA *userA, std::vector<UserInterfaceB *> &usersB) {
        if (usersB.empty()) {
            throw std::invalid_argument("no B users\n");
        }
        clientB = usersB;
    }
    void accept_request(const std::string &message) {
        unsigned long b_user_index = std::rand() % clientB.size();
        std::cout << "\n-----MEDIATOR TIME-----\n";
        std::cout << "I'm mediator and I've recieved this message:\n" << message;
        std::cout << "I'm going to send it to B[" << b_user_index << "]\n-----END MEDIATOR TIME-----\n\n";
        clientB[b_user_index]->get_request("Hello from A\n");
    }
};

class Observer {
private:
    UserInterfaceA *clientA;
    std::vector<UserInterfaceB *> clientsB;
public:
    Observer(UserInterfaceA *userA, std::vector<UserInterfaceB *> &usersB) {
        clientA = userA;
    }

    void subscribe(UserInterfaceB *userB) {
        clientsB.push_back(userB);
    }

    void notify(UserInterfaceB *user, const std::string &message) {
        std::cout << "\n-----OBSERVER TIME-----\n";
        if (std::find(clientsB.begin(), clientsB.end(), user) != clientsB.end()) {
            std::cout << "Client B is found, sending message to A\n";
            std::cout << "-----END OBSERVER TIME-----\n";
            clientA->get_request(message);
        }
    }
};

template <class PatternType>
PatternBuilder<PatternType>::PatternBuilder(UserInterfaceA *userA, std::vector<UserInterfaceB *> usersB)  {
    pattern = new PatternType(userA, usersB);
    userA->set_pattern(this);
    for (auto user : usersB) {
        user->set_pattern(this);
    }
}


int main() {
    auto * userA = new UserInterfaceA();
    auto * userB = new UserInterfaceB();
    std::vector<UserInterfaceB *> usersB{userB};

    PatternBuilder<Proxy> *proxy = new PatternBuilder<Proxy>(userA, usersB);
    PatternBuilder<Mediator> *mediator = new PatternBuilder<Mediator>(userA, usersB);
    PatternBuilder<Observer> *observer = new PatternBuilder<Observer>(userA, usersB);

    std::cout << "---------Testing Proxy---------\n\n";

    userA->send_proxy_request();

    std::cout << "\n\n\n---------Testing Mediator---------\n\n";

    userA->send_mediator_request();

    std::cout << "\n\n\n---------Testing Observer---------\n\n";

    userA->subscribe(userB);
    userB->update();

    delete(userA);
    delete(userB);
    delete(proxy);
    delete(mediator);
    delete(observer);

    return 0;
}