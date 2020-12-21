#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include "TrafficLight.h"

template <class T> void MessageQueue<T>::send(T &&msg)
{
    std::unique_lock<std::mutex> lock1(_mtex);
    _queue.emplace_back(msg);
    _cond.notify_one();
}

template <class T> T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mtex);
    _cond.wait(lock,[this] () {return !_queue.empty(); });
    T out = std::move(_queue.back());
    _queue.pop_back();
    return out;

}


TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::waitForGreen()
{
    while(true)
    {
        TrafficLightPhase color = _myq.receive();
        if(color == TrafficLightPhase::green)
        {
            return;
        }
    }
}


void TrafficLight::simulate()
{
    threads.emplace_back( std::thread(&TrafficLight::cycleThroughPhases,this) ) ;
}
 
void TrafficLight::cycleThroughPhases()
{

    auto time_start = std::chrono::system_clock::now();
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_int_distribution<> distr(4,6);

        int cycleDuration = distr(eng);
       long duration_passed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()-time_start).count();
        while(duration_passed < cycleDuration )
        {
            duration_passed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()-time_start).count();
        }
        //do stuff ;
        switch (_currentPhase)
        {
        case TrafficLightPhase::red:
            _currentPhase = TrafficLightPhase::green;
            break;
        case TrafficLightPhase::green:  
            _currentPhase = TrafficLightPhase::red;
            break;
        }
        _myq.send(std::move(_currentPhase));
        time_start = std::chrono::system_clock::now();

        
        
    }
}

