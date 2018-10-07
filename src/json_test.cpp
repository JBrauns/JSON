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

enum JSONValueType
{
    JsonValue_Undefined,
    
    JSONValue_Object,
    JSONValue_Array,
    JSONValue_Number,
    JSONValue_String,
    JSONValue_Literal
};

enum JSONLiteral
{
    JSONLiteral_False,
    JSONLiteral_True,
    JSONLiteral_Null
};

struct JSONObject
{
    char *Key;
    JSONValueType Type;
    
    unsigned int ChildCount;
    JSONObject *FirstChild;
    JSONObject *LastChild;

    JSONObject *Next;
    JSONObject *Prev;

    union
    {
        double Number;
        JSONLiteral Literal;
        char *String;
    };

    JSONObject(char *key)
    {
        Key = key;
        Type = JsonValue_Undefined;
    
        ChildCount = 0;
        FirstChild = LastChild = 0;
        Next = Prev = 0;
    }
    ~JSONObject()
    {
        if(Key)
        {
            LogMessage("Destruct: key=%s", Key);
            free(Key);
        }
        if((Type == JSONValue_String) || (Type == JSONValue_Array))
        {
            free(String);
        }
        
        if(ChildCount > 0)
        {
            JSONObject *child = FirstChild;
            while(ChildCount > 0)
            {
                if(!child)
                {
                    break;
                }

                JSONObject *nextChild = child->Next;
                delete child;
                child = nextChild;
            
                --ChildCount;
            }
        }
    }

    void InsertChild(JSONObject *child)
    {
        if(!FirstChild)
        {
            FirstChild = child;
            LastChild = child;

            FirstChild->Next = LastChild;
            LastChild->Prev = FirstChild;
        }
        else
        {
            LastChild->Next = child;
            child->Prev = LastChild;

            LastChild = child;
        }
        
        ++ChildCount;
    }
};

#define JSON_IDENTIFIER_COMPONENTS 0

class JSONParser :
    public Tokenizer
{
private:
public:
    JSONParser(const char *fileName) :
            Tokenizer(fileName, JSON_IDENTIFIER_COMPONENTS)
    {
    }
    ~JSONParser() {}

    bool Parse(JSONObject *parent)
    {
        bool result = true;
        
        Token token = GetNextToken();
        while((token.Type != TokenType_Undefined) || (token.Type != TokenType_EndOfStream))
        {
            if(token.Type == TokenType_EndOfStream)
            {
                break;
            }
            else if(token.Type == TokenType_Undefined)
            {
                result = false;
                LogMessage("ERROR >> Undefined token type!");
                break;
            }
            else if(token.Type == TokenType_DoubleQuotationMark)
            {
                char *key = ParseString(token);

                token = GetNextToken();
                if(token.Type == TokenType_Colon)
                {
                    JSONObject *object = new JSONObject(key);
                    
                    token = GetNextToken();
                    if(token.Type == TokenType_DoubleQuotationMark)
                    {
                        object->Type   = JSONValue_String;
                        object->String = ParseString(token);
                        parent->InsertChild(object);
                    }
                    else if(token.Type == TokenType_Number)
                    {
                        char *number = 0;
                        unsigned int numberLength = token.Length;
                        number = (char *)malloc(numberLength + 1);
                        strncpy(number, token.Text, numberLength);
                        number[numberLength] = 0;
                        
                        object->Number = atof(number);
                        object->Type   = JSONValue_Number;

                        free(number);
                        parent->InsertChild(object);
                    }
                    else if(token.Type == TokenType_SquareBracketOpen)
                    {
                        object->Type = JSONValue_Array;
                        Parse(object);
                        parent->InsertChild(object);
                    }
                    else if(token.Type == TokenType_CurlyBracketOpen)
                    {
                        LogMessage("Add an object!");
                    }
                }
                else
                {
                    LogMessage("ERROR >> JSON Object must be key and value pair!");
                }
            }
            else if(token.Type == TokenType_SquareBracketClose)
            {
                break;
            }
            else if(token.Type == TokenType_CurlyBracketClose)
            {
                break;
            }
            else
            {
                LogMessage("TokenType: %d", token.Type);
            }
            
            token = GetNextToken();
        }

        return(result);
    }

    char *ParseString(Token token)
    {
        char *stringStart = 0;
        size_t stringLength = StringBetweenToken(token, &stringStart);
                
        char *result = (char *)malloc(stringLength + 1);
        strncpy(result, stringStart, stringLength);
        result[stringLength] = 0;

        return(result);
    }

    JSONObject *GetRoot()
    {
        unsigned int nameLength = 4;
        char *rootName = (char *)malloc(nameLength + 1);
        strncpy(rootName, "root", 4);
        
        JSONObject *result = new JSONObject(rootName);
        return(result);
    }
};

int main(int argumentCount, char **arguments)
{
    if(arguments[0])
    {
        JSONParser *parser = new JSONParser("test_basic.json");
        
        JSONObject *root = parser->GetRoot();
        if(!parser->Parse(root))
        {
            LogMessage("ERROR >> Something went wrong!");
        }
        else
        {
            
            LogMessage("Use the json tree");
        }
        
        delete root;
    }
    else
    {
        LogMessage("ERROR >> First command line argument missing!");
    }
    
    return(0);
}
