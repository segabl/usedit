local floor = math.floor
local max = math.max

local amount = 0.334 --the user should be prompted for this in the future

local changes = 0

local next_note, prev_note, old_pos, len, tlen
for _, track in pairs(song.tracks) do
  for i, note in ipairs(track) do
    if i > 1 and i < #track and note.type == NoteType.LINEBREAK then
      prev_note = track[i - 1]
      next_note = track[i + 1]
      old_pos = note.position
      len = next_note.position - prev_note.position - prev_note.length
      tlen = floor(len * amount)
      note.position = prev_note.position + prev_note.length + (tlen > 0 and tlen or floor(len * 0.5))
      if old_pos ~= note.position then
        changes = changes + 1
      end
    end
  end
end

log(0, "Repositioned linebreaks at " .. (amount * 100) .. "% between notes")