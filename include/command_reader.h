#pragma once
#include <istream>
#include "iobserver.h"

namespace OTUS
{

class CommandReader: public OTUS::BaseObservable
{
    public:
    CommandReader(std::istream& src): m_src(src) {}
    CommandReader() = delete;
    CommandReader(CommandReader const&) = delete;
    CommandReader& operator=(CommandReader const&) = delete;
    void run();
    private:
    std::istream& m_src;
};

}