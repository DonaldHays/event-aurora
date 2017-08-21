"use strict";

const path = require("path");
const fs = require("fs");
const commander = require("commander");
const formatter = require("./formatter");

commander
  .usage("[options] <in_file> <out_file>")
  .option("-b, --bank [value]", "The output bank", "0")
  .parse(process.argv);

if(commander.args.length != 2) {
  console.error("must provide input and output files");
  process.exit(1);
}

const inputFilePath = path.resolve(commander.args[0]);
const outputFilePath = path.resolve(commander.args[1]);
const outputHeaderPath = outputFilePath.substr(0, outputFilePath.length - path.extname(outputFilePath).length) + ".h";

const noteNames = [
  ["c"],
  ["c#", "db"],
  ["d"],
  ["d#", "eb"],
  ["e"],
  ["f"],
  ["f#", "gb"],
  ["g"],
  ["g#", "ab"],
  ["a"],
  ["a#", "bb"],
  ["b"]
];

const noteIndices = {};
let octave = 2;
let noteNameIndex = 1;
let noteIndex = 1;
while(octave != 8) {
  noteNames[noteNameIndex].forEach((name) => {
    noteIndices[name + octave] = noteIndex;
  });
  noteIndex++;
  noteNameIndex++;
  if(noteNameIndex == noteNames.length) {
    noteNameIndex = 0;
    octave++;
  }
}

const inputText = fs.readFileSync(inputFilePath, "utf8");
const rawLines = inputText.split("\n");
for(let index = 0; index < rawLines.length; index++) {
  rawLines[index] = rawLines[index].trim();
}

const lines = rawLines.filter((line) => {
  if(line.length == 0) {
    return false;
  }
  
  if(line[0] == "#") {
    return false;
  }
  
  return true;
});

const song = {
  tempo : 12,
  bank : null,
  instrumentNamesAndIndices : {},
  squareInstruments : [],
  noiseInstruments : [],
  patternNamesAndIndices : {},
  patterns : [],
  chains : {}
};

function assertLength(components, length) {
  if(components.length != length) {
    console.log(`error: expected ${length} components in "${components.join(" ")}"`);
    process.exit(1);
  }
}

