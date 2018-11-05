/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

JSONValue::JSONValue(char *key, size_t keyLength, JSONValueType type)
        : Sentinel(0),
          NextChild(0),
          PrevChild(0)
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
    if(Sentinel)
    {
        JSONValue *child = this->RemoveChild();
        while(child)
        {
            delete child;
            child = 0;
            child = this->RemoveChild();
        }
    }
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

JSONValue *JSONValue::ChildrenInit(JSONValue *element)
{
    element->NextChild = element;
    element->PrevChild = element;
    Sentinel = element;

    return(element);
}

JSONValue *JSONValue::InsertChild(JSONValue *element)
{
    if(!Sentinel)
    {
        this->ChildrenInit(element);
    }
    else
    {
        element->NextChild = Sentinel->NextChild;
        element->PrevChild = Sentinel;

        element->NextChild->PrevChild = element;
        element->PrevChild->NextChild = element;
    }

    return(element);
}

JSONValue *JSONValue::RemoveChild()
{
    JSONValue *result = Sentinel;
    if(Sentinel)
    {
        if(Sentinel->NextChild == Sentinel)
        {
            AssertMsg(Sentinel->PrevChild == Sentinel, "Double linked list inconsistent!");
            Sentinel = 0;
        }
        else
        {
            Sentinel->NextChild->PrevChild = Sentinel->PrevChild;
            Sentinel->PrevChild->NextChild = Sentinel->NextChild;

            Sentinel = Sentinel->NextChild;
        }
    }
    
    return(result);
}
    
JSONValue *JSONValue::InsertChildLast(JSONValue *element)
{
    if(!Sentinel)
    {
        this->ChildrenInit(element);
    }
    else
    {
        element->PrevChild = Sentinel->PrevChild;
        Sentinel->PrevChild->NextChild = element;
        
        element->NextChild = Sentinel;
        Sentinel->PrevChild = element;
    }

    return(element);
}

JSONValue *JSONValue::GetFirstChild()
{
    JSONValue *result = Sentinel;
    MostRecentChild = result->NextChild;

    //! If a child with matching key and of type JSONValue_Array a pointer to this element is returned otherwise null
    JSONArray *GetArray(char *key);
    
    return(result);
}
    
JSONValue *JSONValue::GetFirstChild(char *key, size_t keyLength)
{
    JSONValue *result = 0;
    if(Sentinel)
    {
        for(JSONValue *child = this->GetFirstChild();
            child;
            child = this->GetNextChild())
        {
            if((keyLength == child->KeyLength) &&
               (strncmp(child->Key, key, child->KeyLength) == 0))
            {
                result = child;
                break;
            }
        }
    }
        
    return(result);
}

JSONValue *JSONValue::GetNextChild()
{
    JSONValue *result = 0;
    if(MostRecentChild)
    {
        if(MostRecentChild != Sentinel)
        {
            result = MostRecentChild;
            MostRecentChild = result->NextChild;
        }
    }
        
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
