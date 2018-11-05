/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

Tokenizer::Tokenizer(const char *fileName, unsigned int identifierComponentFlags)
{
    TextStart      = 0;
    TextAt         = 0;
    validComponent = identifierComponentFlags;

    LogMessage("Parse file: %s", fileName);
    FILE *file = fopen(fileName, "rb");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *TextStart = (char *)malloc(fileSize + 1);
        fread(TextStart, fileSize, 1, file);
        TextStart[fileSize] = 0;
        TextAt = TextStart;
            
        fclose(file);
    }
    else
    {
        LogMessage("ERROR >> Can't open file!");
    }
}
    
Tokenizer::~Tokenizer()
{
    if(TextStart) { free(TextStart); }
}

Token Tokenizer::GetNextToken()
{
    Token result = {};
        
    if(TextAt)
    {
        TextAt = SkipWhitespace(TextAt);
        
        if(*TextAt == 0) { result.Type = TokenType_EndOfStream; }
        else if(*TextAt == '[') { result.Type = TokenType_SquareBracketOpen; result.Text = TextAt++; }
        else if(*TextAt == ']') { result.Type = TokenType_SquareBracketClose; result.Text = TextAt++; }
        else if(*TextAt == '(') { result.Type = TokenType_RoundBracketOpen; result.Text = TextAt++; }
        else if(*TextAt == ')') { result.Type = TokenType_RoundBracketClose; result.Text = TextAt++; }
        else if(*TextAt == '{') { result.Type = TokenType_CurlyBracketOpen; result.Text = TextAt++; }
        else if(*TextAt == '}') { result.Type = TokenType_CurlyBracketClose; result.Text = TextAt++; }
        else if(*TextAt == '<') { result.Type = TokenType_AngleBracketOpen; result.Text = TextAt++; }
        else if(*TextAt == '>') { result.Type = TokenType_AngleBracketClose; result.Text = TextAt++; }
        else if(*TextAt == '.') { result.Type = TokenType_Period; result.Text = TextAt++; }
        else if(*TextAt == ',') { result.Type = TokenType_Comma; result.Text = TextAt++; }
        else if(*TextAt == ':') { result.Type = TokenType_Colon; result.Text = TextAt++; }
        else if(*TextAt == ';') { result.Type = TokenType_Semicolon; result.Text = TextAt++; }
        else if(IsAlpha(*TextAt))
        {
            result.Type = TokenType_Identifier;
            result.Text = TextAt++;

            size_t textLength = 1;
            while(IsAlpha(*TextAt) ||
                  (IsNumber(*TextAt) && (validComponent & IdentifierComponent_Number)) ||
                  ((*TextAt == '_') && (validComponent & IdentifierComponent_Underscore)))
            {
                ++textLength;
                ++TextAt;
            }

            result.Length = textLength;
        }
        else if(*TextAt == '\"')
        {
            TextAt++;
            result.Type = TokenType_String;
            result.Text = TextAt;
            result.Length = 0;
            while(*TextAt != '\"')
            {
                ++result.Length;
                ++TextAt;
            }
            ++TextAt;
        }
        else if(IsNumber(*TextAt))
        {
            result.Type = TokenType_Number;
            result.Text = TextAt++;

            size_t textLength = 1;
            bool wasPeriod = false;
            bool wasExponent = false;
            bool wasSign = false;
            while(IsNumber(*TextAt) || (*TextAt == '.') || (*TextAt == 'e') ||
                  (*TextAt == 'E') || (*TextAt == '-') || (*TextAt == '+'))
            {
                if(*TextAt == '.')
                {
                    if(wasPeriod)
                    {
                        LogMessage("ERROR >> Invalid number format: More than one period found!");
                        result.Type = TokenType_Undefined;
                        break;
                    }
                    wasPeriod = true;
                }

                if((*TextAt == 'e') || (*TextAt == 'E'))
                {
                    if(wasExponent)
                    {
                        LogMessage("ERROR >> Invalid number format: More than one exponent found!");
                        result.Type = TokenType_Undefined;
                        break;
                    }
                    wasExponent = true;
                }

                if((*TextAt == '-') || (*TextAt == '+'))
                {
                    if(wasSign)
                    {
                        LogMessage("ERROR >> Invalid number format: More than one sign found!");
                        result.Type = TokenType_Undefined;
                        break;
                    }
                    wasSign = true;
                }

                ++textLength;
                ++TextAt;
            }
                
            result.Length = textLength;
        }
        else if(*TextAt == '+') { result.Type = TokenType_Plus; result.Text = TextAt++; }
        else if(*TextAt == '-') { result.Type = TokenType_Minus; result.Text = TextAt++; }
        else { result.Type = TokenType_Undefined; result.Text = TextAt++; }
    }

    return(result);
}

size_t Tokenizer::StringBetweenToken(Token tokenStart, char **stringStart)
{
    size_t result = 0;
    *stringStart = TextAt;
        
    while(*tokenStart.Text != *TextAt)
    {
        ++TextAt;
        ++result;
    }
    ++TextAt;

    return(result);
}
    
bool Tokenizer::IsAlpha(char letter)
{
    bool result = (((letter >= 'a') && (letter <= 'z')) ||
                   ((letter >= 'A') && (letter <= 'Z')));
    return(result);
}

bool Tokenizer::IsNumber(char letter)
{
    bool result = ((letter >= '0') && (letter <= '9'));
    return(result);
}

char * Tokenizer::SkipWhitespace(char *at)
{
    char *result = at;
    while(at && ((*result == ASCII_CHAR_SPACE) ||
                 (*result == ASCII_CHAR_TAB) ||
                 (*result == ASCII_CHAR_LINE_FEED) ||
                 (*result == ASCII_CHAR_CARRIAGE_RETURN)))
    {
        ++result;
    }
    return(result);
}
