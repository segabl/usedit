local amount = 0.035 --the user should be prompted for this in the future

local margin = 0.005

for _, track in pairs(song.tracks) do

  local golden_notes = {}
  local normal_notes = {}
  local total_weight = 0
  local perc = 0

  for _, note in ipairs(track) do
    if note.type == NoteType.GOLD then
      table.insert(golden_notes, note)
    elseif note.type == NoteType.DEFAULT then
      local weight = note.length * 20 + note.pitch + 24
      table.insert(normal_notes, {note = note, weight = weight})
      total_weight = total_weight + weight
    end
  end
  
  local total = #golden_notes + #normal_notes
  local percentage = #golden_notes / total
  
  local msg = "Golden note target is " .. string.format("%.1f", amount * 100) .. "% (+/- " .. string.format("%.1f", margin * 100) .. "%), "
  if math.abs(amount - percentage) > margin then
    msg = msg .. "changed percentage from " .. string.format("%.1f", percentage * 100) .. "% to "
    if percentage > amount then
      while percentage > amount + margin do
        local index = math.random(#golden_notes)
        local note = golden_notes[index]
        table.remove(golden_notes, index)
        note.type = NoteType.DEFAULT
        percentage = #golden_notes / total
      end
    else
      while percentage < amount do
        local roll = math.random(total_weight)
        local i = 0
        while roll > 0 and i < #normal_notes do
          i = i + 1
          roll = roll - normal_notes[i].weight
        end
        local note = normal_notes[i].note
        total_weight = total_weight - normal_notes[i].weight
        table.remove(normal_notes, i)
        table.insert(golden_notes, note)
        note.type = NoteType.GOLD
        percentage = #golden_notes / total
      end
    end
    msg = msg .. string.format("%.1f", percentage * 100) .. "%"
  else
    msg = msg .. string.format("%.1f", percentage * 100) .. "% is in acceptable range"
  end
  
  log(0, msg)
  
end