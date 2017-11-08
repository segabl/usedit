local amount = 2 --the user should be prompted for this in the future

song.bpm = song.bpm * amount
for _, track in pairs(song.tracks) do
  for _, note in ipairs(track) do
    note.position = note.position * amount
    note.length = note.length * amount
  end
end

log(0, "Changed song BPM to " .. song.bpm)