for(let index = 0; index < lines.length; index++) {
  const line = lines[index];
  const components = line.split(/\s+/);
  
  if(components[0] == "IT") {
    assertLength(components, 2);
    song.tempo = parseInt(components[1]);
  } else if(components[0] == "BA") {
    assertLength(components, 2);
    song.bank = parseInt(components[1]);
  } else if(components[0] == "SI") {
    assertLength(components, 11);
    
    const instrument = {
      name : components[1],
      sweepRate : parseInt(components[2]),
      sweepDirection : parseInt(components[3]),
      sweepAmount : parseInt(components[4]),
      pattern : parseInt(components[5]),
      length : parseInt(components[6]),
      initialVolume : parseInt(components[7]),
      volumeDirection : parseInt(components[8]),
      volumeAmount : parseInt(components[9]),
      flags : components[10]
    };
    
    if(song.instrumentNamesAndIndices[instrument.name] !== undefined) {
      console.log(`error: duplicate square instrument named ${instrument.name}`);
      process.exit(1);
    }
    
    song.instrumentNamesAndIndices[instrument.name] = song.squareInstruments.length;
    song.squareInstruments.push(instrument);
  } else if(components[0] == "NI") {
    assertLength(components, 10);
    
    const instrument = {
      name : components[1],
      length : parseInt(components[2]),
      initialVolume : parseInt(components[3]),
      volumeDirection : parseInt(components[4]),
      volumeAmount : parseInt(components[5]),
      clockShift : parseInt(components[6]),
      widthMode : parseInt(components[7]),
      divisorCode : parseInt(components[8]),
      flags : components[9]
    };
    
    if(song.instrumentNamesAndIndices[instrument.name] !== undefined) {
      console.log(`error: duplicate noise instrument named ${instrument.name}`);
      process.exit(1);
    }
    
    song.instrumentNamesAndIndices[instrument.name] = song.noiseInstruments.length;
    song.noiseInstruments.push(instrument);
  } else if(components[0] == "PA") {
    const pattern = {
      name : components[1],
      rows : []
    };
    for(let rowIndex = 0; rowIndex < 16; rowIndex++) {
      const rowComponents = lines[index + rowIndex + 1].split(/\s+/);
      let noteIndex = 0;
      let lowerNote = rowComponents[0].toLowerCase();
      if(lowerNote == "p") {
        noteIndex = 1;
      } else if(rowComponents[0] != "-") {
        noteIndex = noteIndices[lowerNote];
        if(noteIndex === undefined) {
          console.log(`unrecognized note ${rowComponents[0]}. Notes from C#2/Db2 to B7 are supported.`);
          process.exit(1);
        }
      }
      
      let instrumentIndex = 0;
      if(noteIndex != 0) {
        if(rowComponents[1] == "-") {
          console.log(`in patterns, any row with a note must have an instrument.`);
          process.exit(1);
        }
        
        instrumentIndex = song.instrumentNamesAndIndices[rowComponents[1]];
        
        if(instrumentIndex === undefined) {
          console.log(`unrecognized instrument ${rowComponents[1]}`);
          process.exit(1);
        }
      }
      
      let command = null;
      
      if(rowComponents[2] == "V") {
        command = {
          "type" : "vibrato",
          "time" : parseInt(rowComponents[3]),
          "step" : parseInt(rowComponents[4])
        }
      } else if(rowComponents[2] == "A") {
        command = {
          "type" : "arpeggio",
          "x" : parseInt(rowComponents[3]),
          "y" : parseInt(rowComponents[4])
        }
      } else if(rowComponents[2] == "X") {
        command = {
          "type" : "terminatephrase"
        };
      } else if(rowComponents[2] == "-") {
        
      } else {
        console.log(`unrecognized command ${rowComponents[2]}`);
        process.exit(1);
      }
      
      const patternRow = {
        note: noteIndex,
        instrument: instrumentIndex,
        command: command
      };
      
      pattern.rows.push(patternRow);
    }
    index += 16;
    
    if(song.patternNamesAndIndices[pattern.name] !== undefined) {
      console.log(`there's already a pattern named ${pattern.name}`);
      process.exit(1);
    }
    
    song.patternNamesAndIndices[pattern.name] = song.patterns.length;
    song.patterns.push(pattern);
  } else if(components[0] == "CH") {
    const name = components[1];
    if(name != "Square1" && name != "Square2" && name != "Noise") {
      console.log(`invalid channel name ${name}, must be "Square1", "Square2", or "Noise"`);
      process.exit(1);
    }
    
    if(song.chains[name] !== undefined) {
      console.log(`duplicate channel definition for ${name}`);
      process.exit(1);
    }
    
    const definition = {
      name : name,
      rows : [],
      labelNamesAndIndices : {},
      labels : []
    };
    
    if(components[2] == "R") {
      definition.infiniteRepeatLabel = components[3];
    }
    
    index++;
    while(lines[index].split(/\s+/)[0] != "EC") {
      const rowComponents = lines[index].split(/\s+/);
      const row = {};
      
      let rowIndex = 0;
      if(rowComponents[rowIndex].indexOf(":") == rowComponents[rowIndex].length - 1) {
        row.label = rowComponents[rowIndex].substring(0, rowComponents[rowIndex].length - 1);
        rowIndex++;
      }
      
      const patternName = rowComponents[rowIndex++];
      if(song.patternNamesAndIndices[patternName] === undefined) {
        console.log(`chain ${name} refers to unknown pattern ${patternName}`);
        process.exit(1);
      }
      row.patternIndex = song.patternNamesAndIndices[patternName];
      
      if(rowIndex != rowComponents.length) {
        row.repeatLabel = rowComponents[rowIndex++];
        row.repeatCount = parseInt(rowComponents[rowIndex++]);
      }
      
      if(row.label !== undefined) {
        if(definition.labelNamesAndIndices[row.label] !== undefined) {
          console.log(`duplicate chain label definition ${row.label}`);
          process.exit(1);
        }
        
        definition.labelNamesAndIndices[row.label] = definition.labels.length;
        definition.labels.push(definition.rows.length);
      }
      
      definition.rows.push(row);
      
      index++;
    }
    
    if(definition.rows.length > 64) {
      console.log(`${definition.length} chain rows in ${name} defined. Limit is 64`);
      process.exit(1);
    }
    
    if(definition.labels.length > 16) {
      console.log(`${definition.labels.length} chain labels in ${name} defined. Limit is 16`);
      process.exit(1);
    }
    
    definition.rows.forEach((row) => {
      if(row.repeatLabel !== undefined) {
        if(definition.labelNamesAndIndices[row.repeatLabel] === undefined) {
          console.log(`chain attempts jump to undefined label ${row.repeatLabel}`);
          process.exit(1);
        }
        row.repeatLabelIndex = definition.labelNamesAndIndices[row.repeatLabel];
      }
    });
    
    if(definition.infiniteRepeatLabel !== undefined) {
      if(definition.labelNamesAndIndices[definition.infiniteRepeatLabel] === undefined) {
        console.log(`chain attempts infinite repeat to undefined label ${definition.infiniteRepeatLabel}`);
        process.exit(1);
      }
      
      definition.infiniteRepeatLabelIndex = definition.labelNamesAndIndices[definition.infiniteRepeatLabel];
    }
    
    song.chains[name] = definition;
  } else {
    throw new Error(`Invalid line: ${line}`);
  }
}

