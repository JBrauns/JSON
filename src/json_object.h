#if !defined(JSON_OBJECT_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

#include <stdio.h>

enum JSONValueType
{
    JSONValue_Undefined,
    
    JSONValue_Object,
    JSONValue_Array,
    JSONValue_Number,
    JSONValue_String,
    JSONValue_Literal
};

enum JSONLiteralType
{
    JSONLiteral_Invalid,
    
    JSONLiteral_False,
    JSONLiteral_True,
    JSONLiteral_Null
};

JSONLiteralType JSONToLiteral(char *literal, size_t literalLength)
{
    JSONLiteralType result = JSONLiteral_Invalid;
        
    if(strncmp("true", literal, literalLength) == 0)
    {
        result = JSONLiteral_True;
    }
    else if(strncmp("false", literal, literalLength) == 0)
    {
        result = JSONLiteral_False;
    }
    else if(strncmp("null", literal, literalLength) == 0)
    {
        result = JSONLiteral_Null;
    }

    return(result);
}

struct JSONArray;
struct JSONString;
struct JSONLiteral;

enum JSONKeyCompare
{
    JSONKey_Less,
    JSONKey_Equal,
    JSONKey_Greater
};

struct JSONValue
{
    char *Key;
    size_t KeyLength;
    JSONValueType Type;

    JSONValue *Next;
    JSONValue *Prev;

    JSONValue *Sentinel;
    JSONValue *MostRecentChild;

    JSONValue *NextChild;
    JSONValue *PrevChild;

    //! Constructor
    JSONValue(char *key, size_t keyLength, JSONValueType type);
    
    //! Destructor
    ~JSONValue();

    //! Returns if the key is less, equal or greater based on a string compare; E.g: key greater than value->Key
    JSONKeyCompare CompareKey(char *key);

    //! Return true if the key is equal to the values key
    bool IsKey(char *key);
    
    //! Must be called once to initialize the children dlist. Returns the element itself
    JSONValue *ChildrenInit(JSONValue *element);

    //! Insert prepend to dlist. Will call ChildrenInit if list is not initialized. Returns the element itself
    JSONValue *InsertChild(JSONValue *element);

    //! TODO(joshua): Documentation
    JSONValue *RemoveChild();

    //! Insert append to dlist. Will call ChildrenInit if list is not initialized. Returns the element itself
    JSONValue *InsertChildLast(JSONValue *element);

    //! Receive the first element of the dlist of children. Is equal to the sentinel
    JSONValue *GetFirstChild();
    
    //! Returns the first child with the specified key. If none is found the result is null
    JSONValue *GetFirstChild(char *key, size_t keyLength);

    //! Subsequent call this after a call to GetFirstChild. Will return the next children up to the last from then on null
    JSONValue *GetNextChild();
};

struct JSONString
    : JSONValue
{
    char* Value;
    size_t ValueLength;

    JSONString(char *key, size_t keyLength, char *value, size_t valueLength) :
            JSONValue(key, keyLength, JSONValue_String)
    {
        this->ValueLength = valueLength;
        this->Value = (char *)malloc((this->ValueLength + 1)*sizeof(char));
        strncpy(this->Value, value, this->ValueLength);
        this->Value[this->ValueLength] = 0;
    }
};
    
inline bool operator==(JSONString &lhs, JSONString *rhs)
{
    bool result = false;
    if(rhs->ValueLength == lhs.ValueLength)
    {
        result = (strncmp(lhs.Value, rhs->Value, lhs.ValueLength) == 0);
    }
    return(result);
}
    
inline bool operator==(JSONString &lhs, const char *rhs)
{
    bool result = false;
    if(strlen(rhs) == lhs.ValueLength)
    {
        result = (strncmp(lhs.Value, rhs, lhs.ValueLength) == 0);
    }
    return(result);
}
    
struct JSONNumber
    : JSONValue
{
    bool IsInteger;
    union
    {
        double R64Value;
        size_t S64Value;
    };
    
    JSONNumber(char *key, size_t keyLength, double value) :
            JSONValue(key, keyLength, JSONValue_Number)
    {
        this->R64Value  = value;
        this->IsInteger = false;
    }
    
    JSONNumber(char *key, size_t keyLength, long long int value) :
            JSONValue(key, keyLength, JSONValue_Number)
    {
        this->S64Value  = value;
        this->IsInteger = true;
    }

    int GetInt32()
    {
        Assert(IsInteger);
        return(S64Value);
    }

    long long int GetInt64()
    {
        Assert(IsInteger);
        return(S64Value);
    }

    double GetReal64()
    {
        Assert(!IsInteger);
        return(R64Value);
    }
};

