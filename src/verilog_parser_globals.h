#ifndef _VERILOG_PARSER_GLOBALS_H_
#define _VERILOG_PARSER_GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <new>
#include <pthread.h>
#include <boost/regex.hpp>
#include <vector>

using namespace std;
using namespace boost;

enum PortDirection
{
    INPUT,
    OUTPUT,
    INOUT
};

enum PortType
{
    NODE,
    TERMINAL
};

struct Port
{
    string name, connected_net, conected_instance_name;
    PortType port_type;
    PortType connected_net_type;
    PortDirection port_direction;
};

struct InstanceInfo
{
    string definition;
    string name;

    vector<Port> ports;
};

extern void removeStrings(string &line, string s);
extern void removeEachChar(string &line, string s);
extern void strToLower(string &str);
extern bool getTerm(string &statement, string &term, char delim);

#include "verilogparser.h"

#endif

