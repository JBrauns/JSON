/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

bool JSONParser::Parse(JSONObject **parentPtr, bool inObject)
{
    JSONObject *parent = *parentPtr;
    if(!HasRoot)
    {
        // TODO(joshua): Error handling!
        JSON_ASSERT(!parent);
        HasRoot = true;
    }
    else
    {
        // TODO(joshua): Error handling!
        JSON_ASSERT(parent);
    }
    
    JSONObject *entry = 0;
        
    Token token = this->GetNextJsonToken();
    while((token.Type != TokenType_Undefined) && (token.Type != TokenType_EndOfStream))
    {
        switch(token.Type)
        {
            case TokenType_String:
            {
                if(!entry)
                {
                    entry = new JSONObject(token.Text);
                            
                    if(inObject)
                    {
                        token = GetNextJsonToken();
                        // TODO(joshua): Error handling!
                        JSON_ASSERT(token.Type == TokenType_Colon);
                    }
                }
                else
                {
                    entry->String = token.Text;
                    entry->Type = JSONValue_String;
                }
            } break;

            case TokenType_Minus:
            case TokenType_Number:
            {
                double sign = 1.0;
                if(TokenType_Minus == token.Type)
                {
                    sign = -1.0;
                    token = GetNextJsonToken();
                    // TODO(joshua): Error handling!
                    JSON_ASSERT(token.Type == TokenType_Number);
                }
                            
                entry->Number = atof(token.Text) * sign;
                entry->Type = JSONValue_Number;
            } break;

            case TokenType_Identifier:
            {
                entry->Type = JSONValue_Literal;
                entry->Literal = StringToJSONLiteral(token.Text);
                // TODO(joshua): Error handling!
                JSON_ASSERT(entry->Literal != JSONLiteral_Invalid);
            } break;

            case TokenType_CurlyBracketOpen:
            {
                if(!parent)
                {
                    *parentPtr = new JSONObject(StringToHeapMem("root"));
                    (*parentPtr)->Type = JSONValue_Object;
                    this->Parse(parentPtr, true);
                }
                else
                {
                    entry->Type = JSONValue_Object;
                    if(!this->Parse(&entry))
                    {
                        LogMessage("ERROR >> While parsing object");
                        return(false);
                    }
                }
            } break;

            case TokenType_SquareBracketOpen:
            {
                entry->Type = JSONValue_Array;
                if(!this->Parse(&entry, false))
                {
                    LogMessage("ERROR >> While parsing array");
                    return(false);
                }
            } break;

            default:
            {
                // NOTE(joshua): This could be a comma after the last entry in an array/object
                // TODO(joshua): Error handling!
                LogMessage("ERROR >> Invalid token");
                JSON_ASSERT(!"Invalid token");
            } break;
        }

        token = GetNextJsonToken();
        if(TokenType_Comma == token.Type)
        {
            if(inObject) { parent->InsertChildLast(entry); }
            else { parent->InsertChild(entry); }
            entry = 0;

            token = GetNextJsonToken();
        }
        else if(TokenType_SquareBracketClose == token.Type)
        {
            if(inObject) { parent->InsertChildLast(entry); }
            else { parent->InsertChild(entry); }
            entry = 0;

            // TODO(joshua): Error handling!
            JSON_ASSERT(!inObject);
            return(true);
        }
        else if(TokenType_CurlyBracketClose == token.Type)
        {
            if(inObject) { parent->InsertChildLast(entry); }
            else { parent->InsertChild(entry); }
            entry = 0;

            // TODO(joshua): Error handling!
            JSON_ASSERT(inObject);
            return(true);
        }
    }

    return(true);
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
    else if((result.Type == TokenType_Number) || (result.Type == TokenType_Identifier))
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
