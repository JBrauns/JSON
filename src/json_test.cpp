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
    JSONObject *root = parser->Parse(fileName);
    
    TEST_EXPRESSION(root, "Root Retrieval");
    if(root)
    {
        JSONString *jsonString = root->GetString("String");
        TEST_EXPRESSION(jsonString, "String Retrieval");
        if(jsonString)
        {
            TEST_EXPRESSION((jsonString->Type == JSONValue_String), "String Type");
            TEST_EXPRESSION((*jsonString == "Hello World!"), "String Value");
        }
        
        JSONNumber *jsonIntNeg = root->GetNumber("IntNeg");
        TEST_EXPRESSION(jsonIntNeg, "IntNeg Retrieval");
        if(jsonIntNeg)
        {
            TEST_EXPRESSION((jsonIntNeg->Type == JSONValue_Number), "IntNeg Type");
            TEST_EXPRESSION((*jsonIntNeg == -3458), "IntNeg Value");
        }
        
        JSONNumber *jsonIntPos = root->GetNumber("IntPos");
        TEST_EXPRESSION(jsonIntPos, "IntPos Retrieval");
        if(jsonIntPos)
        {
            TEST_EXPRESSION((jsonIntPos->Type == JSONValue_Number), "IntPos Type");
            TEST_EXPRESSION((*jsonIntPos == 42), "IntPos Value");
        }
        
        JSONNumber *jsonFloat = root->GetNumber("Float");
        TEST_EXPRESSION(jsonFloat, "Float Retrieval");
        if(jsonFloat)
        {
            TEST_EXPRESSION((jsonFloat->Type == JSONValue_Number), "Float Type");
            TEST_EXPRESSION((*jsonFloat == 1234.5678), "Float Value");
        }
        
        JSONArray *jsonArray = root->GetArray("Array");
        TEST_EXPRESSION(jsonArray, "Array Retrieval");
        if(jsonArray)
        {
            TEST_EXPRESSION((jsonArray->Type == JSONValue_Array), "Array Type");

            int testValue = jsonArray->Size() - 1;
            for(int jsonArrayIndex = 0;
                jsonArrayIndex < jsonArray->Size();
                ++jsonArrayIndex)
            {
                testCount += 4;
                JSONNumber *entry = (JSONNumber *)(*jsonArray)[jsonArrayIndex];
                if(!entry)
                {
                    ++failCount;
                    LogMessage("Failed to retrieve entry %d", jsonArrayIndex);
                }
                else
                {
                    char tmp[256];
                    FormatString(tmp, 256, "%llu", jsonArrayIndex);
                    if(!entry->IsKey(tmp))
                    {
                        ++failCount;
                        LogMessage("Entry[%d] Key expected='0' is='%s'", jsonArrayIndex, entry->Key);
                    }
                    if(entry->Type != JSONValue_Number)
                    {
                        ++failCount;
                        LogMessage("Entry[%d] is not of type number", jsonArrayIndex);
                    }
                    if(!(*entry == testValue))
                    {
                        ++failCount;
                        LogMessage("Entry[%d] Value expected=%d is=%d", jsonArrayIndex, testValue, entry->GetInt());
                    }
                }
                --testValue;
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