if(song.squareInstruments.length > 16) {
  console.log(`${song.squareInstruments.length} square instruments defined. Limit is 16`);
  process.exit(1);
}

if(song.noiseInstruments.length > 16) {
  console.log(`${song.noiseInstruments.length} noise instruments defined. Limit is 16`);
  process.exit(1);
}

let songName = path.basename(inputFilePath);
songName = songName.substr(0, songName.indexOf("."));

function writeHeader() {
  const output = [];
  
  output.push("// IMPORTANT: Tool-generated file. Do not modify.");
  output.push("");
  
  output.push(`#ifndef music_${songName}_h`);
  output.push(`#define music_${songName}_h`);
  output.push("");
  
  output.push(`#include <gb/gb.h>`);
  output.push(`#include "../audio.h"`);
  output.push("");
  
  if(song.bank !== null) {
    output.push(`#define ${songName}Bank ${song.bank}`);
    output.push("");
  }
  
  output.push(`extern const AudioComposition ${songName};`);
  output.push("");
  
  output.push("#endif");
  output.push("");
  
  fs.writeFileSync(outputHeaderPath, output.join("\n"));
}

function writeImplementation() {
  const output = [];
  
  output.push("// IMPORTANT: Tool-generated file. Do not modify.");
  output.push("");
  
  output.push(`#include "${path.basename(outputHeaderPath)}"`);
  output.push("");
  
  if(song.bank !== null) {
    output.push(`#pragma bank ${song.bank}`);
    output.push("");
  }
  
  output.push(`const AudioComposition ${songName} = {`);
  
  output.push(`  { // Square Instruments`);
  song.squareInstruments.forEach((instrument, index) => {
    const bytes = [0, 0, 0, 0];
    
    bytes[0] |= instrument.sweepRate << 4;
    bytes[0] |= (instrument.sweepDirection == 0 ? 0x08 : 0x00);
    bytes[0] |= (instrument.sweepAmount == 0 ? 0x00 : 7 - instrument.sweepAmount);
    
    bytes[1] |= instrument.pattern << 6;
    bytes[1] |= (instrument.length == 0 ? 0x00 : 63 - instrument.length);
    
    bytes[2] |= instrument.initialVolume << 4;
    bytes[2] |= instrument.volumeDirection << 3;
    bytes[2] |= 7 - instrument.volumeAmount;
    
    if(instrument.flags.indexOf("L") != -1) {
      bytes[3] |= 0x02;
    }
    
    if(instrument.flags.indexOf("R") != -1) {
      bytes[3] |= 0x01;
    }
    
    output.push(`    { ${bytes.map((byte) => { return `0x${formatter.toHex(byte)}` }).join(", ")} }${index == song.squareInstruments.length - 1 ? "" : ","}`);
  });
  output.push(`  },`);
  
  output.push(`  { // Noise Instruments`);
  song.noiseInstruments.forEach((instrument, index) => {
    const bytes = [0, 0, 0, 0];
    
    bytes[0] |= (instrument.length == 0 ? 0x00 : 63 - instrument.length);
    
    bytes[1] |= instrument.initialVolume << 4;
    bytes[1] |= instrument.volumeDirection << 3;
    bytes[1] |= 7 - instrument.volumeAmount;
    
    bytes[2] |= instrument.clockShift << 4;
    bytes[2] |= instrument.widthMode << 3;
    bytes[2] |= 7 - instrument.divisorCode;
    
    if(instrument.flags.indexOf("L") != -1) {
      bytes[3] |= 0x02;
    }
    
    if(instrument.flags.indexOf("R") != -1) {
      bytes[3] |= 0x01;
    }
    
    output.push(`    { ${bytes.map((byte) => { return `0x${formatter.toHex(byte)}` }).join(", ")} }${index == song.noiseInstruments.length - 1 ? "" : ","}`);
  });
  output.push(`  },`);
  
  output.push(`  0x${formatter.toHex(song.tempo)}, // Tempo`);
  [song.chains["Square1"], song.chains["Square2"], song.chains["Noise"]].forEach((chain) => {
    if(chain === undefined) {
      output.push(`  { // Undefined Chain Filler`);
      output.push(`    { { 0x00 } },`);
      output.push(`    0x00,`);
      output.push(`    0x00,`);
      output.push(`    { 0x00 }`);
      output.push(`  },`);
      return;
    }
    
    output.push(`  { // ${chain.name} Chain`);
    output.push(`    { // Rows`);
    chain.rows.forEach((row, index) => {
      let repeatCommand = 0;
      
      if(row.repeatLabelIndex !== undefined) {
        repeatCommand |= 0x80;
        repeatCommand |= (row.repeatCount - 1) << 4;
        repeatCommand |= row.repeatLabelIndex;
      }
      
      output.push(`      { 0x${formatter.toHex(row.patternIndex)}, 0x${formatter.toHex(repeatCommand)} }${index == chain.rows.length - 1 ? "" : ","}`);
    });
    output.push(`    },`);
    output.push(`    0x${formatter.toHex(chain.rows.length)}, // Count`);
    if(chain.infiniteRepeatLabel !== undefined) {
      let repeatCommand = 0;
      repeatCommand |= 0x80;
      repeatCommand |= chain.infiniteRepeatLabelIndex;
      
      output.push(`    0x${formatter.toHex(repeatCommand)}, // Infinite repeat`);
    } else {
      output.push(`    0x00, // Don't infinite repeat`);
    }
    if(chain.labels.length == 0) {
      chain.labels.push(0);
    }
    output.push(`    { ${chain.labels.map((label) => { return `0x${formatter.toHex(label)}` }).join(", ")} } // Labels`);
    output.push(`  },`);
  });
  
  output.push(`  { // Patterns`);
  song.patterns.forEach((pattern, index) => {
    output.push(`    {`);
    pattern.rows.forEach((row, index) => {
      const numbers = [0, 0, 0];
      
      numbers[0] = row.note;
      numbers[1] = row.instrument;
      
      if(row.command !== null) {
        if(row.command.type == "vibrato") {
          numbers[2] |= 0xA000;
          numbers[2] |= row.command.time << 4;
          numbers[2] |= row.command.step;
        } else if(row.command.type == "arpeggio") {
          numbers[2] |= 0xB000;
          numbers[2] |= row.command.x << 4;
          numbers[2] |= row.command.y;
        } else if(row.command.type == "terminatephrase") {
          numbers[2] |= 0xC000;
        } else {
          console.log(`error: unrecognized command type ${row.command.type}`);
          process.exit(1);
        }
      }
      
      output.push(`      { 0x${formatter.toHex(numbers[0])}, 0x${formatter.toHex(numbers[1])}, 0x${formatter.toHex(numbers[2], 4)} }${index == pattern.rows.length - 1 ? "" : ","}`);
    });
    output.push(`    }${index == song.patterns.length - 1 ? "" : ","}`)
  })
  output.push(`  }`);
  
  output.push(`};`);
  
  output.push("");
  
  fs.writeFileSync(outputFilePath, output.join("\n"));
}

writeHeader();
writeImplementation();
