#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <regex>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include<iomanip>

using namespace std;

static int lineNumber = 0;
static size_t positionNumber = 1;
static int baseAddressForUpcomingModule = 0;
static int noOfPairsInModule = -1;
static int noOfSymbolsUsedInModule = -1;
static int noOfInstructionsInModule = -1;
static map<int, int> moduleToBaseAddress;
static map<int, pair<string, bool> > mapForSymbolPositionInList;
static map<string, pair<int, bool> > globalHistOfSymbolDefAndUsage;
// don't forget to set it to 0 again once eof is reached
static int memoryMapCounter = 0;
static int memoryMapPrintingCounter = 0;
// need to increase this after all instructions are done
static int moduleNumber = 0;
static vector<pair<string, pair<int, string> > > symbolTable;
static vector<pair<int, pair<int, string> > > memoryMap;
static int generalCounter = 0;
static int pass = 1;
static bool symbolTablePrinted = false;
static size_t tokenSize = 0;

// token variables
const char delimit[] = " \t\n";
static char *token = NULL;
// stream variables
ifstream myfile;
static string inputFileName;
static string line;
static char *cstr;
string invalidToken = "-1";

enum ExpectedTokenType
{
    undefined,
    defcount,
    valueOfSymbol,
    usecount,
    codecount,
    symbol,
    addressingMode,
    operandAndOpcode
};

ExpectedTokenType nextExpectedTokenType = undefined;
set<string> allowedInstructionTypes;
vector<string> WarningList;
vector<string> printingList;

//Function declarations
char *GetNextToken();
int ReadSymbolValuePairListOfModule(char *token);
int ReadSymbolListOfModule(char *token);
int ReadInstructionList(char *token);
void LoadInputFile();
void ResetAllStateVariablesForNextPass();
bool IsTokenValid(char *token);
vector<pair<int, pair<int, string> > >::iterator FindInMemoryMap(int memoryCounter);
vector<pair<string, pair<int, string> > >::iterator FindInSymbolTable(string symbol);
void AddOrUpdateInMemoryMap(int memoryCounter, int value, string errStr);
void AddOrUpdateInSymbolTable(string symbol, int value, string errStr);

