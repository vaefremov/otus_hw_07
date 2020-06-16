#include <gtest/gtest.h>
#include <sstream>

#include "writer.h"

TEST(writer, state_change)
{
    std::stringstream ss;
    size_t block_sz = 2;
    OTUS::OstreamWriter w1("w1", ss, block_sz);
    OTUS::Event ev_c{OTUS::EventType::COMMAND, "c1"};
    auto res = w1.update_state(OTUS::EventType::COMMAND);
    ASSERT_FALSE(res);
}

TEST(writer, update)
{
    std::stringstream ss;
    size_t block_sz = 2;
    OTUS::OstreamWriter w1("w1", ss, block_sz);
    OTUS::Event ev_c{OTUS::EventType::COMMAND, "c1"};
    w1.update(ev_c);
    w1.update(ev_c);
    auto res = ss.str();
    std::string exp_res{"bulk: c1 c1\n"};
    ASSERT_STREQ(exp_res.c_str(), res.c_str());
}

TEST(writer, update_endstream)
{
    std::stringstream ss;
    size_t block_sz = 2;
    OTUS::OstreamWriter w1("w1", ss, block_sz);
    OTUS::Event ev_c{OTUS::EventType::COMMAND, "c1"};
    OTUS::Event ev_e{OTUS::EventType::STREAM_END, ""};
    w1.update(ev_c);
    w1.update(ev_e);
    auto res = ss.str();
    std::string exp_res{"bulk: c1\n"};
    ASSERT_STREQ(exp_res.c_str(), res.c_str());
}

TEST(writer, update_startblock)
{
    std::stringstream ss;
    size_t block_sz = 2;
    OTUS::OstreamWriter w1("w1", ss, block_sz);
    OTUS::Event ev_c{OTUS::EventType::COMMAND, "c1"};
    OTUS::Event ev_e{OTUS::EventType::BLOCK_START, ""};
    w1.update(ev_c);
    w1.update(ev_e);
    auto res = ss.str();
    std::string exp_res{"bulk: c1\n"};
    ASSERT_STREQ(exp_res.c_str(), res.c_str());
}

TEST(writer, update_endstream_in_block)
{
    std::stringstream ss;
    size_t block_sz = 2;
    OTUS::OstreamWriter w1("w1", ss, block_sz);
    OTUS::Event ev_c{OTUS::EventType::COMMAND, "c1"};
    OTUS::Event ev_bs{OTUS::EventType::BLOCK_START, ""};
    OTUS::Event ev_e{OTUS::EventType::STREAM_END, ""};
    w1.update(ev_bs);
    w1.update(ev_c);
    w1.update(ev_e);
    auto res = ss.str();
    std::string exp_res{""};
    ASSERT_STREQ(exp_res.c_str(), res.c_str());
}

TEST(writer, update_endblock)
{
    std::stringstream ss;
    size_t block_sz = 2;
    OTUS::OstreamWriter w1("w1", ss, block_sz);
    OTUS::Event ev_c{OTUS::EventType::COMMAND, "c1"};
    OTUS::Event ev_bs{OTUS::EventType::BLOCK_START, ""};
    OTUS::Event ev_be{OTUS::EventType::BLOCK_END, ""};
    w1.update(ev_bs);
    w1.update(ev_c);
    w1.update(ev_c);
    w1.update(ev_c);
    w1.update(ev_be);
    auto res = ss.str();
    std::string exp_res{"bulk: c1 c1 c1\n"};
    ASSERT_STREQ(exp_res.c_str(), res.c_str());
}

TEST(writer, update_block_concat)
{
    std::stringstream ss;
    size_t block_sz = 2;
    OTUS::OstreamWriter w1("w1", ss, block_sz);
    OTUS::Event ev_c{OTUS::EventType::COMMAND, "c1"};
    OTUS::Event ev_bs{OTUS::EventType::BLOCK_START, ""};
    OTUS::Event ev_be{OTUS::EventType::BLOCK_END, ""};
    w1.update(ev_bs);
    w1.update(ev_c);
    w1.update(ev_bs);
    w1.update(ev_c);
    w1.update(ev_c);
    w1.update(ev_be);
    w1.update(ev_be);
    auto res = ss.str();
    std::string exp_res{"bulk: c1 c1 c1\n"};
    ASSERT_STREQ(exp_res.c_str(), res.c_str());
}

