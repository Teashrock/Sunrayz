local rl = require("raylib")

print(string.char(10) .. "=================================")
print("Hello, world from Lua in Sunrayz!")
print("=================================" .. string.char(10))

local colors = {
    { 255, 255, 255, 255 },
    { 230, 41, 55, 255 },
    { 245, 245, 245, 255 },
    { 0, 158, 47, 255 }
}

local color_index = 1

local start_time = rl.api.GetTime()
local end_time = 3

local showmessage = false

while not rl.api.WindowShouldClose() do
    rl.api.BeginDrawing()

    rl.api.ClearBackground(colors[color_index])
    if rl.api.GetTime() - start_time >= end_time then
        start_time = rl.api.GetTime()
        if color_index < #colors then
            color_index = color_index + 1
        else
            color_index = 1
        end
    end

    local result = rl.api.GuiButton(rl.types.rectangle(24, 24, 120, 30), "#191#Show Message")
    if result == 0 then
        result = false
    else
        result = true
    end
    if result then
        showmessage = true
    end

    if showmessage then
        local result = rl.api.GuiMessageBox(rl.types.rectangle(85, 70, 250, 100), "#191#Message Box", "Hi! This is a message!", "Nice;Cool")
        if result >= 0 then
            showmessage = false
        end
    end

    rl.api.EndDrawing()
end