int main(int argc, char* argv[])
{

    inputFileName = argv[1];
    
    allowedInstructionTypes.insert("M");
    allowedInstructionTypes.insert("A");
    allowedInstructionTypes.insert("R");
    allowedInstructionTypes.insert("I");
    allowedInstructionTypes.insert("E");

    nextExpectedTokenType = defcount;

    for (int pass = 1; pass <= 2; pass++)
    {
        LoadInputFile();
        if (myfile.is_open())
        {
            char *token = GetNextToken();

            while (token != NULL)
            {

                if (noOfPairsInModule == 0 && noOfSymbolsUsedInModule != 0)
                {

                    if(ReadSymbolListOfModule(token) == 0){
                        return 0;
                    }
                }
                else
                {
                    if (noOfSymbolsUsedInModule == 0 && noOfInstructionsInModule != 0)
                    {

                        generalCounter = 0;
                        if(ReadInstructionList(token) == 0){
                            return 0;
                        }
                    }
                    else
                    {

                        if(ReadSymbolValuePairListOfModule(token) == 0){
                            return 0;
                        }
                    }
                }

                //This makrks the end of a module
                if (noOfPairsInModule == 0 && noOfSymbolsUsedInModule == 0 && noOfInstructionsInModule == 0)
                {
                    //warning from rule 5
                    if (pass == 1)
                    {
                        vector<pair<string, pair<int, string> > >::iterator symbolMapIt = symbolTable.begin();
                        while (symbolMapIt != symbolTable.end())
                        {
                            pair<int, string> valueOfSymbolAndErr = symbolMapIt->second;
                            string symbolInEntry = symbolMapIt->first;
                            if (globalHistOfSymbolDefAndUsage.find(symbolInEntry) != globalHistOfSymbolDefAndUsage.end())
                            {

                                if (globalHistOfSymbolDefAndUsage[symbolInEntry].first == moduleNumber)
                                {
                                    if (valueOfSymbolAndErr.first != -1)
                                    {
                                        // check if first def is ok
                                        int symbolAddressAbsolute = valueOfSymbolAndErr.first;
                                        int baseAddressForThisModule = moduleToBaseAddress[moduleNumber];
                                        int instructionsInCurrentModule = baseAddressForUpcomingModule - baseAddressForThisModule;
                                        if ((symbolAddressAbsolute - baseAddressForThisModule) >= instructionsInCurrentModule)
                                        {
                                            cout << "Warning: Module " << moduleNumber+1 << ": " << symbolInEntry << " too big " << (symbolAddressAbsolute - baseAddressForThisModule) << " (max=" << instructionsInCurrentModule - 1 << ") assume zero relative\n";
                                            // Making relative zero means baseAddr + 0
                                            pair<int, string> replaceIncorrectPair(baseAddressForThisModule, valueOfSymbolAndErr.second);
                                            symbolMapIt->second = replaceIncorrectPair;
                                        }
                                    }
                                }
                            }

                            symbolMapIt++;
                        }

                        // print all warning here (after module processing is over):
                        for (int i = 0; i < WarningList.size(); i++)
                        {
                            cout << WarningList[i]<<endl;
                        }

                        WarningList.clear();
                    }

                    // warning from rule 7
                    if (pass == 2)
                    {
                        map<int, pair<string, bool> >::iterator mapForSymbolPositionInListIt = mapForSymbolPositionInList.begin();
                        
                        mapForSymbolPositionInListIt = mapForSymbolPositionInList.begin();
                        while (mapForSymbolPositionInListIt != mapForSymbolPositionInList.end())
                        {
                            pair<string, bool> symbolAndFlag = mapForSymbolPositionInListIt->second;

                            //For rule 4
                            //Update use flags after each module in pass 2, 
                            //no need to dupliacte same thing for pass1 because we have to print it after pass2 anyway
                            if (globalHistOfSymbolDefAndUsage.find(symbolAndFlag.first) != globalHistOfSymbolDefAndUsage.end())
                            {
                                (globalHistOfSymbolDefAndUsage.find(symbolAndFlag.first)->second).second = symbolAndFlag.second;
                            }

                            if (symbolAndFlag.second == false)
                            {
                                string err_msg = "Warning: Module " + to_string(moduleNumber+1) + ": uselist[" + to_string(mapForSymbolPositionInListIt->first) + "]=" + symbolAndFlag.first + " was not used \n";
                                WarningList.push_back(err_msg);
                            }

                            mapForSymbolPositionInListIt++;
                        }
                    }

                    if (pass == 2 && symbolTablePrinted == false)
                    {
                        cout << "Symbol Table \n";

                        for (vector<pair<string, pair<int, string> > >::iterator anotherItr = symbolTable.begin(); anotherItr != symbolTable.end(); anotherItr++)
                        {
                            if ((anotherItr->second).first != -1)
                            {
                                cout << anotherItr->first << "=" << (anotherItr->second).first << " " << (anotherItr->second).second << endl;
                            }
                        }

                        cout << "\nMemory Map \n";

                        symbolTablePrinted = true;
                    }

                    if (pass == 2)
                    {
                        for (vector<pair<int, pair<int, string> > >::iterator itr = memoryMap.begin(); itr != memoryMap.end(); itr++)
                        {
                            if (itr->first >= memoryMapPrintingCounter && itr->first < memoryMapCounter)
                            {
                                cout << setw(3) << setfill('0') << itr->first << ": " << setw(4) << setfill('0') << (itr->second).first << " " << (itr->second).second << endl;
                            }
                        }

                        memoryMapPrintingCounter = memoryMapCounter;

                        // print all warning here (after module processing is over):
                        for (int i = 0; i < WarningList.size(); i++)
                        {
                            cout << WarningList[i]<<endl;
                        }

                        WarningList.clear();
                    }

                    moduleNumber++;
                    // Reset all for the next module
                    mapForSymbolPositionInList.clear();
                    noOfPairsInModule = -1;
                    noOfSymbolsUsedInModule = -1;
                    noOfInstructionsInModule = -1;
                }

                token = GetNextToken();
            }

            if (pass == 2)
            {

                map<string, pair<int, bool> >::iterator globalHistOfSymbolDefAndUsageIt = globalHistOfSymbolDefAndUsage.begin();

                while (globalHistOfSymbolDefAndUsageIt != globalHistOfSymbolDefAndUsage.end())
                {
                    if ((globalHistOfSymbolDefAndUsageIt->second).second == false)
                    {
                        string err_msg = "Warning: Module " + to_string(((globalHistOfSymbolDefAndUsageIt->second).first) + 1) + ": " + globalHistOfSymbolDefAndUsageIt->first + " was defined but never used\n";
                        WarningList.push_back(err_msg);
                    }

                    globalHistOfSymbolDefAndUsageIt++;
                }

                // print all warning here (after module processing is over):
                for (int i = 0; i < WarningList.size(); i++)
                {
                    cout << WarningList[i]<<endl;
                }

                WarningList.clear();
            }

            //Parse errors will happen in Pass 1 and exit. 

            //if we haven't finished pass 1 and we unexpectedly reach the end of file we will have a null token so we need to 
            //throw a parse error before we reset our static variables like line, position etc and exit the process

            //but what if it is a correct ending?? then also these conditions will be true but that time we'll be expecting a defcount as the modules have ended properly.
            if(token == NULL && pass == 1 && nextExpectedTokenType != defcount){
                char *invalidCstr = new char[invalidToken.length() + 1];
                strcpy(invalidCstr, invalidToken.c_str());
                positionNumber = positionNumber + tokenSize;
                if(IsTokenValid(invalidCstr) == false){
                    return 0;
                }
            }

            // Once we're out here go in for the next pass: Maybe craete another function to reset everything for the next pass
            ResetAllStateVariablesForNextPass();

            // For empty file case
            if(pass==2 && symbolTablePrinted==false){

                cout << "Symbol Table \n";

                        for (vector<pair<string, pair<int, string> > >::iterator anotherItr = symbolTable.begin(); anotherItr != symbolTable.end(); anotherItr++)
                        {
                            if ((anotherItr->second).first != -1)
                            {
                                cout << anotherItr->first << "=" << (anotherItr->second).first << " " << (anotherItr->second).second << endl;
                            }
                        }

                        cout << "\nMemory Map \n";

                        for (vector<pair<int, pair<int, string> > >::iterator itr = memoryMap.begin(); itr != memoryMap.end(); itr++)
                        {
                            if (itr->first >= memoryMapPrintingCounter && itr->first < memoryMapCounter)
                            {
                                cout << setw(3) << setfill('0') << itr->first << ": " << setw(4) << setfill('0') << (itr->second).first << " " << (itr->second).second << endl;
                            }
                        }

                        memoryMapPrintingCounter = memoryMapCounter;

                        // print all warning here (after module processing is over):
                        for (int i = 0; i < WarningList.size(); i++)
                        {
                            cout << WarningList[i]<<endl;
                        }

                        WarningList.clear();
                        symbolTablePrinted = true;
            }
        }

        else
            cout << "Unable to open input file";
    }

    return 0;
}


