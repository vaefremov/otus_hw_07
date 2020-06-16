#pragma once
#include <string>
#include <ostream>
#include <memory>
#include <queue>
#include <iostream>
#include <exception>
#include <chrono>

#include "iobserver.h"
#include "command_reader.h"

namespace OTUS
{

class OstreamWriter: public IObserver 
{
    public:
    static std::shared_ptr<OstreamWriter> create(std::string const& name, std::ostream& out, size_t block_sz, OTUS::CommandReader& reader)
    {
        auto ptr = std::make_shared<OstreamWriter>(name, out, block_sz);
        reader.subscribe(ptr);
        return ptr;
    }
    OstreamWriter() = delete;
    OstreamWriter(OstreamWriter const&) = delete;
    OstreamWriter& operator=(OstreamWriter const&) = delete;
    OstreamWriter(std::string const& name, std::ostream& out, size_t block_sz): m_name(name), m_out(out), m_block_sz(block_sz) {}

    void update(Event const& ev)
    {
        std::cerr << m_name << event_type_name(ev.m_type) << ev.m_payload << std::endl;
        if(ev.m_type == EventType::COMMAND)
        {
            if(!m_block_start_tm)
            {
                m_block_start_tm = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            }
            m_commands.emplace(ev.m_payload);
        }
        if(update_state(ev.m_type))
        {
            execute();
            m_block_start_tm = 0;
        }
    }

    bool update_state(EventType t)
    {
        switch (t)
        {
        case EventType::BLOCK_START:
            return !(m_nesting_level++);
        case EventType::BLOCK_END:
            if(m_nesting_level == 0) // Error in input stream!
            {
                throw std::runtime_error("Unexpected end of block");
            }
            return !(--m_nesting_level);
        case EventType::STREAM_END:
            return m_nesting_level == 0;
        case EventType::COMMAND:
            return (m_nesting_level == 0) && (m_commands.size() >= m_block_sz);
        default:
            throw std::runtime_error("Unexpected event type");
        }
        return false;
    }

    void execute()
    {
        if(m_commands.empty())
        {
            return;
        }
        m_out << "bulk:";
        while(!m_commands.empty())
        {
            m_out << " " << m_commands.front();
            m_commands.pop();
        }
        m_out << std::endl;
    }
    
    std::string make_log_file_name()
    {
        return "bulk" + std::to_string(m_block_start_tm) + ".log";
    }

    private:
    
    std::string m_name;
    std::ostream& m_out;
    size_t m_block_sz;
    size_t m_nesting_level = 0;
    std::queue<std::string> m_commands;
    long long m_block_start_tm;
};

}