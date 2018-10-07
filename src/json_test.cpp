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
    JSONParser *parser = new JSONParser(fileName);
    JSONObject *root = parser->GetRoot();
    if(!parser->Parse(root))
    {
        LogMessage("ERROR >> Failed to open test file!");
    }
    else
    {
        LogMessage("Test not implemented yet!");
    }
        
    delete root;
}

int main(int argumentCount, char **arguments)
{
    TestBasic("test_basic.json");
    
    return(0);
}
