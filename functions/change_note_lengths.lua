local min = math.min
local max = math.max

local amount = 1 --the user should be prompted for this in the future

local changes = 0

local max_inc
for _, track in pairs(song.tracks) do
  for i, note in ipairs(track) do
    next_note = i < #track and track[i + 1]
    if note.type ~= NoteType.LINEBREAK then
      old_len = note.length
      max_inc = next_note and next_note.position - note.position - old_len or amount
      note.length = old_len + min(max_inc, max(1 - old_len, amount))
      if old_len ~= note.length then
        changes = changes + 1
      end
    end
  end
end

log(0, "Changed " .. changes .. " note lengths")