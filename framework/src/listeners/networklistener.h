#ifndef NETWORK_LISTENER_H
#define NETWORK_LISTENER_H


class NetworkListener {
    public:
        NetworkListener() = default;
        virtual ~NetworkListener() = default;
        bool isListening() { return listening;}
        virtual void init() = 0;
        virtual void startListening() = 0;
    protected:
        bool listening = false;
};

#endif //#define NETWORK_LISTENER_H