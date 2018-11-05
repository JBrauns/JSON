/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

//
// JSONValue
//

JSONValue::JSONValue(char *key, size_t keyLength, JSONValueType type)
{
    KeyLength = keyLength;
    Key = (char *)malloc((KeyLength + 1)*sizeof(char));
    strncpy(Key, key, KeyLength);
    Key[KeyLength] = 0;

    Type = type;
}
    
JSONValue::~JSONValue()
{
    if(Key) { free(Key); Key = 0; }
}

JSONKeyCompare JSONValue::CompareKey(char *key)
{
    JSONKeyCompare result;
    size_t keyLenght = strlen(key);
    int rel = strncmp(key, Key, keyLenght);
    if(rel < 0)
    {
        // str1 has lower value than in str2
        result = JSONKey_Less;
    }
    else if(rel == 0)
    {
        // str1 equal to str2
        result = JSONKey_Equal;
    }
    else
    {
        // str1 has higher value than in str2
        result = JSONKey_Greater;
    }

    return(result);
}

bool JSONValue::IsKey(char *key)
{
    bool result = (CompareKey(key) == JSONKey_Equal);
    return(result);
}

//
// JSONString
//
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

//
// JSONNumber
//

int JSONNumber::GetInt()
{
    Assert(IsInteger);
    return(S64Value);
}

long long int JSONNumber::GetLong()
{
    Assert(IsInteger);
    return(S64Value);
}

double JSONNumber::GetDouble()
{
    Assert(!IsInteger);
    return(R64Value);
}

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

//
// JSONLiteral
//

bool operator==(JSONLiteral &lhs, JSONLiteral *rhs)
{
    bool result = (lhs.Value == rhs->Value);
    return(result);
}

bool operator==(JSONLiteral &lhs, char *rhs)
{
    bool result = false; // TODO(joshua): Implement this => (this->Value == StringToLiteral(rhs));
    return(result);
}

//
// JSONObject
//

void JSONObject::Push(JSONValue *item)
{
    if(!Sentinel)
    {
        item->Next = item->Prev = item;
        Sentinel = item;
        ++ChildCount;
    }
    else
    {
        JSONValue *iter = Sentinel;
        JSONKeyCompare rel = iter->CompareKey(item->Key);
        while(rel == JSONKey_Greater)
        {
            iter = iter->Next;
            rel = iter->CompareKey(item->Key);
                
            if(iter == Sentinel)
            {
                break;
            }
        }

        // TODO(joshua): Do error handling
        Assert(rel != JSONKey_Equal);
        item->Prev = iter->Prev;
        item->Next = iter;
        iter->Prev->Next = item;
        iter->Prev = item;

        if((iter == Sentinel) && (rel == JSONKey_Less))
        {
            Sentinel = item;
        }
            
        ++ChildCount;
    }
}

size_t JSONObject::Size()
{
    return(ChildCount);
}
    
JSONValue *JSONObject::GetIterator()
{
    Iterator = Sentinel;
    return(Iterator);
}

JSONValue *JSONObject::GetNext()
{
    JSONValue *result = 0;
    JSONValue *test = Iterator->Next;
    if(test != Sentinel)
    {
        result = Iterator = test;
    }
        
    return(result);
}

JSONValue *JSONObject::GetChild(char *key, size_t keyLength)
{
    JSONValue *result = 0;
    for(result = GetIterator();
        result;
        result = GetNext())
    {
        if(result->IsKey(key))
        {
            break;
        }
    }
        
    return(result);
}
    
JSONNumber *JSONObject::GetNumber(const char *key)
{
    JSONNumber *result = 0;
    JSONValue *item = GetChild((char *)key, strlen(key));
    if(item && (item->Type == JSONValue_Number))
    {
        result = (JSONNumber *)item;
    }
        
    return(result);
}
    
JSONString *JSONObject::GetString(const char *key)
{
    JSONString *result = 0;
    JSONValue *item = GetChild((char *)key, strlen(key));
    if(item && (item->Type == JSONValue_String))
    {
        result = (JSONString *)item;
    }
    return(result);
}
    
JSONLiteral *JSONObject::GetLiteral(const char *key)
{
    JSONLiteral *result = 0;
    JSONValue *item = GetChild((char *)key, strlen(key));
    if(item && (item->Type == JSONValue_Literal))
    {
        result = (JSONLiteral *)item;
    }
    return(result);
}
    
JSONArray *JSONObject::GetArray(const char *key)
{
    JSONArray *result = 0;
    JSONValue *item = GetChild((char *)key, strlen(key));
    if(item && (item->Type == JSONValue_Array))
    {
        result = (JSONArray *)item;
    }
    return(result);
}
    
JSONObject *JSONObject::GetObject(const char *key)
{
    JSONObject *result = 0;
    JSONValue *item = GetChild((char *)key, strlen(key));
    if(item && (item->Type == JSONValue_Object))
    {
        result = (JSONObject *)item;
    }
    return(result);
}
