/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

JSONObject::JSONObject(char *key)
{
    Key = key;
    Type = JsonValue_Undefined;

    Sentinel = 0;
    Next = Prev = 0;
}
    
JSONObject::~JSONObject()
{
    if(Key) { free(Key); Key = 0; }
    if(Type == JSONValue_String) { free(String); String = 0; }
        
    if(Sentinel)
    {
        JSONObject *child = this->RemoveChild();
        while(child)
        {
            delete child;
            child = 0;
            child = this->RemoveChild();
        }
    }
}

JSONObject *JSONObject::ChildrenInit(JSONObject *element)
{
    element->Next = element;
    element->Prev = element;
    Sentinel = element;

    return(element);
}

JSONObject *JSONObject::InsertChild(JSONObject *element)
{
    if(!Sentinel)
    {
        this->ChildrenInit(element);
    }
    else
    {
        element->Next = Sentinel->Next;
        element->Prev = Sentinel;

        element->Next->Prev = element;
        element->Prev->Next = element;
    }

    return(element);
}

JSONObject *JSONObject::RemoveChild()
{
    JSONObject *result = Sentinel;
    if(Sentinel)
    {
        if(Sentinel->Next == Sentinel)
        {
            AssertMsg(Sentinel->Prev == Sentinel, "Double linked list inconsistent!");
            Sentinel = 0;
        }
        else
        {
            Sentinel->Next->Prev = Sentinel->Prev;
            Sentinel->Prev->Next = Sentinel->Next;

            Sentinel = Sentinel->Next;
        }
    }
    
    return(result);
}
    
JSONObject *JSONObject::InsertChildLast(JSONObject *element)
{
    if(!Sentinel)
    {
        this->ChildrenInit(element);
    }
    else
    {
        element->Next = Sentinel;
        element->Prev = Sentinel->Prev;

        element->Next->Prev = element;
        element->Prev->Next = element;
    }

    return(element);
}
    
JSONObject *JSONObject::GetFirstChild(char *key)
{
    JSONObject *result = 0;
    if(Sentinel)
    {
        for(JSONObject *child = this->GetFirstChild();
            child;
            child = this->IterateAllOnce(child))
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

JSONObject *JSONObject::GetFirstChild()
{
    JSONObject *result = Sentinel;
    MostRecentChild = result->Next;
    
    return(result);
}

JSONObject *JSONObject::IterateAllOnce(JSONObject *child)
{
    JSONObject *result = 0;
    if(child)
    {
        JSONObject *next = child->Next;
        if(next != Sentinel)
        {
            result = next;
        }
    }
        
    return(result);
}

JSONObject *JSONObject::GetNextChild()
{
    JSONObject *result = 0;
    if(MostRecentChild)
    {
        if(MostRecentChild != Sentinel)
        {
            result = MostRecentChild;
            MostRecentChild = result->Next;
        }
    }
        
    return(result);
}

