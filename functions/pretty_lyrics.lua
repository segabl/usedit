local line_start = true
local next_note
for _, track in pairs(song.tracks) do
  for i, note in ipairs(track) do
    note.lyrics = note.lyrics:gsub("[´`]", "'")
    note.lyrics = note.lyrics:gsub("(%S)%s~", "%1~")
    note.lyrics = note.lyrics:gsub("~%s(%S)", "~%1")
    if note.type == NoteType.LINEBREAK then
      line_start = true
    elseif line_start then
      note.lyrics = note.lyrics:gsub("^[.,; ]+", "")
      note.lyrics = note.lyrics:sub(1, 1):upper() .. note.lyrics:sub(2)
      line_start = false
    end
    next_note = i < #track and track[i + 1]
    if (not next_note or next_note.type == NoteType.LINEBREAK) and note.lyrics:len() > 1 then
      note.lyrics = note.lyrics:gsub("[.,; ]+$", "")
    end
  end
end

log(0, "Capitalized line starts and removed punctuations at line ends")