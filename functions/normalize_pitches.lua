function round(x)
  local fx = math.floor(x)
  return fx + 0.5 > x and fx or math.ceil(x)
end
local abs = math.abs

local fixed = false
for _, track in pairs(song.tracks) do
  local average = 0;
  for _, note in ipairs(track) do
    average = average + note.pitch;
  end
  average = (average / #track) / 12;
  if (abs(average) > 2) then
    average = -round(average)
    for _, note in ipairs(track) do
      note.pitch = note.pitch + average * 12;
    end
    fixed = true
  end
end

log(0, fixed and "Normalized note pitches" or "No normalization needed")
