"use strict";

const path = require("path");
const fs = require("fs");
const commander = require("commander");
const PNG = require("pngjs").PNG;
const formatter = require("./formatter");

commander
  .usage("[options] <in_file> <img_file> <out_file>")
  .option("-b, --bank [value]", "The output bank", "0")
  .parse(process.argv);

if(commander.args.length != 3) {
  console.error("must provide input, image, and output files");
  process.exit(1);
}

const metatiles = [];

const inputFilePath = path.resolve(commander.args[0]);
const inputFileJSONPath = inputFilePath.substr(0, inputFilePath.length - path.extname(inputFilePath).length) + ".json";
const imageFilePath = path.resolve(commander.args[1]);
const outputFilePath = path.resolve(commander.args[2]);
const outputHeaderPath = outputFilePath.substr(0, outputFilePath.length - path.extname(outputFilePath).length) + ".h";
const name = (() => {
  const base = path.basename(inputFilePath);
  const withoutExtension = base.substr(0, base.length - path.extname(base).length);
  const name = withoutExtension[0].toLowerCase() + withoutExtension.substr(1);
  return name;
})();

const tilesPNG = PNG.sync.read(fs.readFileSync(imageFilePath));
const tiles = [];

if(tilesPNG.width % 8 != 0 || tilesPNG.height % 8 != 0) {
  console.error("image png size must be multiple of 8 in both width and height.");
  process.exit(1);
}

const tilesWide = tilesPNG.width / 8;
const tilesHigh = tilesPNG.height / 8;

for(let tileIndexY = 0; tileIndexY < tilesHigh; tileIndexY++) {
  for(let tileIndexX = 0; tileIndexX < tilesWide; tileIndexX++) {
    const tile = [];
    for(let y = tileIndexY * 8; y < tileIndexY * 8 + 8; y++) {
      for(let x = tileIndexX * 8; x < tileIndexX * 8 + 8; x++) {
        const index = (x + y * tilesPNG.width) * 4;
        const average = (tilesPNG.data[index] + tilesPNG.data[index + 1] + tilesPNG.data[index + 2]) / 3;
        const bucket = 3 - Math.floor(average / 64);
        tile.push(bucket);
      }
    }
    tiles.push(tile.join(""));
  }
}

const metatilesJSON = JSON.parse(fs.readFileSync(inputFileJSONPath));
const metaTilesPNG = PNG.sync.read(fs.readFileSync(inputFilePath));

if(metaTilesPNG.width % 16 != 0 || metaTilesPNG.height % 16 != 0) {
  console.error("input png size must be multiple of 16 in both width and height.");
  process.exit(1);
}

const metatilesWide = metaTilesPNG.width / 16;
const metatilesHigh = metaTilesPNG.height / 16;

const bank = metatilesJSON["properties"]["Bank"] || 0;

for(let metatileIndexY = 0; metatileIndexY < metatilesHigh; metatileIndexY++) {
  for(let metatileIndexX = 0; metatileIndexX < metatilesWide; metatileIndexX++) {
    const metatileIndex = metatileIndexX + metatileIndexY * metatilesWide;
    const metatile = {
      "indices" : []
    };
    for(let metatileSubIndexY = 0; metatileSubIndexY < 2; metatileSubIndexY++) {
      for(let metatileSubIndexX = 0; metatileSubIndexX < 2; metatileSubIndexX++) {
        const tile = [];
        for(let y = metatileIndexY * 16 + metatileSubIndexY * 8; y < metatileIndexY * 16 + metatileSubIndexY * 8 + 8; y++) {
          for(let x = metatileIndexX * 16 + metatileSubIndexX * 8; x < metatileIndexX * 16 + metatileSubIndexX * 8 + 8; x++) {
            const index = (x + y * metaTilesPNG.width) * 4;
            const average = (metaTilesPNG.data[index] + metaTilesPNG.data[index + 1] + metaTilesPNG.data[index + 2]) / 3;
            const bucket = 3 - Math.floor(average / 64);
            tile.push(bucket);
          }
        }
        const tileString = tile.join("");
        
        let foundTile = false;
        for(let testIndex = 0; testIndex < tiles.length; testIndex++) {
          if(tileString == tiles[testIndex]) {
            foundTile = true;
            metatile["indices"].push(testIndex);
            break;
          }
        }
        
        if(foundTile == false) {
          console.error(`Metatile (${metatileIndexX}, ${metatileIndexY}) subtile (${metatileSubIndexX}, ${metatileSubIndexY}) not found in image.`);
          process.exit(1);
        }
      }
    }
    const properties = metatilesJSON["tileproperties"][`${metatileIndex}`];
    metatile["properties"] = properties;
    metatiles.push(metatile);
  }
}

function writeHeader() {
  let output = [];
  output.push("// IMPORTANT: Tool-generated file. Do not modify.");
  output.push(``);
  
  output.push(`#ifndef metatiles_${name}_h`);
  output.push(`#define metatiles_${name}_h`);
  output.push(``);
  
  output.push(`#include <gb/gb.h>`);
  output.push(`#include "metatiles.h"`);
  output.push(``);
  
  output.push(`#define ${name}Bank ${bank}`);
  output.push(``);
  
  output.push(`extern MetatileIndices ${name}Indices;`);
  output.push(`extern MetatileAttributes ${name}Attributes;`);
  output.push(``);
  
  output.push(`#endif`);
  output.push(``);
  
  output = output.join("\n");
  console.log(output);
}

function writeImplementation() {
  
}

writeHeader();
writeImplementation();
