#include <zyre.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <set>
#include <ctime>
#include <csignal>

std::vector<std::string> getNodeUuids(zyre_t * node);

std::map<std::string, std::string> getNamesFromUuids(zyre_t * node, 
                                                     std::vector<std::string> uuids);
void receiveLoop(zsock_t *pipe, void *args);
std::string getNodeList(zyre_t * node);
std::vector<std::string> getNodeGroups(zyre_t *node, const std::string &uuid);
std::string getNodeInfo(zyre_t * node, const std::string &uuid);
void stopPrinting(zyre_t * node, zactor_t * actor);
void printNodeShouts(zyre_t * node, zactor_t * actor, const std::string &uuid);
std::string getGroupList(zyre_t * node);
std::string getGroupInfo(zyre_t * node, const std::string &name);
void printGroupShouts(zyre_t * node, zactor_t *actor, const std::string &name);
