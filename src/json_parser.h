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
    
    JSONObject *Sentinel;

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
    
        Sentinel = 0;
        Next = Prev = 0;
    }
    
    ~JSONObject()
    {
        if(Key) { free(Key); }
        if(Type == JSONValue_String) { free(String); }
        
        if(Sentinel)
        {
            JSONObject *child = Sentinel->Next;
            while(true)
            {
                JSONObject *nextChild = child->Next;
                
                delete child;
                if(nextChild == Sentinel)
                {
                    break;
                }
                child = nextChild;
            }
            delete Sentinel;
        }
    }

    void InsertChild(JSONObject *child)
    {
        if(!Sentinel)
        {
            unsigned int nameLength = 8;
            char *name = (char *)malloc(nameLength + 1);
            strncpy(name, "sentinel", nameLength);
            name[nameLength] = 0;
                
            Sentinel = new JSONObject(name);
            
            child->Next = Sentinel;
            child->Prev = Sentinel;
            
            Sentinel->Next = child;
            Sentinel->Prev = child;
        }
        else
        {
            child->Next = Sentinel;
            child->Prev = Sentinel->Prev;

            Sentinel->Prev->Next = child;
            Sentinel->Prev = child;
        }
    }
    
    JSONObject *GetFirstChild(char *key)
    {
        JSONObject *result = 0;
        if(Sentinel)
        {
            for(JSONObject *child = Sentinel->Next;
                child != Sentinel;
                child = child->Next)
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
