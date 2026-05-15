local CACHE_FILE = ".program_cache.txt"

-- Helper to check if a file/directory exists
local function exists(path)
    local f = io.open(path, "r")
    if f then f:close() return true end
    return false
end

-- Generates the directory characters (a-z, then A-Z)
local function get_dir_char(index)
    if index <= 26 then
        return string.char(96 + index) -- 97 is 'a'
    elseif index <= 52 then
        return string.char(64 + (index - 26)) -- 65 is 'A'
    else
        return "?" 
    end
end

-- Scans directories for folders ending with .py or .lua
local function scan_folders(err_obj)
    print("Scanning directories...")
    local structure = {}

    -- Execute popen to get directories ending in .py or .lua safely
    local p_dir = io.popen("find . -maxdepth 1 -type d \\( -name '*.py' -o -name '*.lua' \\) | sort")
    if p_dir == nil then
        err_obj.err_code = 1
        return {}
    end
    
    local dir_idx = 1
    for dir_path in p_dir:lines() do
        local dir_name = dir_path:sub(3) -- Strip the './' prefix
        if dir_name ~= "" then
            local char_key = get_dir_char(dir_idx)
            structure[char_key] = { name = dir_name, files = {} }
            
            -- Scan files inside this directory matching extension types
            local p_file = io.popen("find " .. dir_path .. " -maxdepth 1 -type f \\( -name '*.py' -o -name '*.lua' \\) | sort")
            if p_file == nil then
                err_obj.err_code = 1
                return {}
            end
            
            local file_idx = 1
            for file_path in p_file:lines() do
                structure[char_key].files[tostring(file_idx)] = file_path
                file_idx = file_idx + 1
            end
            p_file:close()
            
            dir_idx = dir_idx + 1
        end
    end
    p_dir:close()

    -- Save full data to plain text cache file
    local f = io.open(CACHE_FILE, "w")
    if f then
        for char_key, dir_data in pairs(structure) do
            for file_idx, file_path in pairs(dir_data.files) do
                f:write(char_key .. "|" .. dir_data.name .. "|" .. file_idx .. "|" .. file_path .. "\n")
            end
        end
        f:close()
    end

    return structure
end

-- Loads the text cache file and reconstructs the structure
local function load_cache()
    if not exists(CACHE_FILE) then return nil end
    
    local structure = {}
    for line in io.lines(CACHE_FILE) do
        local char_key, dir_name, file_idx, file_path = line:match("([^|]+)|([^|]+)|([^|]+)|([^|]+)")
        if char_key then
            if not structure[char_key] then
                structure[char_key] = { name = dir_name, files = {} }
            end
            structure[char_key].files[file_idx] = file_path
        end
    end
    return structure
end

-- Dynamic filtering logic based on user filter preference
local function filter_structure(raw_structure, lang_choice)
    if lang_choice == "all" then return raw_structure end
    
    local filtered = {}
    local target_ext = "." .. lang_choice -- ".py" or ".lua"
    
    for char_key, dir_data in pairs(raw_structure) do
        local filtered_files = {}
        local f_idx = 1
        
        -- Sort internal files numerically to filter them out sequentially
        local temp_keys = {}
        for k in pairs(dir_data.files) do table.insert(temp_keys, tonumber(k)) end
        table.sort(temp_keys)
        
        for _, old_idx in ipairs(temp_keys) do
            local file_path = dir_data.files[tostring(old_idx)]
            if file_path:match(target_ext .. "$") then
                filtered_files[tostring(f_idx)] = file_path
                f_idx = f_idx + 1
            end
        end
        
        -- Only add the directory if it contains matched filtered executable scripts
        local has_files = false
        for _ in pairs(filtered_files) do has_files = true break end
        
        if has_files then
            filtered[char_key] = {
                name = dir_data.name,
                files = filtered_files
            }
        end
    end
    return filtered
end

local function main()
    -- Ask the user what kind of scripts they want to handle
    print("=========================================")
    print("Select language filter option:")
    print("1. Python scripts only (.py)")
    print("2. Lua scripts only (.lua)")
    print("3. All scripts")
    io.write("Choice (1-3): ")
    local filter_input = io.read()
    
    local lang_choice = "all"
    if filter_input == "1" then lang_choice = "py"
    elseif filter_input == "2" then lang_choice = "lua" end

    local raw_structure = nil
    local err_obj = { err_code = 0 }
    
    if exists(CACHE_FILE) then
        io.write("\nCache found. Use cached list? (y/n): ")
        local choice = io.read():lower()
        if choice == "y" then
            raw_structure = load_cache()
        end
    end
    
    if not raw_structure then
        raw_structure = scan_folders(err_obj)
        if err_obj.err_code == 1 then
            io.stderr:write("\n=================== ERROR PAGE ===================\n")
            io.stderr:write("CRITICAL ERROR: Certain Directory or environment find tools not found\n")
            return
        end
    end

    -- Apply runtime file filtering mapping logic dynamically
    local structure = filter_structure(raw_structure, lang_choice)

    -- Check if empty
    local has_data = false
    for _ in pairs(structure) do has_data = true break end
    if not has_data then
        print("No valid programs matching your language filter found.")
        return
    end

    -- Sort the keys for display (a-z, A-Z)
    local sorted_keys = {}
    for k in pairs(structure) do table.insert(sorted_keys, k) end
    table.sort(sorted_keys, function(a, b)
        return a < b
    end)

    -- Display the structured menu
    print("\n=========================================")
    print("           AVAILABLE PROGRAMS (" .. lang_choice:upper() .. ")")
    print("=========================================")
    for _, char_key in ipairs(sorted_keys) do
        local dir_data = structure[char_key]
        print("\n[" .. char_key .. "] Directory: " .. dir_data.name)
        print("-----------------------------------------")
        
        local file_keys = {}
        for fk in pairs(dir_data.files) do table.insert(file_keys, tonumber(fk)) end
        table.sort(file_keys)
        
        for _, file_idx in ipairs(file_keys) do
            local s_idx = tostring(file_idx)
            local path = dir_data.files[s_idx]
            local filename = path:match("^.+/(.+)$") or path
            print("  " .. char_key .. s_idx .. " -> " .. filename)
        end
    end

    -- User selection
    io.write("\nSelect a program option (e.g., a1, B2): ")
    local selection = io.read():gsub("%s+", "") -- strip whitespaces
    
    if #selection < 2 then
        print("Invalid format input. Exiting.")
        return
    end
    
    local sel_char = selection:sub(1,1)
    local sel_num = selection:sub(2)

    if not structure[sel_char] or not structure[sel_char].files[sel_num] then
        print("Selection out of bounds or invalid code.")
        return
    end

    local target_path = structure[sel_char].files[sel_num]
    
    if not exists(target_path) then
        print("Error: Script file no longer exists at " .. target_path)
        return
    end

    -- Determine runtime engine
    local engine = "python3"
    if target_path:match("%.lua$") then
        engine = "lua"
    end

    print("\nLaunching " .. target_path .. " and terminating launcher...")
    
    -- POSIX exec process replacement 
    os.execute("exec " .. engine .. " " .. target_path)
end

main()