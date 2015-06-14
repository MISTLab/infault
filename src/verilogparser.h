//
// source of this file: http://blackpundit.com/2005/03/04/c-gate-level-verilog-parser/
//
// has been enhanced to support new file formats


#ifndef _VERILOGPARSER_H_
#define _VERILOGPARSER_H_

#include "verilog_parser_globals.h"

// Port list
const int PORT_LIST_SIZE = 15;
const char PORT_LIST[] = 
{
    'z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n'
};


class VerilogParser
{
    public:
        ///////////////
        // Functions //
        ///////////////

    // Parse verilog File
        bool Parse(string verilog_file);

    // Is Terminal Input
        bool IsTerminalInput(string module_name, string net_name);

    // Is Terminal Output
        bool IsTerminalOutput(string module_name, string net_name);

    // Get all fo the existing fan in instances from a net
        bool GetAllFanOutInstancesFromNet(string module_name, string net_name, 
                                          vector<InstanceInfo> &inst_info);

    // Get all fo the existing fan out instances from a net
        bool GetAllFanInInstancesFromNet(string module_name, string net_name, 
                                         vector<InstanceInfo> &inst_info);
      
    // Get all of the existing fan ins from a net
        bool GetAllFanInsFromNet(string module_name, string net_name, 
                                 vector<Port> &ports);

    // Get all of the existing fan outs from a net
        bool GetAllFanOutsFromNet(string module_name, string net_name, 
                                  vector<Port> &ports);    

    // Get all of the verilog modules
        bool GetAllModules(vector<string> &mod); 

    // Get all of the ports for this module
        bool GetAllTerminalPorts(string module_name, vector<Port> &ports); 

    // Get all of the input ports for this module
        bool GetAllInputTerminalPorts(string module_name, vector<Port> &ports); 

    // Get all of the output ports for this module
        bool GetAllOutputTerminalPorts(string module_name, vector<Port> &ports); 

    // Get all instance
        bool GetAllInstances(string module_name, vector<InstanceInfo> &instances);

    // Get all connected nets from instances
        bool GetAllConnectedNetsFromInstance(string module_name, string inst_name, 
                                             vector<string> &nets);

    // Get all connected instances from net
        bool GetAllConnectedInstancesFromNet(string module_name, string net_name, 
                                             vector<InstanceInfo> &instances);

    // Returns and, or, xor, ect...
        string GetInstanceDefinition(string module_name, string inst_name); 

    // Get all connected nodes from node
        bool GetAllConnectedNodesFromNet(string net_name, 
                                         vector<Port> &nodes); 


    private:
        ///////////////
        // Functions //
        ///////////////

    // Get port and net name from term
        bool GetPortAndNet(string term, string &port_name, string &net_name);

    // Is terminal port
        bool IsTerminalPort(string port_name);

    // Parse module line
        bool ParseModuleLine(string line);
    
    // Parse input or output line
        bool ParseInputOutputLine(string line);
    
    // Parse assign line
        bool ParseAssignLine(string line);

    // Pares primitive line
        bool ParsePrimitiveLine(string line);

    // Is this primitive supported
        bool IsPrimSupported(string prim);

    // Get complete line
        bool GetCompleteLine(ifstream &v, string &line);

    // Remove any comment from line
        void RemoveAllComments(string file_name);

        /////////////////////
        // Data Structures //
        /////////////////////

    // This current module
        string current_module;

    // List of top level modules
        vector<string> modules;
    
    // List of terminal ports from module
        map<string, vector<Port> > module_terminal_ports;

    // List of instance names
        map<string, vector<InstanceInfo> > module_instances;

    // Keep track of repeating instances
        map<string, int> instances_check;
    
    // List of connected instances on net
    // connected_instances_on_net[module_name][net_name][instance_name]
        map<string, map<string, map<string, InstanceInfo> > >
                connected_instances_on_net;

    // instance index for assign statments
                int assign_instance_index;
};


#endif

