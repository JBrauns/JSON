import json
import random
import string

def TestIDGen():
    result = 0
    while(True):
        yield result
        result = result + 1
        
GlobalTestIDGen = TestIDGen()

class AttribResolver(object):
    def __resolver_gen__(self, text):
        at = 0

        replace = ""

        inTemplate = False
        while(True):
            while not inTemplate:
                try:
                    if(text[at:at+2] == '<('):
                        inTemplate = True
                        at += 2
                    else:
                        at += 1

                    if(at > len(text)):
                        raise IndexError
                    
                except IndexError:
                    return

            while inTemplate:
                try:
                    if(text[at:at+2] == ')>'):
                        inTemplate = False
                        at += 2
                    else:
                        replace += text[at]
                        at += 1
                except IndexError:
                    print("ERROR: No matching End found")
                    return
                    
            yield replace
            at = at
            replace = ""

    def Resolve(self, text):
        attribs = {}
        
        gen = self.__resolver_gen__(text)
        while(True):
            try:
                while(True):
                    attribName = gen.next()
                    attribs[attribName] = self.GetAttribText(attribName)
            except StopIteration:
                break

        for attrib in attribs:
            # print("Key=%s Value=%s" % ('<(%s)>' % (attrib), attribs[attrib]))
            text = str.replace(text, '<(%s)>' % (attrib), attribs[attrib])
            
        return text
            
    def GetAttrib(self, attribKey):
        result = None
        
        try:
            result = self.__getattribute__(attribKey)
        except AttributeError:
            print("ERROR: AttributeKey='%s' was not found" % (attribKey))

        return result
    
    def GetAttribText(self, attribKey):
        attribValue = self.GetAttrib(attribKey)
        
        result = ""
        if(not (attribValue is None)):
            try:
                result = self.TextFormat[type(attribValue)](attribValue)
            except AttributeError:
                print("ERROR: No format method for type=%s found" % (type(attribValue)))
            
        return result

class JsonValue(object):
    def __init__(self):
        self.TextFormat = {}
        self.TextFormat[type("")] = lambda x : ("%s" % (x))
        self.TextFormat[type(0)]  = lambda x : ("%d" % (x))

        self.TestCountName = "TestCount"
        self.CoverageCountName = "CoverageCount"
        self.FailCountName = "FailCount"
        
        self.Indent = 0
        
    def Generate(self):
        codeIndent = self.Indent*"  "
        code = []
        
        for line in self.Header():
            code.append(self.Resolve(line))
            
        for line in (self.PreTest() + self.Test() + self.PostTest()):
            code.append(self.Resolve(codeIndent + line))

        for line in self.Footer():
            code.append(self.Resolve(line))

        return code
    
    def ToJSON(self):
        data = [ self.JsonFmt.format(self) ]
        return data

    def Header(self):
        return []
    
    def PreTest(self):
        return [ "PreTest not implemented!" ]

    def Test(self):
        return [ "Test not implemented!" ]

    def PostTest(self):
        return [ "PostTest not implemented!" ]

    def Footer(self):
        return []
    
class JsonString(JsonValue, AttribResolver):
    def __init__(self, indent, key, value, testName):
        JsonValue.__init__(self)

        self.ID = GlobalTestIDGen.next()
        
        self.JsonFmt = "\"{0.Key}\" : \"{0.Value}\""
        
        self.Indent = indent
        self.TestName = testName
        
        self.Key = key
        self.Value = value
        self.Type = "JSONValue_String"
        
    def PreTest(self):
        code = [
            # "LogMessage(\"Test '<(TestName)>[<(ID)>]'\");",
            "{",
            "  JSONObject *s = root->GetFirstChild(\"<(Key)>\");",
            "  <(TestCountName)> += 3;",
            "  ++<(CoverageCountName)>;",
            "  if(!s)",
            "  {",
            "    ++<(FailCountName)>;",
            "    LogMessage(\"'<(TestName)>' failed: 'No object <(Key)> found!'\");",
            "  }",
            "  else",
            "  {"
        ]
        return code
        
    def Test(self):
        code = [
            "    ++<(CoverageCountName)>;",
            "    if(s->Type != <(Type)>)",
            "    {",
            "      ++<(FailCountName)>;",
            "      LogMessage(\"Type of '<(Key)>' expected as '<(Type)>'\");",
            "    }",
            "    ++<(CoverageCountName)>;",
            "    if(strcmp(s->String, \"<(Value)>\") != 0)",
            "    {",
            "      ++<(FailCountName)>;",
            "      LogMessage(\"Value of '<(Key)>' expected as '<(Value)>'\");",
            "    }"
        ]
        return code

    def PostTest(self):
        code = [ "  }", "}", ]
        return code
    
