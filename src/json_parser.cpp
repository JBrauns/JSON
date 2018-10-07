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
        
    Token token = GetNextToken();
    while((token.Type != TokenType_Undefined) || (token.Type != TokenType_EndOfStream))
    {
        if(token.Type == TokenType_EndOfStream)
        {
            break;
        }
        else if(token.Type == TokenType_Undefined)
        {
            result = false;
            LogMessage("ERROR >> Undefined token type!");
            break;
        }
        else if(token.Type == TokenType_DoubleQuotationMark)
        {
            char *key = ParseString(token);

            token = GetNextToken();
            if(token.Type == TokenType_Colon)
            {
                JSONObject *object = new JSONObject(key);
                    
                token = GetNextToken();
                if(token.Type == TokenType_DoubleQuotationMark)
                {
                    object->Type   = JSONValue_String;
                    object->String = ParseString(token);
                    parent->InsertChild(object);
                }
                else if(token.Type == TokenType_Number)
                {
                    char *number = 0;
                    unsigned int numberLength = token.Length;
                    number = (char *)malloc(numberLength + 1);
                    strncpy(number, token.Text, numberLength);
                    number[numberLength] = 0;
                        
                    object->Number = atof(number);
                    object->Type   = JSONValue_Number;

                    free(number);
                    parent->InsertChild(object);
                }
                else if(token.Type == TokenType_SquareBracketOpen)
                {
                    object->Type = JSONValue_Array;
                    Parse(object);
                    parent->InsertChild(object);
                }
                else if(token.Type == TokenType_CurlyBracketOpen)
                {
                    LogMessage("Add an object!");
                }
            }
            else
            {
                LogMessage("ERROR >> JSON Object must be key and value pair!");
            }
        }
        else if(token.Type == TokenType_SquareBracketClose)
        {
            break;
        }
        else if(token.Type == TokenType_CurlyBracketClose)
        {
            break;
        }
        else
        {
            LogMessage("TokenType: %d", token.Type);
        }
            
        token = GetNextToken();
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
