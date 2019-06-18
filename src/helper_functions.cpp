#include <helper_functions.h>

// keep track of node names (key: uuid, value: name)
std::map<std::string, std::string> uuid_to_name_map;

// zactor which polls for messages from the main thread 
// and events from other nodes
void receiveLoop(zsock_t *pipe, void *args)
{
    zyre_t * node = (zyre_t*)(args);

    zsock_signal(pipe, 0);
    bool terminated = false;
    // this poller will listen to messages that the node receives
    // AND messages received by this actor on pipe
    zpoller_t *poller = zpoller_new (pipe, zyre_socket(node), NULL);
    std::string node_to_print = "";
    std::string group_to_print = "";
    while (!terminated)
    {
        void *which = zpoller_wait (poller, -1); // no timeout
        if (which == pipe) // message sent to the actor
        {
            zmsg_t *msg = zmsg_recv (which);
            if (!msg)
                break;              //  Interrupted
            char *command = zmsg_popstr (msg);
            if (streq (command, "$TERM"))
            {
                terminated = true;
            }
            else if (streq(command, "PRINT SHOUTS FROM NODE"))
            {
                char * node_uuid = zmsg_popstr(msg);
                node_to_print = std::string(node_uuid);
                group_to_print = "";
                free(node_uuid);
            }
            else if (streq(command, "PRINT SHOUTS FROM GROUP"))
            {
                char * group_name = zmsg_popstr(msg);
                group_to_print = std::string(group_name);
                node_to_print = "";
                free(group_name);
            }
            else if (streq(command, "STOP PRINT"))
            {
                node_to_print = "";
                group_to_print = "";
            }
            else
            {
                std::cerr << "invalid message to actor" << std::endl;
                assert (false);
            }
            free (command);
            zmsg_destroy (&msg);
        }
        else if (which == zyre_socket(node)) // message sent to the node
        {
            zmsg_t *msg = zmsg_recv (which);
            char * event = zmsg_popstr(msg);
            char * peer = zmsg_popstr(msg);
            char * name = zmsg_popstr(msg);
            // NOTE: in the case of SHOUT, this is the group name
            // in the case of WHISPER, this is the message
            char * group_or_message = zmsg_popstr(msg);
            char * message = zmsg_popstr(msg);
            uuid_to_name_map[std::string(peer)] = std::string(name);
            if (streq(event, "SHOUT") &&
                    (node_to_print == std::string(peer) ||
                     group_to_print == std::string(group_or_message)))
            {
                    std::cout << "[shout:" << group_or_message <<"]["
                              << std::string(name) << "]" << std::endl
                              << message << std::endl;
                    std::cout << std::endl;
            }
            else if (streq(event, "WHISPER"))
            {
                std::cout << "[whisper][" << std::string(name) << "]" << std::endl
                          <<  group_or_message << std::endl;
            }
            free(event);
            free (peer);
            free (name);
            free (group_or_message);
            free (message);
            zmsg_destroy(&msg);
        }
    }
    zpoller_destroy (&poller);
}

// prints a list of nodes on the network (their UUID and name)
std::string getNodeList(zyre_t * node)
{
    zlist_t * peers = zyre_peers(node);
    std::stringstream output;
    int size = zlist_size(peers);
    for (int i = 0; i < size; i++)
    {
        char * peer_uuid = (char*)zlist_pop(peers);
        // look for name of peer based on uuid
        std::map<std::string, std::string>::iterator it = uuid_to_name_map.find(peer_uuid);
        output << peer_uuid;
        if (it != uuid_to_name_map.end())
        {
            output << " (" << it->second << ")";
        }
        output << std::endl;
        free(peer_uuid);
    }
    output << zyre_uuid(node) << " (" << zyre_name(node) << ") [this node]";
    zlist_destroy(&peers);
    return output.str();
}

// get list of groups that a node (identified by uuid) belongs to
std::vector<std::string> getNodeGroups(zyre_t *node, const std::string &uuid)
{
    std::vector<std::string> peer_groups;
    // get peers by group
    zlist_t * groups = zyre_peer_groups(node);
    if (!groups)
    {
        std::cout << "No groups exist" << std::endl;
        return peer_groups;
    }
    int size = zlist_size(groups);
    for (int i = 0; i < size; i++)
    {
        char * group_name = (char *)zlist_pop(groups);

        zlist_t * peers = zyre_peers_by_group(node, group_name);
        if (!peers)
        {
            free(group_name);
            continue;
        }
        int peer_size = zlist_size(peers);
        for (int j = 0; j < peer_size; j++)
        {
            char * peer_name = (char *)zlist_pop(peers);
            if (std::string(peer_name) == uuid)
            {
                peer_groups.push_back(std::string(group_name));
            }
            free(peer_name);
        }
        zlist_destroy(&peers);

        free(group_name);
    }
    zlist_destroy(&groups);
    return peer_groups;
}