class JsonNumber(JsonValue, AttribResolver):
    def __init__(self, indent, key, value, testName, real=True):
        JsonValue.__init__(self)

        self.ID = GlobalTestIDGen.next()
        
        self.IsReal = real
        self.JsonFmt = "\"{0.Key}\" : {0.Value:%s}" % ("f" if self.IsReal else "d")
        
        self.Indent = indent
        self.TestName = testName
        
        self.Key = key
        self.Value = value
        self.Type = "JSONValue_Number"
        
    def PreTest(self):
        code = [
            # "LogMessage(\"Test '<(TestName)>::[<(ID)>]'\");",
            "{",
            "  JSONObject *n = root->GetFirstChild(\"<(Key)>\");",
            "  <(TestCountName)> += 3;",
            "  ++<(CoverageCountName)>;",
            "  if(!n)",
            "  {",
            "    ++<(FailCountName)>;",
            "    LogMessage(\"Test '<(TestName)>' failed\");",
            "  }",
            "  else",
            "  {"
        ]
        return code
        
    def Test(self):
        code = [
            "    ++<(CoverageCountName)>;",
            "    if(n->Type != <(Type)>)",
            "    {",
            "      ++<(FailCountName)>;",
            "      LogMessage(\"Type of '<(Key)>' expected as '<(Type)>'\");",
            "    }",
            "    ++<(CoverageCountName)>;",
            "    if(n->Number != ({0})<(Value)>)".format("double" if self.IsReal else "int"),
            "    {",
            "      ++<(FailCountName)>;",
            "      LogMessage(\"Value of '<(Key)>' expected as '<(Value)>' but is %f\", n->Number);",
            "    }"
        ]
        return code

    def PostTest(self):
        code = [ "  }", "}", ]
        return code
    
class JsonLiteral(JsonValue, AttribResolver):
    def __init__(self, indent, key, value, testName):
        JsonValue.__init__(self)

        self.ID = GlobalTestIDGen.next()
        
        self.JsonFmt = "\"{0.Key}\" : {0.Value}"
        
        self.Indent = indent
        self.TestName = testName
        
        self.Key = key
        self.Value = value
        self.Type = "JSONValue_Literal"
        if(self.Value == "false"):
            self.ValueLiteral = "JSONLiteral_False"
        elif(self.Value == "true"):
            self.ValueLiteral = "JSONLiteral_True"
        elif(self.Value == "null"):
            self.ValueLiteral = "JSONLiteral_Null"
        else:
            self.ValueLiteral = "JSONLiteral_Invalid"
        
    def PreTest(self):
        code = [
            # "LogMessage(\"Test '<(TestName)>::[<(ID)>]'\");",
            "{",
            "  JSONObject *l = root->GetFirstChild(\"<(Key)>\");",
            "  <(TestCountName)> += 3;",
            "  ++<(CoverageCountName)>;",
            "  if(!l)",
            "  {",
            "    ++<(FailCountName)>;",
            "    LogMessage(\"Test '<(TestName)>' failed\");",
            "  }",
            "  else",
            "  {"
        ]
        return code
        
    def Test(self):
        code = [
            "    ++<(CoverageCountName)>;",
            "    if(l->Type != <(Type)>)",
            "    {",
            "      ++<(FailCountName)>;",
            "      LogMessage(\"Type of '<(Key)>' expected as '<(Type)>'\");",
            "    }",
            "    ++<(CoverageCountName)>;",
            "    if(l->Literal != <(ValueLiteral)>)",
            "    {",
            "      ++<(FailCountName)>;",
            "      LogMessage(\"Value of '<(Key)>' expected as '<(Value)>' but is %d\", l->Literal);",
            "    }"
        ]
        return code

    def PostTest(self):
        code = [ "  }", "}", ]
        return code
    
