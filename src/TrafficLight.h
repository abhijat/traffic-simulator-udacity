#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

class Vehicle;


template<class T>
class MessageQueue {
public:
    void send(T &&t);

    T receive();

private:
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;
};


enum class TrafficLightPhase {
    red,
    green
};

std::ostream &operator<<(std::ostream &os, const TrafficLightPhase &phase);

class TrafficLight : public TrafficObject {
public:
    TrafficLight();

    void waitForGreen();

    void simulate() override;

    TrafficLightPhase getCurrentPhase();

private:
    [[noreturn]] void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _phases;
    TrafficLightPhase _currentPhase;
};

#endif