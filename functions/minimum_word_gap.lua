local min = math.min
local max = math.max

local amount = 1 --the user should be prompted for this in the future

local changes = 0

local next_note, old_len
for _, track in pairs(song.tracks) do
  for i, note in ipairs(track) do
    if i == #track then
      break
    end
    next_note = track[i + 1]
    if note.type ~= NoteType.LINEBREAK and next_note.type ~= NoteType.LINEBREAK then
      if note.lyrics:sub(note.lyrics:len()) == " " or next_note.lyrics:sub(1, 1) == " " then
        old_len = note.length
        note.length = old_len - math.max(0, math.min(old_len - 1, amount - (next_note.position - note.position - old_len)))
        if old_len ~= note.length then
          changes = changes + 1
        end
      end
    end
  end
end

log(0, "Changed " .. changes .. " note lengths")