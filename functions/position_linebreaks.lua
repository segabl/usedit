local floor = math.floor

local amount = 0.25 --the user should be prompted for this in the future

local changes = 0

local next_note, prev_note, old_pos
for _, track in pairs(song.tracks) do
  for i, note in ipairs(track) do
    if i > 1 and i < #track and note.type == NoteType.LINEBREAK then
      prev_note = track[i - 1]
      next_note = track[i + 1]
      old_pos = note.position
      note.position = prev_note.position + prev_note.length + floor((next_note.position - prev_note.position - prev_note.length) * amount)
      if old_pos ~= note.position then
        changes = changes + 1
      end
    end
  end
end

log(0, "Repositioned linebreaks at " .. (amount * 100) .. "% between notes")