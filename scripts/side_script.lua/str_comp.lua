-- Helper to validate comma placement in Base-10 strings
local function validate_commas(str)
    -- Check if string starts or ends with a comma
    if str:sub(1,1) == "," or str:sub(-1,-1) == "," then return false end
    -- Split by commas to check chunk sizes
    local chunks = {}
    for chunk in str:gmatch("[^,]+") do
        table.insert(chunks, chunk)
    end
    
    for i, chunk in ipairs(chunks) do
        if i == 1 then
            -- First chunk can be 1-3 digits
            if #chunk < 1 or #chunk > 3 then return false end
        else
            -- All subsequent chunks MUST be exactly 3 digits
            if #chunk ~= 3 then return false end
        end
        -- Ensure only digits are in the chunk
        if not chunk:match("^%d+$") then return false end
    end
    return true
end

print(" ===================== STRING COMPARISON ===================== ")
print("Select Mode:")
print("    +) 1. Plain Text")
print("    +) 2. Numbers (Base-10 with Comma Validation)")
io.write("Enter your choice here: ")
local mode = io.read()

print("\n ---------------------------------------------- ")
io.write("Enter first input: ")
local input1 = io.read()
io.write("Enter second input: ")
local input2 = io.read()

if mode == "1" then
    -- --- PLAIN TEXT MODE ---
    print("\n ------ RESULTS - (1) PLAIN TEXT ------ ")
    print("--- Metadata ---")
    print("String 1 Length: " .. #input1)
    print("String 2 Length: " .. #input2)
    
    if input1 == input2 then
        print("RESULT: Identical")
    else
        print("RESULT: Different")
    end

elseif mode == "2" then
    -- --- NUMBER MODE ---
    local has_comma1 = input1:find(",") ~= nil
    local has_comma2 = input2:find(",") ~= nil
    
    -- Validate if commas exist
    if has_comma1 then
        if not validate_commas(input1) then
            print("ERROR: Invalid comma placement in first input.")
            return
        end
    end
    if has_comma2 then
        if not validate_commas(input2) then
            print("ERROR: Invalid comma placement in second input.")
            return
        end
    end

    -- Strip commas for comparison
    local clean1 = input1:gsub(",", "")
    local clean2 = input2:gsub(",", "")

    -- Final Digit-only check (for cases with no commas)
    if not clean1:match("^%d+$") or not clean2:match("^%d+$") then
        print("ERROR: Input contains non-numeric characters.")
        return
    end
    
    print("\n ------ RESULTS - (2) NUMERICAL STRING ------ ")
    print("----- Metadata -----")
    print("String 1 Length: " .. #clean1)
    print("String 2 Length: " .. #clean2)
    if clean1 == clean2 then
        print("RESULT: The numbers are equal.")
    else
        print("RESULT: The numbers are different.")
    end
else
    print("Invalid mode selected.")
end