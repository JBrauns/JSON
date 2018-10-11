/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

bool JSONParser::Parse(JSONObject **parentPtr)
{
    bool result = true;
    JSONObject *parent = *parentPtr;
        
    Token token = GetNextJsonToken();

    JSONObject *keyValuePair = 0;
    bool isValue = false;
    bool isNegative = false;
    bool inArray = false;
    
    if(parent && parent->Type == JSONValue_Array)
    {
        inArray = true;
    }
    
    while((token.Type != TokenType_Undefined) && (token.Type != TokenType_EndOfStream))
    {
        if(token.Type == TokenType_CurlyBracketOpen)
        {
            if(!parent)
            {
                *parentPtr = new JSONObject(StringToHeapMem("root"));
                parent = *parentPtr;
                parent->Type = JSONValue_Object;

                this->Parse(parentPtr);
            }
            else
            {
                if(isValue)
                {
                    keyValuePair->Type = JSONValue_Object;
                    result = this->Parse(&keyValuePair);
                }
                else
                {
                    LogMessage("ERROR >> JSON object cannot be a key");
                    result = false;
                }
                
                if(!result) { break; }
            }
        }
        else if(token.Type == TokenType_SquareBracketOpen)
        {
            if(parent)
            {
                if(isValue)
                {
                    if(isValue)
                    {
                        keyValuePair->Type = JSONValue_Array;
                        result = this->Parse(&keyValuePair);
                    }
                    else
                    {
                        LogMessage("ERROR >> JSON array cannot be a key");
                        result = false;
                    }
                }
                else
                {
                    LogMessage("ERROR >> JSON array cannot be negative");
                    result = false;
                }
                    
                if(!result) { break; }
            }
            else
            {
                LogMessage("ERROR >> JSON array not allowed as root");
                result = false;
                break;
            }
        }
        else if(token.Type == TokenType_Colon)
        {
            isValue = true;
        }
        else if(token.Type == TokenType_Comma)
        {
            if(inArray)
            {
                parent->InsertChildLast(keyValuePair);
            }
            else
            {
                parent->InsertChild(keyValuePair);
            }
            
            isValue = false;
            isNegative = false;
        }
        else if(token.Type == TokenType_String)
        {
            if(parent)
            {
                if(isValue)
                {
                    keyValuePair->String = token.Text;
                    keyValuePair->Type = JSONValue_String;
                }
                else
                {
                    keyValuePair = new JSONObject(token.Text);
                }
            }
            else
            {
                LogMessage("ERROR >> String found in NULL scope");
                result = false;
            }
            
            if(!result) { break; }
        }
        else if(token.Type == TokenType_Number)
        {
            if(parent)
            {
                if(isValue)
                {
                    keyValuePair->Number = atof(token.Text);
                    keyValuePair->Type = JSONValue_Number;
                    if(isNegative)
                    {
                        keyValuePair->Number *= -1.0;
                        isNegative = false;
                    }
                }
                else
                {
                    LogMessage("ERROR >> Number cannot be value");
                    result = false;
                }
            }
            else
            {
                LogMessage("ERROR >> String found in NULL scope");
                result = false;
            }
            
            if(!result) { break; }
        }
        else if(token.Type == TokenType_Identifier)
        {
            if(parent)
            {
                if(isNegative)
                {
                    if(isValue)
                    {
                        if(strncmp("true", token.Text, strlen(token.Text)) == 0)
                        {
                            keyValuePair->Literal = JSONLiteral_True;
                        }
                        else if(strncmp("false", token.Text, strlen(token.Text)) == 0)
                        {
                            keyValuePair->Literal = JSONLiteral_False;
                        }
                        else if(strncmp("null", token.Text, strlen(token.Text)) == 0)
                        {
                            keyValuePair->Literal = JSONLiteral_Null;
                        }
                        else
                        {
                            keyValuePair->Literal = JSONLiteral_Invalid;
                            LogMessage("ERROR >> Invalid literal '%s' [true|false|null]", token.Text);
                            result = false;
                        }
                    
                        keyValuePair->Type = JSONValue_Literal;
                    }
                    else
                    {
                        LogMessage("ERROR >> Identifier cannot be value");
                        result = false;
                    }
                }
                else
                {
                    LogMessage("ERROR >> Identifier cannot be negative");
                    result = false;
                }
            }
            else
            {
                LogMessage("ERROR >> String found in NULL scope");
                result = false;
            }
            
            if(!result) { break; }
        }
        else if(token.Type == TokenType_SquareBracketClose)
        {
            result = true;
            break;
        }
        else if(token.Type == TokenType_CurlyBracketClose)
        {
            if(!isNegative)
            {
                if(keyValuePair && isValue)
                {
                    if(inArray)
                    {
                        parent->InsertChildLast(keyValuePair);
                    }
                    else
                    {
                        parent->InsertChild(keyValuePair);
                    }
                    result = true;
                }
            }
            else
            {
                LogMessage("ERROR >> Unhandled minus detected");
                result = false;
            }
            
            break;
        }
        else if(token.Type == TokenType_Minus)
        {
            isNegative = true;
        }
        else
        {
            LogMessage("ERROR >> Unhandled token");
        }
        
        token = GetNextJsonToken();
    }

    return(result);
}

Token JSONParser::GetNextJsonToken()
{
    Token result = GetNextToken();

    // NOTE(joshua): Handle strings correctly
    if(result.Type == TokenType_DoubleQuotationMark)
    {
        char *stringStart = 0;
        unsigned int stringLength = StringBetweenToken(result, &stringStart);

        result.Type = TokenType_String;
        result.Text = _StringToHeapMem(stringStart, stringLength);
    }
    else if(result.Type == TokenType_Number)
    {
        result.Text = _StringToHeapMem(result.Text, result.Length);
    }

    return(result);
}

char *JSONParser::ParseString(Token token)
{
    char *stringStart = 0;
    size_t stringLength = StringBetweenToken(token, &stringStart);
                
    char *result = (char *)malloc(stringLength + 1);
    strncpy(result, stringStart, stringLength);
    result[stringLength] = 0;

    return(result);
}

JSONObject *JSONParser::GetRoot()
{
    unsigned int nameLength = 4;
    char *rootName = (char *)malloc(nameLength + 1);
    strncpy(rootName, "root", 4);
        
    JSONObject *result = new JSONObject(rootName);
    return(result);
}
