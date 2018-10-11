/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

bool JSONParser::Parse(JSONObject *parent)
{
    bool result = true;
        
    Token token = GetNextJsonToken();

    JSONObject *keyValuePair = 0;
    bool inValueParsing = false;
    
    while((token.Type != TokenType_Undefined) && (token.Type != TokenType_EndOfStream))
    {
        if(token.Type == TokenType_CurlyBracketOpen)
        {
            if(!parent)
            {
                LogMessage("Init root");
            
                parent = new JSONObject(StringToHeapMem("root"));
                parent->Type = JSONValue_Object;

                this->Parse(parent);
            }
            else
            {
                if(inValueParsing)
                {
                    LogMessage("Value is object");
                    
                    keyValuePair->Type = JSONValue_Object;
                    result = this->Parse(keyValuePair);
                    if(result)
                    {
                        parent->InsertChild(keyValuePair);
                    }
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
            LogMessage("TokenType: TokenType_SquareBracketOpen");
            
            if(parent)
            {
                if(inValueParsing)
                {
                    keyValuePair->Type = JSONValue_Array;
                    result = this->Parse(keyValuePair);
                    if(result)
                    {
                        parent->InsertChild(keyValuePair);
                    }
                }
                else
                {
                    LogMessage("ERROR >> JSON array cannot be a key");
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
            LogMessage("TokenType: TokenType_Colon");
            inValueParsing = true;
        }
        else if(token.Type == TokenType_Comma)
        {
            inValueParsing = false;
        }
        else if(token.Type == TokenType_String)
        {
            if(parent)
            {
                if(inValueParsing)
                {
                    keyValuePair->String = token.Text;
                    keyValuePair->Type = JSONValue_String;
                    parent->InsertChild(keyValuePair);
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
                if(inValueParsing)
                {
                    keyValuePair->Number = atof(token.Text);
                    keyValuePair->Type = JSONValue_Number;
                    parent->InsertChild(keyValuePair);
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
                if(inValueParsing)
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
                    parent->InsertChild(keyValuePair);
                }
                else
                {
                    LogMessage("ERROR >> Identifier cannot be value");
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
            result = true;
            break;
        }
        else
        {
            // LogMessage("TokenType: %d", token.Type);
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
