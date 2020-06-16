#include <gtest/gtest.h>
#include <sstream>
#include <fstream>

#include "writer.h"

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

TEST(file_writer, filenames)
{
    const size_t block_sz = 2;
    std::string exp{"bulk1.log"};
    OTUS::FilesWriter w1("w1", block_sz, [](){return 1LL;});
    OTUS::Event ev_c{OTUS::EventType::COMMAND, "c1"};
    w1.update(ev_c);
    auto res = w1.make_log_file_name();
    ASSERT_STREQ(exp.c_str(), res.c_str());
}

TEST(file_writer, commands)
{
    size_t block_sz = 2;
    OTUS::FilesWriter w1("w1", block_sz, [](){return 1LL;});
    OTUS::Event ev_c{OTUS::EventType::COMMAND, "c1"};
    w1.update(ev_c);
    w1.update(ev_c);
    std::string exp_res{"bulk: c1 c1\n"};
    std::string filename{"bulk1.log"};
    std::cout << "Reading from " << filename << std::endl;
    std::ifstream t(filename);
    std::string res((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    ASSERT_STREQ(exp_res.c_str(), res.c_str());
}
