#if !defined(JSON_OBJECT_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

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
    JSONLiteral_Invalid,
    
    JSONLiteral_False,
    JSONLiteral_True,
    JSONLiteral_Null
};

#define StringToHeapMem(string) _StringToHeapMem(string, strlen(string))
inline char *
_StringToHeapMem(char *string, unsigned int nameLength)
{
    char *result = 0;
        
    if(nameLength > 0)
    {
        result = (char *)malloc(nameLength + 1);
        strncpy(result, string, nameLength);
        result[nameLength] = 0;
    }
    
    return(result);
}

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

    //! Constructor
    JSONObject(char *key);
    
    //! Destructor
    ~JSONObject();

    //! Must be called once to initialize the children dlist. Returns the element itself
    JSONObject *ChildrenInit(JSONObject *element);

    //! Insert prepend to dlist. Will call ChildrenInit if list is not initialized. Returns the element itself
    JSONObject *InsertChild(JSONObject *element);

    //! TODO(joshua): Documentation
    JSONObject *RemoveChild();

    //! Insert append to dlist. Will call ChildrenInit if list is not initialized. Returns the element itself
    JSONObject *InsertChildLast(JSONObject *element);
    
    //! Returns the first child with the specified key. If none is found the result is null
    JSONObject *GetFirstChild(char *key);

    //! Receive the first element of the dlist of children. Is equal to the sentinel
    JSONObject *GetFirstChild();

    //! Iterate over all objects up to the sentinel in next direction. Receive first element wit GetFirstChild. Will return null if sentinel is reached
    JSONObject *IterateAllOnce(JSONObject *element);
};

#define JSON_OBJECT_H
#endif
