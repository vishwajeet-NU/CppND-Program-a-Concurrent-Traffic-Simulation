#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <future>
#include "TrafficObject.h"

enum TrafficLightPhase
{
    red,
    green
};

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
      void send(T &&msg);
      T receive();
      std::deque<TrafficLightPhase> _queue;
private:
      std::condition_variable _cond;
      std::mutex _mtex;
    
};


class TrafficLight: public TrafficObject
{
public:
    TrafficLight() : _currentPhase(TrafficLightPhase::red) {};
    void waitForGreen(); 
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    void cycleThroughPhases();
    

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _myq;
};

#endif