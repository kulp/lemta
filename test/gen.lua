local xmlua = require("xmlua")

local xml_file = io.open("model.Model.xml")
local xml = xml_file:read("*all")
xml_file:close()

local document = xmlua.XML.parse(xml)

local classes = document:search("//Class");

function elaborate_type(doc,id,ptr)
    local t = doc:search("//*[@id='" .. id .. "']") -- must succeed
    --print("look up id " .. id .. " in doc " .. tostring(doc) .. " = " .. tostring(t))
    ptr = ptr or ""

    for i,t in ipairs(t) do
        local name = t:name()
        local typ = t:get_attribute("type")

        if name == "FundamentalType" then
            return t:get_attribute("name") .. " " .. ptr
        elseif name == "Class" or name == "Struct" then
            return t:get_attribute("name") .. " " .. ptr
        elseif name == "CvQualifiedType" then
            return elaborate_type(doc,typ,"const" .. ptr)
        elseif name == "PointerType" then
            return elaborate_type(doc,typ,"*" .. ptr)
        elseif name == "ReferenceType" then
            return elaborate_type(doc,typ,"&" .. ptr)
        elseif name == "Typedef" then
            return elaborate_type(doc,typ,ptr)
        elseif name == "FunctionType" then
            local start = elaborate_type(doc,t:get_attribute("returns")) .. "(" .. ptr .. ") ("
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

for i,val in ipairs(classes) do
    if val.members then
        for v in string.gmatch(val.members, "%S+") do
            local methods = document:search("//Method[@id='" .. v .. "']")
            for j,meth in ipairs(methods) do
                print(meth:get_attribute("name"))
                for k,arg in ipairs(meth:children()) do
                    local typ = arg:get_attribute("type")
                    --print("trying for " .. tostring(arg) .. ", type = " .. typ)
                    print(elaborate_type(document,typ))
                end
            end
        end
    end
end
