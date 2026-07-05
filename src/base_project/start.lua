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

local start_time = rl.GetTime()
local end_time = 3

while not rl.WindowShouldClose() do
    rl.BeginDrawing()

    rl.ClearBackground(colors[color_index])
    if rl.GetTime() - start_time >= end_time then
        start_time = rl.GetTime()
        if color_index < #colors then
            color_index = color_index + 1
        else
            color_index = 1
        end
    end

    rl.EndDrawing()
end