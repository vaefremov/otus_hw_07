#pragma once

#include <list>
#include <memory>

namespace OTUS
{

enum class EventType
{
    COMMAND, BLOCK_START, BLOCK_END, STREAM_END
};

constexpr char const* event_type_name(EventType t)
{
    switch (t)
    {
    case EventType::COMMAND:
        return "COMMAND";
    case EventType::BLOCK_START:
        return "Start of block";
    case EventType::BLOCK_END:
        return "End of block";
    case EventType::STREAM_END:
        return "End of stream";
    default:
        return "Unknown event type!";
    }
}

struct Event
{
    EventType m_type;
    std::string m_payload;
};

class IObserver 
{
    public:
    virtual ~IObserver() = default;
    virtual void update(Event const&) = 0;

};

class IObservable 
{
    public:
    virtual ~IObservable() = default;
    virtual void subscribe(std::shared_ptr<IObserver> const&) = 0;
};


class BaseObservable : public IObservable {
public:
    void subscribe(const std::shared_ptr<IObserver>& obs) override {
        m_subscribers.emplace_back(obs);
    }

    void notify(Event const& ev) {
        auto iter = std::begin(m_subscribers);
        while(iter != std::end(m_subscribers)) {
            auto ptr = iter->lock();
            if (ptr) {
                ptr->update(ev);
                ++iter;
            } else {
                // The current implementation explicitly uses the std::list behaviour regarding 
                // the influence of the erase method on iterators (see e.g. https://en.cppreference.com/w/cpp/container/list/erase)
                iter = m_subscribers.erase(iter);
            }
        }
    }
private:
    std::list<std::weak_ptr<IObserver>> m_subscribers;
};

} // namespace OTUS