void ResetAllStateVariablesForNextPass()
{

    // don't forget to set it to 0 again once eof is reached
    memoryMapCounter = 0;
    // need to increase this after all instructions are done
    moduleNumber = 0;
    baseAddressForUpcomingModule = 0;
    generalCounter = 0;
    mapForSymbolPositionInList.clear();
    moduleToBaseAddress.clear();
    lineNumber = 0;
    positionNumber = 1;
    pass = 2;
    tokenSize = 0;
}

void LoadInputFile()
{

    if (myfile.is_open())
    {

        myfile.close();
    }

    myfile.clear();
    myfile.open(inputFileName);
}

char *GetNextToken()
{
    token = strtok(NULL, delimit);
    
    if (token == NULL)
    {
        if (myfile.is_open())
        {
            if (myfile.peek() != EOF )
            {
                if (getline(myfile, line))
                {
                    //at the beginning always set positionNo to 1 and tokenSize to zero
                    positionNumber = 1;
                    tokenSize = 0;

                    cstr = new char[line.length() + 1];
                    strcpy(cstr, line.c_str());
                    token = strtok(cstr, delimit);
                    lineNumber++;
                    if (token == NULL)
                    {
                        // This line is empty: move to the next
                        return GetNextToken();
                    }
                    tokenSize = strlen(token);
                    positionNumber = (token - cstr) + 1;
                    return token;
                }
            }

            else{

            }
        }
        

        return NULL;
    }
    else
    {
        positionNumber = (token - cstr) +1;
        tokenSize = strlen(token);
        return token;
    }
};

