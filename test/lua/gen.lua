local xmlua = require("xmlua")

local xml_in = arg[1]
local header_out = arg[2]
local impl_out = arg[3]

local xml_file = io.open(xml_in)
local xml = xml_file:read("*all")
xml_file:close()

local document = xmlua.XML.parse(xml)

local header = io.open(header_out, "w")
local impl = io.open(impl_out, "w")

local classes = document:search("//Class");
local structs = document:search('//Struct');
local enums = document:search('//Enumeration');

function elaborate_type(doc,id,inner)
    local t = doc:search("//*[@id='" .. id .. "']") -- must succeed
    inner = inner or ""

    for i,t in ipairs(t) do
        local name = t:name()
        local typ = t:get_attribute("type")

        if name == "FundamentalType" then
            return t:get_attribute("name") .. " " .. inner
        elseif name == "Class" or name == "Struct" then
            return t:get_attribute("name") .. " " .. inner
        elseif name == "CvQualifiedType" then
            return elaborate_type(doc,typ,"const" .. inner)
        elseif name == "PointerType" then
            return elaborate_type(doc,typ,"*" .. inner)
        elseif name == "ReferenceType" then
            return elaborate_type(doc,typ,"&" .. inner)
        elseif name == "Typedef" then
            return elaborate_type(doc,typ,inner)
        elseif name == "Enumeration" then
            return t:get_attribute("name") .. " " .. inner
        elseif name == "FunctionType" then
            local start = elaborate_type(doc,t:get_attribute("returns")) .. "(" .. inner .. ") ("
            local first = true
            for i, arg in ipairs(t:children()) do
                if not first then
                    start = start .. ", "
                end
                if arg:name() == "Ellipsis" then
                    start = start .. "..."
                else
                    start = start .. elaborate_type(doc,arg:get_attribute("type"))
                end
                first = false
            end
            return start .. ")"
        end

        return nil
    end
end

function make_c_decl(fh,class,meth)
    local class_name = class:get_attribute("name")
    fh:write(elaborate_type(document,meth:get_attribute("returns")))
    fh:write(class_name .. "__" .. meth:get_attribute("name"))
    fh:write("(")
    fh:write(class_name .. "*")
    for k,arg in ipairs(meth:children()) do
        local typ = arg:get_attribute("type")
        fh:write(", " .. elaborate_type(document,typ))
    end
    fh:write(");\n")
end

function make_cpp_defn(fh,class,meth)
    local class_name = class:get_attribute("name")
    local method_name = meth:get_attribute("name")
    fh:write('extern "C" ')
    fh:write(elaborate_type(document,meth:get_attribute("returns")))
    fh:write(class_name .. "__" .. method_name)
    fh:write("(")
    fh:write(class_name .. "* _this")
    local args = {}
    for k,arg in ipairs(meth:children()) do
        local typ = arg:get_attribute("type")
        local nonce = "_" .. k
        fh:write(", " .. elaborate_type(document,typ,nonce))
        args[k] = nonce
    end
    fh:write(")\n")
    fh:write("    { return _this->" .. method_name .. "(" .. table.concat(args,", ") .. "); }\n")
end

for i,class in ipairs(classes) do
    local name = class:get_attribute("name")
    header:write("typedef struct " .. name .. " " .. name .. ";\n")
end

for i,class in ipairs(structs) do
    local name = class:get_attribute("name")
    header:write("typedef struct " .. name .. " " .. name .. ";\n")
end

for _,enum in ipairs(enums) do
    local name = enum:get_attribute("name")
    local children = enum:children()
    header:write("typedef enum " .. name .. (#children > 0 and " { " or " "))
    for _,val in ipairs(children) do
        local init = val:get_attribute("init")
        header:write(val:get_attribute("name") .. (init and " = " .. init or "") .. ", ")
    end
    header:write((#children > 0 and "} " or "") .. name .. ";\n")
end

for i,class in ipairs(classes) do
    if class.members then
        for v in string.gmatch(class.members, "%S+") do
            local methods = document:search("//Method[@id='" .. v .. "']")
            for j,meth in ipairs(methods) do
                make_c_decl(header,class,meth)
                make_cpp_defn(impl,class,meth)
            end
        end
    end
end
