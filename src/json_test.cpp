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
    
    TEST_EXPRESSION(parser->Parse(&root), "Root Retrieval");
    if(root)
    {
        JSONObject *jsonString = root->GetFirstChild("String");
        TEST_EXPRESSION(jsonString, "String Retrieval");
        if(jsonString)
        {
            TEST_EXPRESSION((jsonString->Type == JSONValue_String), "String Type");
            TEST_EXPRESSION(strcmp(jsonString->String, "Hello World!") == 0, "String Value");
        }
        
        JSONObject *jsonIntNeg = root->GetFirstChild("IntNeg");
        TEST_EXPRESSION(jsonIntNeg, "IntNeg Retrieval");
        if(jsonIntNeg)
        {
            TEST_EXPRESSION((jsonIntNeg->Type == JSONValue_Number), "IntNeg Type");
            TEST_EXPRESSION(((int)jsonIntNeg->Number == -3458), "IntNeg Value");
        }
        
        JSONObject *jsonIntPos = root->GetFirstChild("IntPos");
        TEST_EXPRESSION(jsonIntPos, "IntPos Retrieval");
        if(jsonIntPos)
        {
            TEST_EXPRESSION((jsonIntPos->Type == JSONValue_Number), "IntPos Type");
            TEST_EXPRESSION(((int)jsonIntPos->Number == 42), "IntPos Value");
        }
        
        JSONObject *jsonFloat = root->GetFirstChild("Float");
        TEST_EXPRESSION(jsonFloat, "Float Retrieval");
        if(jsonFloat)
        {
            TEST_EXPRESSION((jsonFloat->Type == JSONValue_Number), "Float Type");
            TEST_EXPRESSION((jsonFloat->Number == 1234.5678), "Float Value");
        }
        
        JSONObject *jsonArray = root->GetFirstChild("Array");
        TEST_EXPRESSION(jsonArray, "Array Retrieval");
        if(jsonArray)
        {
            TEST_EXPRESSION((jsonArray->Type == JSONValue_Array), "Array Type");

            unsigned int entryIndex = 0;
            for(JSONObject *entry = jsonArray->GetFirstChild();
                entry;
                entry = jsonArray->IterateAllOnce(entry))
            {
                if(entryIndex == 0)
                {
                    TEST_EXPRESSION((entry->Type == JSONValue_Number), "Entry1 Type");
                    TEST_EXPRESSION((strcmp(entry->Key, "Entry1") == 0), "Entry1 Positioning");
                    TEST_EXPRESSION((entry->Number == 1), "Entry1 Value");
                }
                else if(entryIndex == 1)
                {
                    TEST_EXPRESSION((entry->Type == JSONValue_Number), "Entry1 Type");
                    TEST_EXPRESSION((strcmp(entry->Key, "Entry0") == 0), "Entry0 Positioning");
                    TEST_EXPRESSION((entry->Number == 0), "Entry0 Value");
                }
                else if(entryIndex == 2)
                {
                    TEST_EXPRESSION((entry->Type == JSONValue_Number), "Entry2 Type");
                    TEST_EXPRESSION((strcmp(entry->Key, "Entry2") == 0), "Entry2 Positioning");
                    TEST_EXPRESSION((entry->Number == 2), "Entry2 Value");
                }
                
                ++entryIndex;
            }
        }
        
        JSONObject *jsonObject = root->GetFirstChild("Object");
        TEST_EXPRESSION(jsonObject, "Object Retrieval");
        if(jsonObject)
        {
            TEST_EXPRESSION((jsonObject->Type == JSONValue_Object), "Object Type");

            int expectedMembers = 3;
            for(JSONObject *entry = jsonObject->GetFirstChild();
                entry;
                entry = jsonObject->IterateAllOnce(entry))
            {
                if(strcmp(entry->Key, "Member0") == 0)
                {
                    --expectedMembers;
                    TEST_EXPRESSION((entry->Type == JSONValue_Literal), "Member0 Type");
                    TEST_EXPRESSION((entry->Literal == JSONLiteral_True), "Member0 Value");
                }

                if(strcmp(entry->Key, "Member1") == 0)
                {
                    --expectedMembers;
                    TEST_EXPRESSION((entry->Type == JSONValue_Literal), "Member1 Type");
                    TEST_EXPRESSION((entry->Literal == JSONLiteral_False), "Member1 Value");
                }

                if(strcmp(entry->Key, "Member2") == 0)
                {
                    --expectedMembers;
                    TEST_EXPRESSION((entry->Type == JSONValue_Literal), "Member2 Type");
                    TEST_EXPRESSION((entry->Literal == JSONLiteral_Null), "Member2 Value");
                }
            }

            TEST_EXPRESSION((expectedMembers == 0), "Object Member Count");
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
