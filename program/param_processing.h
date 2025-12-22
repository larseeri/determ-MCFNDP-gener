// Copyright (c) 2025 Eric Larsen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ****************************************************************************
// ****************************************************************************
//
//
#ifndef _STRUCPARAM_H_
#define _STRUCPARAM_H_

#include <cstring>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <sstream>
#include <string>

using namespace std;

namespace PARAM
{

// default value of boolean variables
#define DEFAULT_BOOL true

// prefix of all parameters, except configuration file related
#define PARAM_PREFIX '-'

// prefix of parameters related to configuration file and to reading
// of configuration file
#define CONFIG_PREFIX '+'

// prefix of non-default configuration file name
#define CONFIG_FILE_KEY "+F"

// requesting verbose mode when looking for configuration file
#define CONFIG_VERBOSE_MODE "+v"

// Class ParamProcessing handles processing of parameter values supplied
// in configuration file and/or on instruction line.
//
// Parameter values are successively updated from these locations in this
// order:
// (i) hard-coded initialization values in lines 28-41 of data.cpp
// (ii) values appearing in configuration file whose name is specified in
// instruction line with +F (if it is specified and if it exists); if no valid
// configuration file is specified with +F in instruction line, values will be
// taken from configuration file whose name is specified in line 712 of
// main.cpp; at this point, the latter should exist, although it may be empty
// (iii) values specified in the instruction line
// Location (i) is insufficiently transparent to the user and should not be
// relied upon. Parameter values should be set through (ii) and (iii).
//
// Remarks:
// Option names appearing in instruction line must be prefixed with '-' as
// follows:
// -option_name option_value
// except for the following:
// +F when specifying a configuration file
// +v when requesting verbose mode while parsing options.
// - When options are called from the instruction line, dash prefixes '-'
// must be present. The latter can be omitted when the options are called from
// a configuration file.
// - To supply an option, a line of a configuration file must begin with a dash
// '-' or a letter. A line beginning with any other character will be
// considered a comment.
// - The remainder of a line beyond the stated option and its value (if any is
// expected) is disregarded and may therefore be used for adding comments.
// - Unknown option keys following '-' in the instruction line are disregarded.
// - Unknown option keys (following '-' or not) at beginning of lines in a
// configuration file are disregarded.
// - Whenever the name of a boolean parameter is not followed by a boolean
// value, it will be assigned the default value DEFAULT_BOOL.

class ParamProcessing
{
  public:
    // Constructor
    ParamProcessing(int argc, char **argv, string fileName = "");
    // 1st param.: number of values specified on instruction line
    // 2nd param.: pointer to instruction line
    // 3rd param.: name of configuration file
    // (This name can be overridden through the instruction line by
    // introducing +F followed by the new file name.)

    template <class T> bool assignParamValue(const string &cmdString, const string &configFileString, T &var) const;

    bool assignParamValue(const string &cmdString, const string &configFileString, bool &var) const;

    bool assignParamValue(const string &cmdString, const string &configFileString, char *var) const;

    bool assignParamValue(const string &cmdString, const string &configFileString, string &var) const;

    // assignParamValue determines the value to be assigned to a parameter.
    // It will return false if the parameter value is available neither from
    // instruction line nor from a configuration file.
    //
    // cmdString is the parameter name expected when parsing the instruction
    // line. configFileString is the parameter name expected when parsing the
    // configuration file. var is the variable to which the parameter value
    // will be assigned. This value will be taken from the instruction line
    // unless it is unavailable thereof, in which case it will be taken from
    // the configuration file. If the value is absent from both, then a default
    // value should have been supplied when calling the constructor of the
    // ParamProcessing class in Data::Data.
    //
    // Fictional example:
    // ParamProcessing paramProcessing
    // paramProcessing.assignParamValue("ord", "evalOrd", evalOrder);
    // paramProcessing.assignParamValue("freq", "printFrequ", printFrequency);
    //
    // if the configuration file contains:
    //
    //  evalOrd AFTER
    //  printFrequ 1000
    //
    // and the instruction line contains:
    //
    // prog_exec -ord BEFORE
    //
    // then the variables evalOrder and printFrequency will be assigned values
    // as follows:
    //
    // evalOrder = BEFORE, printFrequency = 1000.
    //

  private:
    // mapping from name of parameter to its value, according to instruction
    // line
    map<string, string> paramToValueMapFromCmdLine;

    // mapping from name of parameter to its value, according to configuration
    // file
    map<string, string> paramToValueMapFromConfigFile;

    // number of arguments in instruction line
    int nbArg;

    // array of char array containing the instruction line
    char **vectArg;

    // name of configuration file
    string configFileName;

