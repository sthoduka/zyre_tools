#include <helper_functions.h>

zyre_t *node;
zactor_t *actor;

void printHelp()
{
    /* TODO */
    std::cout << "help" << std::endl;
}

void cleanup_and_close()
{
    zstr_sendx(actor, "$TERM", NULL);
    zactor_destroy(&actor);

    zyre_stop(node);
    // wait for node to stop
    zclock_sleep(100);
    zyre_destroy(&node);
    std::cout << std::endl;
    exit(0);
}
void signal_handler(int signal)
{
    cleanup_and_close();
}


int main(int argc, char *argv[])
{
    std::string valid_commands[] = {"list"};
    bool print_uuid = false;
    if (argc < 2 || 
       std::find(std::begin(valid_commands), std::end(valid_commands), argv[1]) == std::end(valid_commands) ||
       streq(argv[1], "-h") || streq(argv[1], "--help"))
    {
        printHelp();
        return 1;
    }

    std::time_t t = std::time(0);
    std::string node_name = "zyrenode_"+std::to_string(t);

    node = zyre_new(node_name.c_str());
    if (!node)
    {
        return 1;
    }
    zyre_start(node);
    zclock_sleep(250);
    /* std::cout << node << std::endl; */
    /* zyre_print(node); */
    actor = zactor_new(receiveLoop, node);

    std::signal(SIGINT, signal_handler);


    std::string command = argv[1];
    if (streq(argv[1], "list"))
    {
        std::cout << getNodeList(node);
    }

    cleanup_and_close();
    return 0;
}
