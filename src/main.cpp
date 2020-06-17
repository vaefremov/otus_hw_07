#include <iostream>
#include <cstdlib>

#include "command_reader.h"
#include "writer.h"

static void usage()
{
    std::cout << "Usage: "
              << "bulk N" << std::endl
              << "  N: size of commands block (int > 0)" << std::endl;
    std::exit(EXIT_FAILURE);
}

static int cli_int_arg(int argc, const char** argv)
{
    if(argc < 2)
        usage();
    try
    {
        int block_sz = std::stoi(argv[1]);
        if(block_sz < 1)
            usage();
        return block_sz;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        usage();
    }
    return 0;
}

int main(int argc, const char** argv) {
    int block_sz = cli_int_arg(argc, argv);
    OTUS::CommandReader reader(std::cin);
    auto w1 = OTUS::OstreamWriter::create_subscriber("cout1", std::cout, block_sz, reader);
    auto wf = OTUS::FilesWriter::create_subscriber("file", block_sz, reader);
    reader.run();
    return EXIT_SUCCESS;
}