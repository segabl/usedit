local changes = {
  capitalize = 0,
  punctuation = 0
}

local line_start = true
local next_note, new_lyrics
for _, track in pairs(song.tracks) do
  for i, note in ipairs(track) do
    if note.type == NoteType.LINEBREAK then
      line_start = true
    elseif line_start then
      new_lyrics = note.lyrics:sub(1, 1):upper() .. note.lyrics:sub(2)
      if new_lyrics ~= note.lyrics then
        note.lyrics = new_lyrics
        changes.capitalize = changes.capitalize + 1
      end
      line_start = false
    end
    next_note = i < #track and track[i + 1]
    if (not next_note or next_note.type == NoteType.LINEBREAK) and note.lyrics:len() > 1 then
      new_lyrics = note.lyrics:gsub("[.,;]$", "")
      if new_lyrics ~= note.lyrics then
        note.lyrics = new_lyrics
        changes.punctuation = changes.punctuation + 1
      end
    end
  end
end

log(0, "Capitalized " .. changes.capitalize .. " line starts and removed " .. changes.punctuation .. " punctuations at line ends")