bool IsTokenValid(char *token)
{

    if (token == NULL)
        return false;

    switch (nextExpectedTokenType)
    {
    case defcount:
        if (std::regex_match(token, std::regex("[0-9]*")))
        {
            if (stoi(token) > 16)
            {
                cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
                     << "TOO_MANY_DEF_IN_MODULE\n";
                return false;
            }
            return true;
        }
        else
        {
            cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
                 << "NUM_EXPECTED\n";

            return false;
        }

        break;
    case usecount:
        if (std::regex_match(token, std::regex("[0-9]*")))
        {
            if (stoi(token) > 16)
            {
                cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
                     << "TOO_MANY_USE_IN_MODULE\n";

                return false;
            }
            return true;
        }
        else
        {
            cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
                 << "NUM_EXPECTED\n";

            return false;
        }

        break;
    case codecount:
        if (std::regex_match(token, std::regex("[0-9]*")))
        {
            if (stoi(token) + baseAddressForUpcomingModule > 512)
            {
                cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
                     << "TOO_MANY_INSTR\n";

                return false;
            }
            return true;
        }
        else
        {
            cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
                 << "NUM_EXPECTED\n";

            return false;
        }

        break;
    case valueOfSymbol:
        
        if (std::regex_match(token, std::regex("[0-9]*")) && stoi(token) < 1073741824)
        {
            return true;
        }
        else
        {
            cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
                 << "NUM_EXPECTED\n";

            return false;
        }

        break;
    case symbol:
        if (std::regex_match(token, std::regex("[a-zA-Z][a-zA-Z0-9]*")))
        {
            if (strlen(token) > 16)
            {
                cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
                     << "SYM_TOO_LONG\n";

                return false;
            }

            return true;
        }
        else
        {
            cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
                 << "SYM_EXPECTED\n";

            return false;
        }
        break;
    case addressingMode:
        if (allowedInstructionTypes.find(token) != allowedInstructionTypes.end())
        {
            return true;
        }
        else
        {
            cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
                 << "MARIE_EXPECTED\n";

            return false;
        }
        break;
    case operandAndOpcode:
    {
        int opcode = (atoi(token) / 1000);
        if (opcode >= 10)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    case undefined:

        cout << "Parse Error line " << lineNumber << " offset " << positionNumber << ": "
             << "Unexpected token\n";

        break;
    };

    return false;
};

void ComputeAbsoluteAddressForModuleInstr(int operandAndOpcode, int moduleNumberForInstr, string err_str = "")
{
    // will be computed in pass2.
    // So for now we will keep this as -1 (in whole program we are assuming -1 is a state we need to replace)

    int operand = operandAndOpcode % 1000;
    int opcode = operandAndOpcode / 1000;

    // Taking precedent
    if (operandAndOpcode == 9999 && err_str.compare("Error: Illegal opcode; treated as 9999") == 0)
    {
        AddOrUpdateInMemoryMap(memoryMapCounter, operandAndOpcode, err_str);
    }

    else
    {
        if (moduleToBaseAddress.find(operand) == moduleToBaseAddress.end())
        {
            if (pass == 2)
            {
                // still didn't find the module so replace with 0: rule 12
                AddOrUpdateInMemoryMap(memoryMapCounter, (opcode * 1000), "Error: Illegal module operand ; treated as module=0");
    
            }
            else
            {
                // actually doesn't matter if i put -1 or the original operand+opcode stuff but i'm gonna keep it -1
                // just so we're clear that something needs to be updated here later
                AddOrUpdateInMemoryMap(memoryMapCounter, -1, err_str);
            }
        }
        else
        {

            int replacingOperand = moduleToBaseAddress[operand];
            AddOrUpdateInMemoryMap(memoryMapCounter, (opcode * 1000) + replacingOperand, err_str);

            
        }
    }
};

void ComputeAbsoluteAddressForAbsoluteInstr(int operandAndOpcode, string err_str = "")
{
    int operand = operandAndOpcode % 1000;
    int opcode = operandAndOpcode / 1000;

    // Taking precedent
    if (operandAndOpcode == 9999 && err_str.compare("Error: Illegal opcode; treated as 9999") == 0)
    {
        AddOrUpdateInMemoryMap(memoryMapCounter, operandAndOpcode, err_str);
    }
    else
    {
        if (operand >= 512)
        {
            AddOrUpdateInMemoryMap(memoryMapCounter, (opcode * 1000), "Error: Absolute address exceeds machine size; zero used");
        }
        else
        {
            AddOrUpdateInMemoryMap(memoryMapCounter, operandAndOpcode, err_str);

        }
    }
};

void ComputeAbsoluteAddressForRelativeInstr(int operandAndOpcode, string err_str = "")
{
    int relativeAddr = operandAndOpcode % 1000;
    int opCode = operandAndOpcode / 1000;

    // Taking precedent
    if (operandAndOpcode == 9999 && err_str.compare("Error: Illegal opcode; treated as 9999") == 0)
    {
        AddOrUpdateInMemoryMap(memoryMapCounter, operandAndOpcode, err_str);
    }
    else
    {
        if (moduleToBaseAddress.find(moduleNumber) != moduleToBaseAddress.end())
        {
            int baseAddrForCurrModule = moduleToBaseAddress[moduleNumber];
            if (relativeAddr > baseAddressForUpcomingModule - baseAddrForCurrModule)
            {
                string err_msg = "Error: Relative address exceeds module size; relative zero used";
                int newAbsoluteAddr = (opCode * 1000) + (baseAddrForCurrModule);
                AddOrUpdateInMemoryMap(memoryMapCounter, newAbsoluteAddr, err_msg);

            }

            else
            {
                AddOrUpdateInMemoryMap(memoryMapCounter, (opCode * 1000) + (baseAddrForCurrModule + relativeAddr), err_str);
            }
        }

        else
        {
            AddOrUpdateInMemoryMap(memoryMapCounter, -1, err_str);

        }
    }
};

void ComputeAbsoluteAddressForImmediateInstr(int operandAndOpcode, string err_str = "")
{
    int operand = operandAndOpcode % 1000;
    int opcode = operandAndOpcode / 1000;
    // Taking precedent
    if (operandAndOpcode == 9999 && err_str.compare("Error: Illegal opcode; treated as 9999") == 0)
    {
        AddOrUpdateInMemoryMap(memoryMapCounter, operandAndOpcode, err_str);
    }
    else
    {
        if (operand >= 900)
        {
            int replacingOperand = 999;
            AddOrUpdateInMemoryMap(memoryMapCounter, (opcode * 1000) + replacingOperand, "Error: Illegal immediate operand; treated as 999");

        }
        else
        {
            AddOrUpdateInMemoryMap(memoryMapCounter, operandAndOpcode, err_str);
        }
    }
};

void ComputeAbsoluteAddressForExternalInstr(int operandAndOpcode, string err_str = "")
{
    int operand = operandAndOpcode % 1000;
    int opCode = operandAndOpcode / 1000;

    // Taking precedent
    if (operandAndOpcode == 9999 && err_str.compare("Error: Illegal opcode; treated as 9999") == 0)
    {
        AddOrUpdateInMemoryMap(memoryMapCounter, operandAndOpcode, err_str);
    }
    else
    {
        if(pass == 2){
            if (operand >= mapForSymbolPositionInList.size())
            {
                // operand too large for use list
                return ComputeAbsoluteAddressForRelativeInstr((opCode * 1000), "Error: External operand exceeds length of uselist; treated as relative=0");
            }

            pair<string, bool> symbolAndFlagAtThatPosition = mapForSymbolPositionInList[operand];
            string symbolAtThatPosition = symbolAndFlagAtThatPosition.first;
            // first change bool to true because it is now being used:
            pair<string, bool> newSymbolAndFlagAtThatPosition(symbolAndFlagAtThatPosition.first, true);
            mapForSymbolPositionInList[operand] = newSymbolAndFlagAtThatPosition;

            vector<pair<string, pair<int, string> > >::iterator symbolTableItr = FindInSymbolTable(symbolAtThatPosition);
            if (symbolTableItr != symbolTable.end())
            {
                // we have it in the symbol table and we know the value of it i.e. base + value in module, then we replace it in memory map
                int replacingOperand = (opCode * 1000) + (symbolTableItr->second).first;
                AddOrUpdateInMemoryMap(memoryMapCounter, replacingOperand, err_str);

            }  
            else
            {
                // This means we didn't ever define this symbol
                string err_msg = "Error: " + symbolAtThatPosition + " is not defined; zero used";
                AddOrUpdateInMemoryMap(memoryMapCounter, (opCode * 1000), err_msg);
            }
        }
        else{
            AddOrUpdateInMemoryMap(memoryMapCounter, -1, err_str);
        }        
    }
};

int ReadSymbolValuePairListOfModule(char *token)
{

    if (IsTokenValid(token))
    {
        switch (nextExpectedTokenType)
        {
        case defcount:
            noOfPairsInModule = stoi(token);
            if (noOfPairsInModule == 0)
                nextExpectedTokenType = usecount;
            else
                nextExpectedTokenType = symbol;
            break;
        case symbol:

            if (FindInSymbolTable(token) == symbolTable.end())
            {
                // not present: then add
                AddOrUpdateInSymbolTable(token, -1, "");
                globalHistOfSymbolDefAndUsage[token]= pair<int, bool>(moduleNumber, false);
                string symbolThatNeedsToBeUpdated = token;
                token = GetNextToken();
                nextExpectedTokenType = valueOfSymbol;

                if (IsTokenValid(token))
                {
                    AddOrUpdateInSymbolTable(symbolThatNeedsToBeUpdated, baseAddressForUpcomingModule + stoi(token), "");
                }
                else
                {

                    return 0;
                }
                
            }
            else
            {
                // we need to replace the value with new value
                string symbolThatNeedsToBeUpdated = token;
                vector<pair<string, pair<int, string> > >::iterator symbolTableItr = FindInSymbolTable(token);
                
                // Get Next Token anyway and discard because we have already read this info in Pass no. 1
                token = GetNextToken();
                nextExpectedTokenType = valueOfSymbol;

                if (IsTokenValid(token))
                {
                    if (pass == 1)
                    {
                        
                        AddOrUpdateInSymbolTable(symbolThatNeedsToBeUpdated, (symbolTableItr->second).first, "Error: This variable is multiple times defined; first value used");
                        WarningList.push_back("Warning: Module " + to_string(moduleNumber+1) + ": " + symbolThatNeedsToBeUpdated + " redefinition ignored\n");
                    }
                }
                else
                {

                    return 0;
                }
                
            }

            noOfPairsInModule -= 1;
            if (noOfPairsInModule > 0)
                nextExpectedTokenType = symbol;
            else
                nextExpectedTokenType = usecount;
            break;

        default:
            // unexpected thing exit process
            cout << "Unexpected token: ERROR \n";
            break;
        }

        return 1;
    }

    return 0;
};

int ReadSymbolListOfModule(char *token)
{

    if (IsTokenValid(token))
    {
        switch (nextExpectedTokenType)
        {
        case usecount:

            noOfSymbolsUsedInModule = stoi(token);
            generalCounter = noOfSymbolsUsedInModule;
            if (noOfSymbolsUsedInModule == 0) nextExpectedTokenType = codecount;
            else nextExpectedTokenType = symbol;
            break;
        case symbol:
        {
            if(pass == 2){
                // We will use this later for E instructions: only storing in pass 2 because not required in Pass 1
                pair<string, bool> symbolAndUsedFlag(token, false);
                mapForSymbolPositionInList[(generalCounter - noOfSymbolsUsedInModule)]=symbolAndUsedFlag;
            }
            
            noOfSymbolsUsedInModule -= 1;
            if (noOfSymbolsUsedInModule > 0)
                nextExpectedTokenType = symbol;
            else
                nextExpectedTokenType = codecount;
            break;
        }

        default:
            // unexpected scenario: exit process;
            cout << "Unexpected token: ERROR \n";
            break;
        }

        return 1;
    }

    return 0;
};

int ReadInstructionList(char *token)
{
    if (IsTokenValid(token))
    {
        switch (nextExpectedTokenType)
        {
        case codecount:

            noOfInstructionsInModule = stoi(token);

            if (moduleToBaseAddress.find(moduleNumber) == moduleToBaseAddress.end())
            {
                // not present that means add it and move on.. try again in next pass
                moduleToBaseAddress[moduleNumber]=baseAddressForUpcomingModule;
                baseAddressForUpcomingModule = baseAddressForUpcomingModule + noOfInstructionsInModule;
            }

            if (noOfInstructionsInModule == 0)
                nextExpectedTokenType = defcount;
            else nextExpectedTokenType = addressingMode;
            break;
        case addressingMode:

            if (strcmp(token, "M") == 0)
            {
                nextExpectedTokenType = operandAndOpcode;
                token = GetNextToken();
                if(IsTokenValid(token)){
                    ComputeAbsoluteAddressForModuleInstr(stoi(token), moduleNumber);
                }
                else{
                    ComputeAbsoluteAddressForModuleInstr(9999, moduleNumber, "Error: Illegal opcode; treated as 9999");
                }
                
            }

            else if (strcmp(token, "A") == 0)
            {
                nextExpectedTokenType = operandAndOpcode;
                token = GetNextToken();
                if(IsTokenValid(token)){
                    ComputeAbsoluteAddressForAbsoluteInstr(stoi(token));
                }
                else{
                    ComputeAbsoluteAddressForAbsoluteInstr(9999, "Error: Illegal opcode; treated as 9999");
                }
                
            }

            else if (strcmp(token, "R") == 0)
            {
                nextExpectedTokenType = operandAndOpcode;
                token = GetNextToken();
                if(IsTokenValid(token)){
                    ComputeAbsoluteAddressForRelativeInstr(stoi(token));
                }
                else{
                    ComputeAbsoluteAddressForRelativeInstr(9999, "Error: Illegal opcode; treated as 9999");
                }
                
    
            }

            else if (strcmp(token, "I") == 0)
            {
                nextExpectedTokenType = operandAndOpcode;
                token = GetNextToken();
                if(IsTokenValid(token)){
                    ComputeAbsoluteAddressForImmediateInstr(stoi(token));
                }
                else{
                    ComputeAbsoluteAddressForImmediateInstr(9999, "Error: Illegal opcode; treated as 9999");
                }
                
            }

            else if (strcmp(token, "E") == 0)
            {
                nextExpectedTokenType = operandAndOpcode;
                token = GetNextToken();
                if(IsTokenValid(token)){
                    ComputeAbsoluteAddressForExternalInstr(stoi(token));
                }
                else{
                    ComputeAbsoluteAddressForExternalInstr(9999, "Error: Illegal opcode; treated as 9999");
                }
            }

            memoryMapCounter++;

            noOfInstructionsInModule -= 1;
            if (noOfInstructionsInModule > 0)
                nextExpectedTokenType = addressingMode;
            else
                nextExpectedTokenType = defcount;

            break;

        default:
            // unexpected scenario: exit process;
            cout << "Unexpected token: ERROR \n";
            break;
        }

        return 1;
    }

    return 0;
}

vector<pair<int, pair<int, string> > >::iterator FindInMemoryMap(int memoryCounter){

    for (vector<pair<int, pair<int, string> > >::iterator itr = memoryMap.begin(); itr != memoryMap.end(); itr++)
    {
        if (itr->first >= memoryCounter)
        {
            return itr;
        }
    }

    return memoryMap.end();
}

void AddOrUpdateInMemoryMap(int memoryCounter, int value, string errStr){

    //if we find then update else push_back
    vector<pair<int, pair<int, string> > >::iterator memMapItr = FindInMemoryMap(memoryCounter);

    //found it
    if(memMapItr != memoryMap.end()){
        (memMapItr->second).first = value;
        (memMapItr->second).second = errStr;
        return;
    }

    pair<int, string> memoryAddressAndErrStr(value, errStr);
    memoryMap.push_back(pair<int, pair <int, string> > (memoryMapCounter, memoryAddressAndErrStr));
    return;
}

vector<pair<string, pair<int, string> > >::iterator FindInSymbolTable(string symbol){

    for (vector<pair<string, pair<int, string> > >::iterator itr = symbolTable.begin(); itr != symbolTable.end(); itr++)
    {
        if (symbol.compare(itr->first) == 0)
        {
            return itr;
        }
    }

    return symbolTable.end();
}

void AddOrUpdateInSymbolTable(string symbol, int value, string errStr){

    //if we find then update else push_back
    vector<pair<string, pair<int, string> > >::iterator symTableItr = FindInSymbolTable(symbol);

    //found it
    if(symTableItr != symbolTable.end()){
        (symTableItr->second).first = value;
        (symTableItr->second).second = errStr;
        return;
    }

    pair<int, string> valAndErrStr(value, errStr);
    symbolTable.push_back(pair<string, pair <int, string> > (symbol, valAndErrStr));
    return;
}
