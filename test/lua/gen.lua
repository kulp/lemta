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

-- prevent builtins from spoiling our pristine output
local file = document:search("//File[@name='<builtin>']")
local skip = file[1]:get_attribute("id")
function get_items(kind, skip_file_id)
    return document:search("//" .. kind .. "[@file!='" .. skip_file_id .. "']")
end
local classes = get_items("Class", skip)
local structs = get_items("Struct", skip)
local enums = get_items("Enumeration", skip)

function elaborate_type(doc, id, inner)
    local t = doc:search("//*[@id='" .. id .. "']") -- must succeed
    inner = inner or ""

    for i, t in ipairs(t) do
        local name = t:name()
        local typ = t:get_attribute("type")
        local function pad(inner) return string.len(inner) > 0 and (" " .. inner) or inner end

        if name == "FundamentalType" then
            return t:get_attribute("name") .. pad(inner)
        elseif name == "Class" or name == "Struct" then
            return t:get_attribute("name") .. pad(inner)
        elseif name == "CvQualifiedType" then
            local quals = {}
            if t:get_attribute("const"   ) then quals[#quals + 1] = "const"    end
            if t:get_attribute("volatile") then quals[#quals + 1] = "volatile" end
            return elaborate_type(doc, typ, table.concat(quals, " ") .. pad(inner))
        elseif name == "ArrayType" then
            local min = tonumber(t:get_attribute("min"))
            local max = tonumber(t:get_attribute("max"))
            local extent = (max == nil) and "" or (max - min + 1)
            return elaborate_type(doc, typ, inner .. "[" .. extent .. "]")
        elseif name == "PointerType" then
            return elaborate_type(doc, typ, "*" .. inner)
        elseif name == "ReferenceType" then
            return elaborate_type(doc, typ, "&" .. inner)
        elseif name == "Typedef" or name == "ElaboratedType" then
            return elaborate_type(doc, typ, inner)
        elseif name == "Enumeration" then
            return t:get_attribute("name") .. pad(inner)
        elseif name == "FunctionType" then
            local start = elaborate_type(doc, t:get_attribute("returns")) .. " (" .. inner .. ")("
            local first = true
            for i, arg in ipairs(t:children()) do
                if not first then
                    start = start .. ", "
                end
                if arg:name() == "Ellipsis" then
                    start = start .. "..."
                else
                    start = start .. elaborate_type(doc, arg:get_attribute("type"))
                end
                first = false
            end
            return start .. ")"
        end

        return nil
    end
end

function make_c_decl(fh, class, meth)
    local class_name = class:get_attribute("name")
    fh:write(elaborate_type(document, meth:get_attribute("returns"), class_name .. "__" .. meth:get_attribute("name")))
    fh:write("(")
    fh:write(class_name .. " *")
    for k, arg in ipairs(meth:children()) do
        local typ = arg:get_attribute("type")
        fh:write(", " .. elaborate_type(document, typ))
    end
    fh:write(");\n")
end

function make_cpp_defn(fh, class, meth)
    local class_name = class:get_attribute("name")
    local method_name = meth:get_attribute("name")
    fh:write(elaborate_type(document, meth:get_attribute("returns"), class_name .. "__" .. method_name))
    fh:write("(")
    fh:write(class_name .. " *_this")
    local args = {}
    for k, arg in ipairs(meth:children()) do
        local typ = arg:get_attribute("type")
        local nonce = "_" .. k
        fh:write(", " .. elaborate_type(document, typ, nonce))
        args[k] = nonce
    end
    fh:write(")\n")
    fh:write("    { return _this->" .. method_name .. "(" .. table.concat(args, ", ") .. "); }\n")
end

for _, enum in ipairs(enums) do
    local name = enum:get_attribute("name")
    local children = enum:children()
    if #children > 0 then
        header:write("typedef enum " .. name .. " { ")
        for i, val in ipairs(children) do
            local init = val:get_attribute("init")
            local comma = ""
            if i ~= 1 then comma = ", " end
            header:write(comma .. val:get_attribute("name") .. (init and " = " .. init or ""))
        end
        header:write(" } " .. name .. ";\n")
    else
        header:write("typedef int " .. name .. ";\n")
    end
end

for i, class in ipairs(classes) do
    local name = class:get_attribute("name")
    header:write("typedef struct " .. name .. " " .. name .. ";\n")
end

for i, struct in ipairs(structs) do
    local name = struct:get_attribute("name")
    header:write("typedef struct " .. name .. " " .. name .. ";\n") -- forward declaration
    if struct.members then
        header:write("struct " .. name .. " {\n")
        for v in string.gmatch(struct.members, "%S+") do
            local fields = document:search("//Field[@id='" .. v .. "']")
            for j, field in ipairs(fields) do
                local typ = field:get_attribute("type")
                header:write("    " .. elaborate_type(document, typ, field:get_attribute("name")) .. ";\n")
            end
        end
        header:write("};\n")
    end
end

impl:write('extern "C" {\n')

for i, class in ipairs(classes) do
    if class.members then
        for v in string.gmatch(class.members, "%S+") do
            local methods = document:search("//Method[@id='" .. v .. "']")
            for j, meth in ipairs(methods) do
                make_c_decl(header, class, meth)
                make_cpp_defn(impl, class, meth)
            end
        end
    end
end

impl:write("}\n")
