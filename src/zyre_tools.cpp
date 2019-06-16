/**
 * Copyright (c) Santosh Thoduka, 2018
 * Licensed under GPLv3
 */

#include <helper_functions.h>
#include <readline/readline.h>
#include <readline/history.h>

zyre_t *node;
zactor_t *actor;

// print prompt and get input
// tokenize and return as a vector of strings
std::vector<std::string> getCommand()
{
    std::vector<std::string> sub_commands;
    char * input_buffer = readline("$ ");
    if (input_buffer != NULL)
    {
        if (input_buffer[0] != 0)
        {
            add_history(input_buffer);

            std::string command(input_buffer);
            std::istringstream iss(command);
            sub_commands = std::vector<std::string>{std::istream_iterator<std::string>{iss},
                                                    std::istream_iterator<std::string>{}};

        }
    }
    free(input_buffer);
    return sub_commands;
}

// print list of commands available
void help()
{
    std::cout << "Available commands: " << std::endl;
    std::cout << "\tnl                           : list all peers" << std::endl;
    std::cout << "\tni <uuid>                    : get info on a peer" << std::endl;
    std::cout << "\tnecho <uuid>                 : echo shouts and whispers by a peer" << std::endl;
    std::cout << "\tgl                           : list all groups" << std::endl;
    std::cout << "\tgi <group name>              : get info on a group" << std::endl;
    std::cout << "\tgecho <group name>           : echo shouts on a group" << std::endl;
    std::cout << "\tjoin <group name>            : join a group" << std::endl;
    std::cout << "\tleave <group name>           : leave a group" << std::endl;
    std::cout << "\tstop                         : stop printing whispers and shouts" << std::endl;
    std::cout << "\ts <group name> <message>     : shout a message to a group" << std::endl;
    std::cout << "\tw <uuid> <message>           : whisper a message to a peer" << std::endl;
    std::cout << "\tq                            : exits" << std::endl;
    std::cout << "\thelp" << std::endl;
}

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
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
    std::string node_name;
    if (argc > 1)
    {
        node_name = std::string(argv[1]);
    }
    else
    {
        node_name = "zyre_tools";
    }
    node = zyre_new(node_name.c_str());
    if (!node)
    {
        return 1;
    }

    zyre_start(node);
    zclock_sleep(250);

    actor = zactor_new(receiveLoop, node);

    std::signal(SIGINT, signal_handler);

    while (true && !zsys_interrupted)
    {
        std::vector<std::string> cmd = getCommand();
        if (cmd.empty())
        {
            continue;
        }
        else if (cmd[0] == "exit" || cmd[0] == "quit" || cmd[0] == "q")
        {
            break;
        }
        if (cmd[0] == "help")
        {
            help();
        }
        else if (cmd[0] == "stop")
        {
            stopPrinting(node, actor);
        }
        else if (cmd[0] == "nl")
        {
            std::string output = getNodeList(node);
            std::cout << "\t" << replaceAll(output, std::string("\n"), std::string("\n\t")) << std::endl;
        }
        else if (cmd[0] == "ni")
        {
            if (cmd.size() < 2)
            {
                std::cerr << "Specify a node uuid: ni <node uuid>" << std::endl;
                continue;
            }
            std::string uuid = cmd[1];
            printNodeInfo(node, uuid);
        }
        else if (cmd[0] == "necho")
        {
            if (cmd.size() < 2)
            {
                std::cerr << "Specify a node uuid: necho <node uuid>" << std::endl;
                continue;
            }
            std::string uuid = cmd[1];
            printNodeShouts(node, actor, uuid);
        }
        else if (cmd[0] == "gl")
        {
            printGroupList(node);
        }
        else if (cmd[0] == "gi")
        {
            if (cmd.size() < 2)
            {
                std::cerr << "Specify a group name: gi <group name>" << std::endl;
                continue;
            }
            std::string name = cmd[1];
            printGroupInfo(node, name);
        }
        else if (cmd[0] == "gecho")
        {
            if (cmd.size() < 2)
            {
                std::cerr << "Specify a group name: gecho <group name>" << std::endl;
                continue;
            }
            std::string name = cmd[1];
            printGroupShouts(node, actor, name);
        }
        else if (cmd[0] == "join")
        {
            if (cmd.size() < 2)
            {
                std::cerr << "Specify a group name: join <group name>" << std::endl;
                continue;
            }
            std::string group_name = cmd[1];
            zyre_join(node, group_name.c_str());
        }
        else if (cmd[0] == "leave")
        {
            if (cmd.size() < 2)
            {
                std::cerr << "Specify a group name: leave <group name>" << std::endl;
                continue;
            }
            std::string group_name = cmd[1];
            zyre_leave(node, group_name.c_str());
        }
        else if (cmd[0] == "s")
        {
            if (cmd.size() < 3)
            {
                std::cerr << "Specify a group and message: s <group name> <message>" << std::endl;
                continue;
            }
            else
            {
                std::string group = cmd[1];
                std::string message = cmd[2];
                for (int i = 3; i < cmd.size(); i++)
                {
                    message = message + " " + cmd[i];
                }
                zyre_shouts(node, group.c_str(), "%s", message.c_str());
            }
        }
        else if (cmd[0] == "w")
        {
            if (cmd.size() < 3)
            {
                std::cerr << "Specify a node uuid and message: w <node uuid> <message>" << std::endl;
                continue;
            }
            else
            {
                std::string peer = cmd[1];
                std::string message = cmd[2];
                for (int i = 3; i < cmd.size(); i++)
                {
                    message = message + " " + cmd[i];
                }
                zyre_whispers(node, peer.c_str(), "%s", message.c_str());
            }

        }
        else
        {
            std::cerr << "Unknown command. Type 'help' for list of commands" << std::endl;
        }
    }

    cleanup_and_close();
    return 0;
}
