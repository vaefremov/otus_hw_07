#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <memory>

#include "command_reader.h"
#include "iobserver.h"

class TestExecutor: public OTUS::IObserver
{
    public:
    void update(OTUS::Event const& ev)
    {
        m_events.push_back(ev);
    }
    std::vector<OTUS::Event> m_events;
};

TEST(reader, input_command)
{
    std::stringstream ss("c1\n");
    auto r = OTUS::CommandReader(ss);
    auto ex1_ptr = std::make_shared<TestExecutor>();
    r.subscribe(ex1_ptr);
    r.run();
    ASSERT_EQ(2, ex1_ptr->m_events.size());
    ASSERT_EQ(OTUS::EventType::COMMAND, ex1_ptr->m_events[0].m_type);
    ASSERT_STREQ("c1", ex1_ptr->m_events[0].m_payload.c_str());
    ASSERT_EQ(OTUS::EventType::STREAM_END, ex1_ptr->m_events[1].m_type);
}

TEST(reader, input_blocks)
{
    std::stringstream ss("{\n}\n");
    auto r = OTUS::CommandReader(ss);
    auto ex1_ptr = std::make_shared<TestExecutor>();
    r.subscribe(ex1_ptr);
    r.run();
    ASSERT_EQ(3, ex1_ptr->m_events.size());
    ASSERT_EQ(OTUS::EventType::BLOCK_START, ex1_ptr->m_events[0].m_type);
    ASSERT_EQ(OTUS::EventType::BLOCK_END, ex1_ptr->m_events[1].m_type);
    ASSERT_EQ(OTUS::EventType::STREAM_END, ex1_ptr->m_events[2].m_type);
}