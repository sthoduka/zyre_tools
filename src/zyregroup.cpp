#include <helper_functions.h>
#include <fstream>

zyre_t *node;
zactor_t *actor;

/* Colors */
const std::string red("\033[0;31m");
const std::string plain("\033[0m");

void printHelp()
{
    const char *help_string = 
"\n"
"usage: zyregroup COMMAND [args]\n"
"\n"
"Command line tool for printing information about zyre groups\n"
"\n"
"Commands:\n"
"  list                 list all the zyre groups in the network\n" 
"  info GROUP           provide info on a zyre groups in the network\n" 
"  echo GROUP           print all the SHOUT messages from a zyre group\n" 
"  pub GROUP MESSAGE    shout message to a zyre group\n" 
"  pub GROUP -f FILE    shout contents of file to a zyre group\n" 
"\n"
"optional arguments:\n"
"  -h, --help    show this help message and exit\n"
"";
    std::cout << help_string;
}

void cleanup_and_close()
{
    zstr_sendx(actor, "$TERM", NULL);
    zactor_destroy(&actor);

    zyre_stop(node);
    // wait for node to stop
    zclock_sleep(100);
    zyre_destroy(&node);
    exit(0);
}

void signal_handler(int signal)
{
    cleanup_and_close();
    std::cout << std::endl;
}


int main(int argc, char *argv[])
{
    std::string valid_commands[] = {"list", "info", "echo", "pub"};
    bool print_uuid = false;
    if (argc < 2 || 
       std::find(std::begin(valid_commands), std::end(valid_commands), argv[1]) == std::end(valid_commands) ||
       streq(argv[1], "-h") || streq(argv[1], "--help"))
    {
        printHelp();
        return 1;
    }

    std::time_t t = std::time(0);
    std::string node_name = "zyregroup_"+std::to_string(t);

    node = zyre_new(node_name.c_str());
    if (!node)
    {
        return 1;
    }
    zyre_start(node);
    zclock_sleep(250);
    actor = zactor_new(receiveLoop, node);

    std::signal(SIGINT, signal_handler);

    std::string command = argv[1];
    if (streq(argv[1], "list"))
    {
        std::cout << getGroupList(node);
    }
    else if (streq(argv[1], "info"))
    {
        if (argc < 3)
        {
            std::cerr << red << "Incorrect usage" << plain << std::endl;
            printHelp();
        }
        else
        {
            std::string uuid = argv[2];
            std::cout << getGroupInfo(node, uuid);
        }
    }
    else if (streq(argv[1], "echo"))
    {
        if (argc < 3)
        {
            std::cerr << red << "Incorrect usage" << plain << std::endl;
            printHelp();
        }
        else
        {
            std::string uuid = argv[2];
            printGroupShouts(node, actor, uuid);
            while (true)
            {
                zclock_sleep(250);
            }
        }
    }
    else if (streq(argv[1], "pub"))
    {
        if (argc < 4)
        {
            std::cerr << red << "Incorrect usage" << plain << std::endl;
            printHelp();
        }
        else
        {
            std::string group_name = argv[2];
            std::stringstream message;
            if (streq(argv[3], "-f"))
            {
                if (argc < 5 || argc > 5)
                {
                    std::cerr << red << "Incorrect usage" << plain << std::endl;
                    printHelp();
                }
                else
                {
                    std::ifstream file;
                    file.open(std::string(argv[4]));
                    if (!file)
                    {
                        std::cerr << red << "Could not read file: " << plain << argv[4] << std::endl;
                    }
                    else
                    {
                        message << file.rdbuf();
                        zyre_shouts(node, group_name.c_str(), "%s", message.str().c_str());
                    }
                    file.close();
                }
            }
            else
            {
                for (int i = 3; i < argc; i++)
                {
                    message << argv[i] << " ";
                }
                zyre_shouts(node, group_name.c_str(), "%s", message.str().c_str());
            }
        }
    }

    cleanup_and_close();
    return 0;
}
