local CACHE_FILE = ".program_cache.txt"
local CONFIG_FILE = ".languages.txt"

-- Helper to check if a file/directory exists
local function exists(path)
    local f = io.open(path, "r")
    if f then f:close() return true end
    return false
end

-- Reads the configuration file to determine supported extensions and engines
local function load_supported_languages()
    local languages = {}
    if not exists(CONFIG_FILE) then
        -- Default fallback if config file isn't found
        languages[".py"] = "python3"
        languages[".lua"] = "lua"
        return languages
    end

    for line in io.lines(CONFIG_FILE) do
        -- Matches extension (e.g., .py) and engine (e.g., python3)
        local ext, engine = line:match("([%.%w]+)%s+(%S+)")
        if ext and engine then
            languages[ext] = engine
        end
    end
    return languages
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

-- Scans directories for folders ending with any configured suffix
local function scan_folders(supported_langs, err_obj)
    print("Scanning directories...")
    local structure = {}

    -- Build a dynamic find argument string for directories based on suffixes
    -- Example: -name '*.py' -o -name '*.lua'
    local find_dir_args = {}
    for ext, _ in pairs(supported_langs) do
        table.insert(find_dir_args, "-name '*" .. ext .. "'")
    end
    local dir_clause = table.concat(find_dir_args, " -o ")

    local p_dir = io.popen("find . -maxdepth 1 -type d \\( " .. dir_clause .. " \\) | sort")
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
            
            -- Scan files inside this directory matching any of the allowed extension types
            local p_file = io.popen("find " .. dir_path .. " -maxdepth 1 -type f \\( " .. dir_clause .. " \\) | sort")
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
local function filter_structure(raw_structure, target_ext)
    if target_ext == "all" then return raw_structure end
    
    local filtered = {}
    for char_key, dir_data in pairs(raw_structure) do
        local filtered_files = {}
        local f_idx = 1
        
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
    -- Dynamically read configured languages
    local supported_langs = load_supported_languages()

    print("=========================================")
    print("Select language filter option:")
    
    -- Dynamically generate menu options from configuration
    local menu_map = {}
    local menu_idx = 1
    for ext, _ in pairs(supported_langs) do
        print(string.format("%d. Only %s files", menu_idx, ext))
        menu_map[tostring(menu_idx)] = ext
        menu_idx = menu_idx + 1
    end
    print(string.format("%d. All scripts", menu_idx))
    menu_map[tostring(menu_idx)] = "all"

    io.write("Choice: ")
    local filter_input = io.read():gsub("%s+", "")
    
    local lang_choice = menu_map[filter_input] or "all"

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
        raw_structure = scan_folders(supported_langs, err_obj)
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
    table.sort(sorted_keys)

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
    local selection = io.read():gsub("%s+", "")
    
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

    -- Dynamically determine runtime engine based on target file extension match
    local engine = ""
    for ext, eng in pairs(supported_langs) do
        -- Escape the dot in the extension pattern matching logic
        local escaped_ext = ext:gsub("%.", "%%.")
        if target_path:match(escaped_ext .. "$") then
            engine = eng
            break
        end
    end

    if engine == "" then
        print("Error: No execution engine configured for this file type.")
        return
    end

    print("\nLaunching " .. target_path .. " and terminating launcher...")
    
    -- POSIX exec process replacement 
    os.execute("exec " .. engine .. " " .. target_path)
end

main()