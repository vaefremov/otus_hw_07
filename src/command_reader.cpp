#include <string>
#include <iostream>
#include "command_reader.h"

static OTUS::Event cmd2event(std::string&& cmd)
{
    switch (cmd[0])
    {
    case '{':
        return OTUS::Event{OTUS::EventType::BLOCK_START, std::string()};
    case '}':
        return OTUS::Event{OTUS::EventType::BLOCK_END, std::string()};
    default:
         return OTUS::Event{OTUS::EventType::COMMAND, cmd};
    }
}

void OTUS::CommandReader::run()
{
    std::string cmd;
    while(std::getline(m_src, cmd))
    {
        if(cmd.size() > 0)
        {
         notify(cmd2event(std::move(cmd)));
        }
    }
    Event ev{EventType::STREAM_END, std::string()};
    notify(ev);
}