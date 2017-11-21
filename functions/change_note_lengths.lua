local amount = -1 --the user should be prompted for this in the future

local max_inc
for _, track in pairs(song.tracks) do
  for i, note in ipairs(track) do
    next_note = i < #track and track[i + 1]
    if note.type ~= NoteType.LINEBREAK then
      old_len = note.length
      max_inc = next_note and next_note.position - note.position - old_len or amount
      note.length = old_len + math.min(max_inc, math.max(1 - old_len, amount))
    end
  end
end

log(0, "Changed note lengths by " .. amount)