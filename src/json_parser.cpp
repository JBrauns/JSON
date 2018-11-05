/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

JSONObject *JSONParser::Parse(const char *fileName)
{
    JSONObject *result = 0;
    if(!fileName)
    {
        // TODO(joshua): Error handling!
        Assert(!"Filename NULL");
        return(result);
    }
    else
    {
        Token token = GetNextToken();
        if(token.Type == TokenType_CurlyBracketOpen)
        {
            char *key = "root";
            result = ParseObject(key, strlen(key));
            
            token = GetNextToken();
            if(token.Type != TokenType_EndOfStream)
            {
                // TODO(joshua): Error handling!
                Assert(!"Expecting EOF after root object!");
            }
        }
        else
        {
            // TODO(joshua): Error handling!
            Assert(!"Expecting EOF after root object!");
        }
    }
    
    return(result);
}

JSONObject *JSONParser::ParseObject(char *key, size_t keyLength)
{
    JSONObject *result = new JSONObject(key, keyLength);

    Token token = GetNextToken();
    bool inObject = true;
    while(inObject)
    {
        char  *key = 0;
        size_t keyLenght = 0;

        JSONValue *entry = 0;
        
        // NOTE(joshua): Key
        if(token.Type == TokenType_String)
        {
            key = token.Text;
            keyLength = token.Length;

            token = GetNextToken();
            if(token.Type != TokenType_Colon)
            {
                // TODO(joshua): Error handling!
                Assert(inObject);
                inObject = false;
            }
            else
            {
                // NOTE(joshua): Value
                token = GetNextToken();
                switch(token.Type)
                {
                    case TokenType_String:
                    {
                        entry = ParseString(token, key, keyLength);
                    } break;
                    
                    case TokenType_Identifier:
                    {
                        entry = ParseLiteral(token, key, keyLength);
                    } break;

                    case TokenType_Minus:
                    case TokenType_Number:
                    {
                        entry = ParseNumber(token, key, keyLength);
                    } break;

                    case TokenType_CurlyBracketOpen:
                    {
                        entry = ParseObject(key, keyLength);
                    } break;

                    case TokenType_SquareBracketOpen:
                    {
                        entry = ParseArray(key, keyLength);
                    } break;

                    default:
                    {
                        // TODO(joshua): Error handling!
                        Assert(!"Invalid token type");
                    } break;
                }
            }            
        }
        else if(token.Type == TokenType_CurlyBracketClose)
        {
            // NOTE(joshua): End object
            entry = 0;
            inObject = false;
        }
        else
        {
            // TODO(joshua): Error handling!
            Assert(inObject);
            inObject = false;
        }

        if(entry)
        {
            token = GetNextToken();
            if((token.Type == TokenType_Comma) || (token.Type == TokenType_CurlyBracketClose))
            {
                result->Push(entry);

                if(token.Type == TokenType_Comma)
                {
                    token = GetNextToken();
                }
            }
            else
            {
                // TODO(joshua): Error handling!
                Assert(!"Missing comma in object");
            }
        }
    }

    return(result);
}

JSONArray *JSONParser::ParseArray(char *key, size_t keyLength)
{
    JSONArray *result = new JSONArray(key, keyLength);
    size_t arrayIndex = 0;
    
    char memberKey[64];
    size_t memberKeyLength;
    
    Token token = GetNextToken();
    bool inArray = true;
    while(inArray)
    {
        JSONValue *entry = 0;
        switch(token.Type)
        {
            case TokenType_String:
            {
                memberKeyLength = FormatString(memberKey, 64, "%llu", arrayIndex++);
                entry = ParseString(token, memberKey, memberKeyLength);
            } break;

            case TokenType_Identifier:
            {
                memberKeyLength = FormatString(memberKey, 64, "%llu", arrayIndex++);
                entry = ParseLiteral(token, memberKey, memberKeyLength);
            } break;

            case TokenType_Minus:
            case TokenType_Number:
            {
                memberKeyLength = FormatString(memberKey, 64, "%llu", arrayIndex++);
                entry = ParseNumber(token, memberKey, memberKeyLength);
            } break;

            case TokenType_CurlyBracketOpen:
            {
                memberKeyLength = FormatString(memberKey, 64, "%llu", arrayIndex++);
                entry = ParseObject(memberKey, memberKeyLength);
            } break;

            case TokenType_SquareBracketOpen:
            {
                memberKeyLength = FormatString(memberKey, 64, "%llu", arrayIndex++);
                entry = ParseArray(memberKey, memberKeyLength);
            } break;

            case TokenType_SquareBracketClose:
            {
                // NOTE(joshua): End array
                entry = 0;
                inArray = false;
            } break;

            default:
            {
                // TODO(joshua): Error handling!
                Assert(!"Invalid token type");
            } break;
        }

        if(entry)
        {
            token = GetNextToken();
            if((token.Type == TokenType_Comma) || (token.Type == TokenType_SquareBracketClose))
            {
                result->Append(entry);
                
                if(token.Type == TokenType_Comma)
                {
                    token = GetNextToken();
                }
            }
            else
            {
                // TODO(joshua): Error handling!
                Assert(!"Missing comma in array");
            }
        }
    }

    return(result);
}

JSONString *JSONParser::ParseString(Token token, char *key, size_t keyLength)
{
    JSONString *result = 0;
    // TODO(joshua): Error handling!
    Assert(token.Text);
    result = new JSONString(key, keyLength, token.Text, token.Length);

    return(result);
}
    
JSONNumber *JSONParser::ParseNumber(Token token, char *key, size_t keyLength)
{
    JSONNumber *result = 0;

    bool isNegative = false;
    if(token.Type == TokenType_Minus)
    {
        isNegative = true;
        token = GetNextToken();
    }
    Assert(token.Type == TokenType_Number);

    // NOTE(joshua): Check if the number has a fractional or
    // exponential part
    char *numberString = token.Text;
    size_t numberStringLength = token.Length;
    
    bool hasFracOrExpPart = false;
    for(size_t numberStringIndex = 0;
        numberStringIndex < numberStringLength;
        ++numberStringIndex)
    {
        char at = numberString[numberStringIndex];
        if((at == '.') || (at == 'e') || (at == 'E'))
        {
            hasFracOrExpPart = true;
            break;
        }
    }

    // NOTE(joshua): Convert the number string into a double
    char *tmp = (char *)malloc((numberStringLength + 1)*sizeof(char));
    strncpy(tmp, numberString, numberStringLength);
    tmp[numberStringLength] = 0;

    if(hasFracOrExpPart)
    {
        double value = atof(tmp) * (isNegative ? -1.0 : 1.0);
        result = new JSONNumber(key, keyLength, value);
    }
    else
    {
        long long int value = 0;
        if(numberStringLength > 10)
        {
            // TODO(joshua): Error handling!
            Assert(!"Currently no support for an atoll function, stdlib only supports it from c++11 onwards!");
        }
        else
        {
            value = (long long int)atol(tmp) * (isNegative ? -1 : 1);
            result = new JSONNumber(key, keyLength, value);
        }
    }
    free(tmp);
    
    return(result);
}
    
JSONLiteral *JSONParser::ParseLiteral(Token token, char *key, size_t keyLength)
{
    JSONLiteral *result = 0;
    JSONLiteralType literalType = JSONToLiteral(token.Text, token.Length);

    if(literalType != JSONLiteral_Invalid)
    {
        result = new JSONLiteral(key, keyLength, literalType);
    }
    else
    {
        // TODO(joshua): Error handling!    
        Assert(!"Invalid literal");
    }
    
    return(result);
}
