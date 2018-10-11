/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

#include "json_test.h"
#include "tokenizer.cpp"
#include "json_object.cpp"
#include "json_parser.cpp"

void TestBasic(const char *fileName)
{
    unsigned int testCount = 0;
    unsigned int failCount = 0;
    
    JSONParser *parser = new JSONParser(fileName);
    JSONObject *root = 0;
    
    ++testCount;
    if(!parser->Parse(root))
    {
        ++failCount;
        LogMessage("ERROR >> Failed to parse test file!");
    }
    else
    {
        JSONObject *jsonString = root->GetFirstChild("String");
        ++testCount;
        if(!jsonString)
        {
            ++failCount;
            LogMessage("Member String not found!");
        }
        else
        {
            ++testCount;
            if(jsonString->Type != JSONValue_String)
            {
                ++failCount;
                LogMessage("Member String has wrong type!");
            }

            ++testCount;
            if(strcmp(jsonString->String, "Hello World!") != 0)
            {
                ++failCount;
                LogMessage("Member String has wrong value!");
            }
        }
        
        JSONObject *jsonNumber = root->GetFirstChild("Number");
        ++testCount;
        if(!jsonNumber)
        {
            ++failCount;
            LogMessage("Member Number not found!");
        }
        else
        {
            ++testCount;
            if(jsonNumber->Type != JSONValue_Number)
            {
                ++failCount;
                LogMessage("Member Number has wrong type!");
            }

            ++testCount;
            if(jsonNumber->Number != 42.0)
            {
                ++failCount;
                LogMessage("Member Number has wrong value!");
            }
        }
        
        JSONObject *jsonFloat = root->GetFirstChild("Float");
        ++testCount;
        if(!jsonFloat)
        {
            ++failCount;
            LogMessage("Member Float not found!");
        }
        else
        {
            ++testCount;
            if(jsonFloat->Type != JSONValue_Number)
            {
                ++failCount;
                LogMessage("Member Float has wrong type!");
            }

            ++testCount;
            if(jsonFloat->Number != 1234.5678)
            {
                ++failCount;
                LogMessage("Member Float has wrong value!");
            }
        }
        
        JSONObject *jsonArray = root->GetFirstChild("Array");
        ++testCount;
        if(!jsonArray)
        {
            ++failCount;
            LogMessage("Member Array not found!");
        }
        else
        {
            ++testCount;
            if(jsonArray->Type != JSONValue_Array)
            {
                ++failCount;
                LogMessage("Member Array has wrong type!");
            }

            unsigned int entryIndex = 0;
            for(JSONObject *entry = jsonArray->GetFirstChild();
                entry;
                entry = jsonArray->IterateAllOnce(entry))
            {
                ++testCount;
                if(entry->Type != JSONValue_Number)
                {
                    ++failCount;
                    LogMessage("Member has wrong type!");
                }

                ++testCount;
                if((entryIndex == 0) && (strcmp(entry->Key, "Entry1") != 0) && (entry->Number != 1))
                {
                    ++failCount;
                    LogMessage("Entry1 wrong position!");
                }
                else if((entryIndex == 1) && (strcmp(entry->Key, "Entry0") != 0) && (entry->Number != 0))
                {
                    ++failCount;
                    LogMessage("Entry0 wrong position!");
                }
                else if((entryIndex == 2) && (strcmp(entry->Key, "Entry2") != 0) && (entry->Number != 2))
                {
                    ++failCount;
                    LogMessage("Entry2 wrong position!");
                }
                
                ++entryIndex;
            }
        }
    }

    LogMessage("Test finished: Failed=%d Passed=%d from total %d",
               failCount, testCount - failCount, testCount);
        
    delete root;
}

int main(int argumentCount, char **arguments)
{
    TestBasic("test_basic.json");
    
    return(0);
}
