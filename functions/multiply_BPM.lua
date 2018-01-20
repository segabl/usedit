local amount = 0.5 --the user should be prompted for this in the future

song.bpm = song.bpm * amount
for _, track in pairs(song.tracks) do
  for _, note in ipairs(track) do
    note.position = math.floor(note.position * amount)
    note.length = math.max(1, math.floor(note.length * amount))
  end
end
song.medley_start = math.floor(song.medley_start * amount)
song.medley_end = math.floor(song.medley_end * amount)

log(0, "Changed song BPM to " .. song.bpm)