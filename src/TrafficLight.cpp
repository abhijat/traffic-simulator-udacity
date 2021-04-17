#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template
class MessageQueue<TrafficLightPhase>;

template<typename T>
T MessageQueue<T>::receive() {
    std::unique_lock<std::mutex> lck{_mutex};
    _condition.wait(lck, [this] { return !_queue.empty(); });
    auto t = std::move(_queue.front());
    _queue.pop_front();

    return t;
}

template<typename T>
void MessageQueue<T>::send(T &&msg) {
    std::lock_guard<std::mutex> lck{_mutex};
    _queue.push_back(msg);
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() {
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen() {
    while (true) {
        auto phase = _phases.receive();
        if (phase == TrafficLightPhase::green) {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase() {
    return _currentPhase;
}

void TrafficLight::simulate() {
    threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

[[noreturn]] void TrafficLight::cycleThroughPhases() {
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator{seed};
    std::uniform_int_distribution<int> distribution(4000, 6000);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        int cycleDuration = distribution(generator);

        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate > cycleDuration) {

            std::cout << "Phase switch\n";
            _currentPhase = _currentPhase == TrafficLightPhase::green ? TrafficLightPhase::red
                                                                      : TrafficLightPhase::green;
            _phases.send(std::move(_currentPhase));
            std::cout << "Phase switch to: " << _currentPhase << ", cycleDuration: " << cycleDuration
                      << "\n";
            lastUpdate = std::chrono::system_clock::now();
        }
    }
}

std::ostream &operator<<(std::ostream &os, const TrafficLightPhase &phase) {
    switch (phase) {
        case TrafficLightPhase::red:
            os << "RED";
            break;
        case TrafficLightPhase::green:
            os << "GREEN";
            break;
    }
    return os;
}