    // flags display of parameter value when parsing
    bool verbose;

    // These methods used in parsing configuration parameters:

    void parseCmdLine();

    void parseConfigFileNameFromCmdLine();

    bool parseConfigFile(const string &nameInFile);

    string parseConfigLine(istringstream &ligneFichier);
};

const int lineLength = 256;

inline ParamProcessing::ParamProcessing(int argc, char **argv, string fileName)
    : nbArg(argc), vectArg(argv), verbose(false)
{

    // configuration file name as passed to constructor
    configFileName = fileName;

    // attempt to find configuration file name in instruction line
    parseConfigFileNameFromCmdLine();

    // parse the configuration file
    bool found = parseConfigFile(configFileName);

    if (!found)
    {
        cerr << "\nWARNING: The configuration file is empty.\n"
				"If an optional configuration file name was specified in instruction\n"
				"line with option key +F, this file was empty.\n"
				"Otherwise, the default configuration file was empty.\n" 
                "Consequently, configuration will proceed entirely from instruction line\n"
				"instructions and from hard-coded initialization values.\n"
             << endl;
    }

    // parsing the options in instruction line
    parseCmdLine();
}

// Parses the configuration file name specified on instruction line
inline void ParamProcessing::parseConfigFileNameFromCmdLine()
{
    int a;
    bool foundFile = false;

    for (a = 1; a < nbArg; a++)
    {
        if (vectArg[a][0] == CONFIG_PREFIX)
        {
            if (strcmp(vectArg[a], CONFIG_FILE_KEY) == 0)
            {
                // checks if configuration file name is specified on cmd line
                if (a < nbArg - 1)
                {
                    configFileName = vectArg[a + 1];

                    if (!filesystem::is_regular_file(configFileName))
                    {
                        cerr << "\nERROR: " << endl;
                        cerr << "The configuration file " << configFileName
                             << "\nspecified in instruction line could not be found." << endl;
                        cerr << "EXECUTION ABORTED\n" << endl;
                        exit(1);
                    }

                    foundFile = true;
                }
            }
            else if (strcmp(vectArg[a], CONFIG_VERBOSE_MODE) == 0)
            {
                verbose = true;
            }
            //
            else if (isalpha(vectArg[a][1]))
                cerr << "********* config prefix [" << vectArg[a] << "] unknown.\n";
        }
    }

    if (verbose)
    {
        cout << "\n********* VERBOSE MODE *********\n\n";

        if (foundFile)
            cout << "****** config file name [" << CONFIG_FILE_KEY << "] = [" << configFileName << "]\n";
    }
}

// Parses the configuration parameter values supplied on instruction line.
inline void ParamProcessing::parseCmdLine()
{
    int a = 1;
    const char *arg1;
    const char *arg2;

    if (verbose)
        cout << "****** Parsing configuration parameters from the instruction line.\n\n";

    while (a < nbArg)
    {
        if ((vectArg[a][0] == PARAM_PREFIX))
        {
            arg1 = &vectArg[a][1];

            if (a < nbArg - 1)
            {
                arg2 = vectArg[a + 1];

                if (((vectArg[a + 1][0] == PARAM_PREFIX) || (vectArg[a + 1][0] == CONFIG_PREFIX)) &&
                    isalpha(vectArg[a + 1][1]))
                    arg2 = "";
                else
                    a++;
            }
            else
                arg2 = "";

            if (verbose)
                cout << "****** instruction line -- parameter [" << arg1 << "] = [" << arg2 << "]\n";
            paramToValueMapFromCmdLine[arg1] = arg2;
        }

        a++;
    }

    if (verbose)
        cout << endl;
}

// Parses configuration parameter values supplied in configuration file.
inline bool ParamProcessing::parseConfigFile(const string &infileName)
{
    char line[lineLength];
    ifstream inStr;

    // checking if configuration file exists
    if (!filesystem::is_regular_file(infileName.c_str()))
    {
        cerr << "\nERROR:" << endl;
        cerr << "No configuration file was specified in instruction line and\n";
        cerr << "the default configuration file " << infileName.c_str() << "\nspecified in main.cpp could not be found."
             << endl;
        cerr << "EXECUTION ABORTED\n" << endl;
        exit(1);
    }

    // attempting to open configuration file
    inStr.open(infileName.c_str());

    if (!inStr.is_open())
    {
        cerr << "\nERROR:" << endl;
        cerr << "The configuration file " << infileName.c_str() << " could not be opened." << endl;
        cerr << "EXECUTION ABORTED\n" << endl;
        exit(1);
    }

    bool found = 0;

    if (verbose)
        cout << "****** Parsing this configuration file: " << infileName << "\n\n";

    inStr.getline(line, lineLength);
    while (inStr)
    {
        istringstream configLine(line);

        string key;
        configLine >> key;

        string value = parseConfigLine(configLine);

        int pos = (key[0] == PARAM_PREFIX) ? 1 : 0;

        // ensures that it is not a comment
        if (isalpha(key[pos]))

        {
            found = 1;
            if (verbose)
                cout << "****** configuration file -- parameter [" << key.substr(pos) << "] = [" << value << "]\n";

            // stores in map
            paramToValueMapFromConfigFile[key.substr(pos)] = value;
        }

        inStr.getline(line, lineLength);
    }

    if (verbose)
        cout << endl;

    return found;
}

// Parses a line of the configuration file.
inline string ParamProcessing::parseConfigLine(istringstream &configLine)
{
    string inString(""), outString("");

    configLine >> inString;

    if (!inString.empty() && (inString.substr(0, 1) == "\""))
    {
        size_t pos = inString.substr(1, lineLength).find('"');
        outString = inString.substr(1, pos);
    }
    else
    {
        outString = inString;
    }

    return outString;
}

// Determines value to be assigned to a parameter.
template <class T>
inline bool ParamProcessing::assignParamValue(const string &cmdString, const string &configFileString, T &var) const
{
    bool ok = true;

    // attempt to retrieve from data collected from instruction line
    map<string, string>::const_iterator itL = paramToValueMapFromCmdLine.find(cmdString);

    if (itL != paramToValueMapFromCmdLine.end())
    {
        istringstream istr((*itL).second);
        istr >> var;
    }
    else
    {
        // if unavailable, attempt to retrieve from data collected from configuration file(s)
        map<string, string>::const_iterator itF = paramToValueMapFromConfigFile.find(configFileString);

        if (itF != paramToValueMapFromConfigFile.end())
        {
            istringstream istr((*itF).second);
            istr >> var;
        }
        else
            ok = false;
    }

    return ok;
}

// Determines value to be assigned to a parameter. Specialized to string.
inline bool ParamProcessing::assignParamValue(const string &cmdString, const string &configFileString,
                                              string &var) const
{
    bool ok = true;

    // attempt to retrieve from data collected from instruction line
    map<string, string>::const_iterator itL = paramToValueMapFromCmdLine.find(cmdString);

    if (itL != paramToValueMapFromCmdLine.end())
        var = (*itL).second;
    else
    {
        // if unavailable, attempt to retrieve from data collected from configuration file(s)
        map<string, string>::const_iterator itF = paramToValueMapFromConfigFile.find(configFileString);

        if (itF != paramToValueMapFromConfigFile.end())
            var = (*itF).second;
        else
            ok = false;
    }

    return ok;
}

// Determines value to be assigned to a parameter. Specialized to char*.
inline bool ParamProcessing::assignParamValue(const string &cmdString, const string &configFileString, char *var) const
{
    bool ok = true;

    // attempt to retrieve from data collected from instruction line
    map<string, string>::const_iterator itL = paramToValueMapFromCmdLine.find(cmdString);

    if (itL != paramToValueMapFromCmdLine.end())
        strcpy(var, (*itL).second.c_str());
    else
    {
        // if unavailable, attempt to retrieve from data collected from configuration file(s)
        map<string, string>::const_iterator itF = paramToValueMapFromConfigFile.find(configFileString);

        if (itF != paramToValueMapFromConfigFile.end())
            strcpy(var, (*itF).second.c_str());
        else
            ok = false;
    }

    return ok;
}

// Determines value to be assigned to a parameter. Specialized to bool.
inline bool ParamProcessing::assignParamValue(const string &cmdString, const string &configFileString, bool &var) const
{
    bool ok = true;

    // attempt to retrieve from data collected from instruction line
    map<string, string>::const_iterator itL = paramToValueMapFromCmdLine.find(cmdString);

    if (itL != paramToValueMapFromCmdLine.end())
    {
        if ((*itL).second == "")
            var = DEFAULT_BOOL;
        else
        {
            istringstream istr((*itL).second);
            istr >> var;
        }
    }
    else
    {
        // if unavailable, attempt to retrieve from data collected from configuration file(s)
        map<string, string>::const_iterator itF = paramToValueMapFromConfigFile.find(configFileString);

        if (itF != paramToValueMapFromConfigFile.end())
        {
            if ((*itF).second == "")
                var = DEFAULT_BOOL;
            else
            {
                istringstream istr((*itF).second);
                istr >> var;
            }
        }
        else
            ok = false;
    }

    return ok;
}

} // namespace PARAM

#endif