class JsonRoot(JsonValue, AttribResolver):
    def __init__(self, testName):
        JsonValue.__init__(self)
        
        self.Indent = 1
        self.TestName = testName
        self.FileName = testName + ".json"

        self.root = {}

        indent = self.Indent + 1
        choices = string.ascii_uppercase + string.ascii_lowercase + string.digits + "%/()[]{}!$%&"
        for length in random.sample(range(32, 512), 200):

            selection = random.randint(0, 3)
            if(selection == 0):
                key = "rnd_str_{0}".format(str(length))
                value = ''.join(random.choice(choices) for _ in range(length))
                self.root[key] = JsonString(indent, key, value, "StringRandomLength")
            elif(selection == 1):
                key = "rnd_int_{0}".format(str(length))
                value = random.randint(-4096, 4096)
                self.root[key] = JsonNumber(indent, key, value, "IntegerRandomSize", False)
            elif(selection == 2):
                key = "rnd_dbl_{0}".format(str(length))
                value = random.randint(-4096.0, 4096.0)
                self.root[key] = JsonNumber(indent, key, value, "DoubleRandomSize", True)
            elif(selection == 3):
                key = "rnd_ltl_{0}".format(str(length))
                value = random.choice(("false", "true", "null"))
                self.root[key] = JsonLiteral(indent, key, value, "RandomLiteral")

    def ToJSON(self):
        data = []
        data.append('{')

        separator = ""
        for key in self.root:
            for line in self.root[key].ToJSON():
                data.append("{0}{1}{2}".format(self.Indent*"  ", separator, line))
                separator = ","
                
        data.append('}')
        return data
        
    def Header(self):
        code = [
            "#include \"json_test.h\"",
            "#include \"tokenizer.cpp\"",
            "#include \"json_object.cpp\"",
            "#include \"json_parser.cpp\"",
            "",
            "int main(int argCount, char** args)",
            "{",
        ]
        return code
        
    def PreTest(self):
        code = [
            "  LogMessage(\"RunTest::<(TestName)>\");",
            "  ",
            "  unsigned int <(TestCountName)> = 0;",
            "  unsigned int <(CoverageCountName)> = 0;",
            "  unsigned int <(FailCountName)> = 0;",
            "  JSONParser *parser = new JSONParser(\"<(FileName)>\");",
            "  JSONObject *root = 0;",
            "  ",
            "  <(TestCountName)> += 1;",
            "  ++<(CoverageCountName)>;",
            "  if(!parser->Parse(&root))",
            "  {",
            "    ++<(FailCountName)>;",
            "    LogMessage(\"Test 'Root retrieval' failed\");",
            "  }",
            "  if(root)",
            "  {"
        ]
        
        return code
        
    def Test(self):
        code = []
        
        for key in self.root:
            code += self.root[key].Generate()
        
        return code

    def PostTest(self):
        code = [
            "  }",
            "  LogMessage(\"Test finished: Failed=%d Coverage=%0.2f%% from total %d\",",
            "             <(FailCountName)>, ((float)<(CoverageCountName)> / (float)<(TestCountName)>) * 100.00f,",
            "             <(TestCountName)>);",
            "  ",
            "  delete root;"
        ]
        
        return code

    def Footer(self):
        code = [
            "}",
        ]
        
        return code

root = JsonRoot("test_aa")
with open("test_aa.json", "wb") as f:
    f.write("\n".join(root.ToJSON()))
with open("test_aa.cpp", "wb") as f:
    f.write("\n".join(root.Generate()))
