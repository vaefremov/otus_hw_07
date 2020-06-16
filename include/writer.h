#pragma once
#include <string>
#include <ostream>
#include <memory>
#include <queue>
#include <iostream>
#include <exception>
#include <chrono>
#include <fstream>
#include <functional>

#include "iobserver.h"
#include "command_reader.h"

namespace OTUS
{

class AbstractExecutor: public IObserver
{
    public:
    
    using TimingFn = std::function<long long()>;

    AbstractExecutor() = delete;
    AbstractExecutor(AbstractExecutor const&) = delete;
    AbstractExecutor& operator=(AbstractExecutor const&) = delete;
    virtual ~AbstractExecutor() = default;
    AbstractExecutor(std::string const& name, size_t block_sz): m_name{name}, m_block_sz{block_sz},
    m_timing{[](){return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();}} {}
    /**
     * This constructor was added added in order to have possibility to use a custom timer function that generates
     * timestamps. These timestamps may be used to generate file names in the descending classes.
     */
    AbstractExecutor(std::string const& name, size_t block_sz, TimingFn tm): m_name{name}, m_block_sz{block_sz}, m_timing{std::move(tm)}{}
    virtual void update(Event const& ev) override
    {
        if(ev.m_type == EventType::COMMAND)
        {
            if(!m_block_start_tm)
            {
                m_block_start_tm = m_timing();
            }
            m_commands.emplace(ev.m_payload);
        }
        if(update_state(ev.m_type))
        {
            execute();
            m_block_start_tm = 0;
        }
    }
    protected:
    virtual void execute() = 0;
    virtual bool update_state(EventType t)
    {
        switch (t)
        {
        case EventType::BLOCK_START:
            return !(m_nesting_level++);
        case EventType::BLOCK_END:
            if(m_nesting_level == 0) 
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

    std::string m_name;
    size_t m_block_sz = 1;
    std::queue<std::string> m_commands;
    size_t m_nesting_level = 0;
    long long m_block_start_tm = 0;
    TimingFn m_timing;
};

class OstreamWriter: public AbstractExecutor 
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
    OstreamWriter(std::string const& name, std::ostream& out, size_t block_sz): AbstractExecutor(name, block_sz), m_out(out) {}


    std::string make_log_file_name()
    {
        return "bulk" + std::to_string(m_block_start_tm) + ".log";
    }

    private:

    void execute() override
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
    
    
    std::ostream& m_out;
};

class FilesWriter: public AbstractExecutor
{
    public:
    static std::shared_ptr<FilesWriter> create(std::string const& name, size_t block_sz, OTUS::CommandReader& reader)
    {
        auto ptr = std::make_shared<FilesWriter>(name, block_sz);
        reader.subscribe(ptr);
        return ptr;
    }
    FilesWriter() = delete;
    FilesWriter(FilesWriter const&) = delete;
    FilesWriter& operator=(FilesWriter const&) = delete;
    FilesWriter(std::string const& name, size_t block_sz): AbstractExecutor(name, block_sz) {}
    /**
     * This constructor is used primarilly for testing purposes in order to obtain reproducible
     * log output file names.
     */
    FilesWriter(std::string const& name, size_t block_sz, AbstractExecutor::TimingFn tm): AbstractExecutor(name, block_sz, tm) {}


    void execute() override
    {
        m_out.open(make_log_file_name());
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
        m_out.close();
        std::cerr << "Log written to " << make_log_file_name() << std::endl;
    }
    
    std::string make_log_file_name() const
    {
        return "bulk" + std::to_string(m_block_start_tm) + ".log";
    }
    private:
    std::ofstream m_out;
};


}