bool operator==(JSONNumber &lhs, JSONNumber *rhs)
{
    bool result = false;
    if(lhs.IsInteger && rhs->IsInteger)
    {
        result = (lhs.S64Value == rhs->S64Value);
    }
    else if(!lhs.IsInteger && !rhs->IsInteger)
    {
        result = (lhs.R64Value == rhs->R64Value);
    }
    return(result);
}

bool operator==(JSONNumber &lhs, long long int rhs)
{
    bool result = false;
    if(lhs.IsInteger) { result = (lhs.S64Value == rhs); }
    return(result);
}

bool operator==(JSONNumber &lhs, int rhs)
{
    bool result = false;
    // TODO(joshua): Convert down and check for truncation
    if(lhs.IsInteger) { result = (lhs.S64Value == rhs); }
    return(result);
}

bool operator==(JSONNumber &lhs, double rhs)
{
    bool result = false;
    if(!lhs.IsInteger) { result = (lhs.R64Value == rhs); }
    return(result);
}

struct JSONLiteral
    : JSONValue
{
    JSONLiteralType Value;

    JSONLiteral(char *key, size_t keyLength, JSONLiteralType value) :
            JSONValue(key, keyLength, JSONValue_Literal)
    {
        this->Value = value;
    }

    bool operator==(JSONLiteral *rhs)
    {
        bool result = (this->Value == rhs->Value);
        return(result);
    }

    bool operator==(char *rhs)
    {
        bool result = false; // TODO(joshua): Implement this => (this->Value == StringToLiteral(rhs));
        return(result);
    }
};

struct JSONObject
    : JSONValue
{
    JSONValue *Sentinel;
    JSONValue *Iterator;
    
    size_t ChildCount;
    
    JSONObject(char *key, size_t keyLength) :
            JSONValue(key, keyLength, JSONValue_Object),
            Sentinel(0),
            Iterator(0)
    {
        ChildCount = 0;
    }

    //! Push a new element to this object. Internally gets sorted descending by key
    void Push(JSONValue *item);

    //! Return the current children size. No recursion
    size_t Size();
    
    //! Returns the first child for iterating over this object childs
    JSONValue *GetIterator();

    //! Call this after getting the iterator to iterate until the end. After the last element was returned returns null
    JSONValue *GetNext();

    //! Returns a valid JSONValue if a matching element based on the key was found. Otherwise null
    JSONValue *GetChild(char *key, size_t keyLength);
    
    //! Returns a valid JSONNumber if a matching element based on the key and of type JSONNumber was found. Otherwise null
    JSONNumber *GetNumber(const char *key);
    
    //! Returns a valid JSONString if a matching element based on the key and of type JSONString was found. Otherwise null
    JSONString *GetString(const char *key);
    
    //! Returns a valid JSONLiteral if a matching element based on the key and of type JSONLiteral was found. Otherwise null
    JSONLiteral *GetLiteral(const char *key);
    
    //! Returns a valid JSONArray if a matching element based on the key and of type JSONArray was found. Otherwise null
    JSONArray *GetArray(const char *key);
    
    //! Returns a valid JSONObject if a matching element based on the key and of type JSONObject was found. Otherwise null
    JSONObject *GetObject(const char *key);
};

struct JSONArray
    : JSONValue
{
    JSONValue *ChildFirst;
    JSONValue *ChildLast;
    size_t     ChildCount;
    
    JSONArray(char *key, size_t keyLength) :
            JSONValue(key, keyLength, JSONValue_Array),
            ChildCount(0),
            ChildFirst(0),
            ChildLast(0)
    {
        ChildCount = 0;
    }

    size_t Size()
    {
        return(ChildCount);
    }
    
    void Init(JSONValue *item)
    {
        ChildFirst = item;
        ChildLast = item;
        ChildFirst->Next = ChildLast;
        ChildLast->Prev = ChildFirst;

        ++ChildCount;
    }
    
    void Append(JSONValue *item)
    {
        if(!ChildFirst)
        {
            Assert(!ChildLast);
            Init(item);            
        }
        else
        {
            ChildLast->Next = item;
            item->Prev = ChildLast;
            item->Next = 0;
            ChildLast = item;
            
            ++ChildCount;
        }
    }
    
    JSONValue *operator[](size_t index)
    {
        JSONValue *result = 0;
        Assert(index < ChildCount);
        if(index < ChildCount)
        {
            result = ChildFirst;
            while(index > 0)
            {
                if(!result)
                {
                    break;
                }
                
                result = result->Next;
                --index;
            }
        }
        
        return(result);
    }
};

#define JSON_OBJECT_H
#endif
