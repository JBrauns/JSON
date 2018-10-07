#if !defined(JSON_PARSER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

#define JSON_IDENTIFIER_COMPONENTS 0

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
        if(Key) { free(Key); }
        if(Type == JSONValue_String) { free(String); }
        
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

    JSONObject *GetFirstChild(char *key)
    {
        JSONObject *result = 0;
        if(FirstChild)
        {
            for(JSONObject *child = FirstChild;
                child != LastChild;
                child = result->Next)
            {
                if(strcmp(child->Key, key) == 0)
                {
                    result = child;
                    break;
                }
            }
        }
        
        return(result);
    }
};

class JSONParser :
    public Tokenizer
{
public:
    JSONParser(const char *fileName) :
            Tokenizer(fileName, JSON_IDENTIFIER_COMPONENTS) {}
    ~JSONParser() {}

    //! Parse the complete json file into a json root object
    bool Parse(JSONObject *parent);

    //! Parse a string
    char *ParseString(Token token);

    //! Get a root json object
    JSONObject *GetRoot();
};

#define JSON_PARSER_H
#endif