// prints node uuid, name, endpoint and groups
std::string getNodeInfo(zyre_t * node, const std::string &uuid)
{
    std::stringstream output;
    std::map<std::string, std::string>::iterator it = uuid_to_name_map.find(uuid);
    if (it != uuid_to_name_map.end())
    {
        output << "UUID: " << it->first << std::endl;
        output << "Name: " << it->second << std::endl;
        char * endpoint = zyre_peer_address(node, uuid.c_str());

        if (strlen(endpoint) > 0)
        {
            output << "Endpoint: " << endpoint << std::endl;
        }
        else
        {
            output << "Endpoint: node does not exist" << std::endl;
        }
        free(endpoint);

        std::vector<std::string> peer_groups = getNodeGroups(node, uuid);
        if (peer_groups.empty())
        {
            output << "Groups: None" << std::endl;
        }
        else
        {
            output << "Groups: ";
            for (int i = 0; i < peer_groups.size(); i++)
            {
                output << peer_groups[i];
                if (i != peer_groups.size() - 1)
                {
                    output << ", ";
                }
            }
        }
    }
    else
    {
        std::cerr << "Peer " << uuid << " does not exist";
    }
    return output.str();
}

// stop listening to shouts from a node or group
void stopPrinting(zyre_t * node, zactor_t * actor)
{
    zlist_t * groups = zyre_own_groups(node);
    int size = zlist_size(groups);
    for (int i = 0; i < size; i++)
    {
        char * group_name = (char *)zlist_pop(groups);
        zyre_leave(node, group_name);
    }
    zlist_destroy(&groups);

    zstr_sendx(actor, "STOP PRINT", NULL);
}

// start listening to (and print) shouts from a certain node (to any group it belongs to)
void printNodeShouts(zyre_t * node, zactor_t * actor, const std::string &uuid)
{
    std::vector<std::string> peer_groups = getNodeGroups(node, uuid);
    for (int i = 0; i < peer_groups.size(); i++)
    {
        zyre_join(node, peer_groups[i].c_str());
    }
    zstr_sendx(actor, "PRINT SHOUTS FROM NODE", uuid.c_str(), NULL);
}

// print list of groups
std::string getGroupList(zyre_t * node)
{
    std::stringstream output;
    std::set<std::string> unique_groups;
    zlist_t * groups = zyre_peer_groups(node);

    int size = zlist_size(groups);
    for (int i = 0; i < size; i++)
    {
        char * group_name = (char *)zlist_pop(groups);
        output << group_name << std::endl;
        unique_groups.insert(std::string(group_name));
        free(group_name);
    }
    zlist_destroy(&groups);

    zlist_t * self_groups = zyre_own_groups(node);
    int num_groups = zlist_size(self_groups);
    for (int i = 0; i < num_groups; i++)
    {
        char * self_group = (char*)zlist_pop(self_groups);
        if (unique_groups.find(std::string(self_group)) == unique_groups.end())
        {
            unique_groups.insert(std::string(self_group));
            output << self_group << std::endl;
        }
    }
    zlist_destroy(&self_groups);
    return output.str();
}

// print peers of a group
std::string getGroupInfo(zyre_t * node, const std::string &name)
{
    std::stringstream output;
    zlist_t * peers = zyre_peers_by_group(node, name.c_str());

    zlist_t * groups = zyre_own_groups(node);
    int gsize = zlist_size(groups);
    int is_self_in_group = 0;
    for (int i = 0; i < gsize; i++)
    {
        char * group_name = (char *)zlist_pop(groups);
        if (std::string(group_name) == name)
        {
            is_self_in_group = 1;
        }
    }
    zlist_destroy(&groups);

    if (!peers && is_self_in_group == 0)
    {
        std::cerr << "No group named " << name << std::endl;
        return output.str();
    }

    int size = 0;
    if (peers)
        size = zlist_size(peers);
    int num_nodes = size + is_self_in_group;
    output << "Group " << name  << " has " << num_nodes << ((num_nodes == 1)?" node":" nodes") << std::endl;
    for (int i = 0; i < size; i++)
    {
        char * peer_name = (char *)zlist_pop(peers);
        output << "\t" << peer_name;
        // look for name of peer based on uuid
        std::map<std::string, std::string>::iterator it = uuid_to_name_map.find(peer_name);
        if (it != uuid_to_name_map.end())
        {
            output << " (" << it->second << ")";
        }
        output << std::endl;
        free(peer_name);
    }
    if (is_self_in_group == 1)
    {
        output << "\t" << zyre_uuid(node) << " [this node]" << std::endl;
    }
    zlist_destroy(&peers);
    return output.str();
}

// start listening to (and print) shouts to a certain group from any node
void printGroupShouts(zyre_t * node, zactor_t *actor, const std::string &name)
{
    zyre_join(node, name.c_str());
    zstr_sendx(actor, "PRINT SHOUTS FROM GROUP", name.c_str(), NULL);
}

