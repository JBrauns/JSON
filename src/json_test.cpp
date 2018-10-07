/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

#include "json_test.h"
#include "tokenizer.cpp"
#include "json_parser.cpp"

void TestBasic(const char *fileName)
{
    unsigned int testCount = 0;
    unsigned int failCount = 0;
    
    JSONParser *parser = new JSONParser(fileName);
    JSONObject *root = parser->GetRoot();
    if(!parser->Parse(root))
    {
        LogMessage("ERROR >> Failed to open test file!");
    }
    else
    {
        JSONObject *jsonString = root->GetFirstChild("String");
        
        ++testCount;
        if(jsonString->Type != JSONValue_String)
        {
            ++failCount;
            LogMessage("Member String has wrong type!");
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
