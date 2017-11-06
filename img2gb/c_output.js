"use strict";

const fs = require("fs");
const path = require("path");
const formatter = require("./formatter");

function processHeader(filePath, bytes, options) {
  let output = [];
  output.push("// IMPORTANT: Tool-generated file. Do not modify.");
  output.push("");
  
  output.push(`#ifndef image_${options["name"]}_h`);
  output.push(`#define image_${options["name"]}_h`);
  output.push("");
  
  output.push(`#include <gb/gb.h>`);
  output.push(`#include "../sprites.h"`);
  output.push(``);
  
  if(options["bank"] !== undefined) {
    output.push(`#define ${options["name"]}Bank ${options["bank"]}`);
    output.push("");
  }
  
  if(options["frames"] !== undefined) {
    output.push(`/** Frame definitions. */`);
    output.push(`extern const SpriteFrame2x2 ${options["name"]}Frames[];`);
    output.push(``);
  }
  
  if(options["animations"] !== undefined) {
    output.push(`/** Animation definitions. */`);
    for (const key in options["animations"]) {
      if (options["animations"].hasOwnProperty(key)) {
        output.push(`extern const SpriteAnimation ${options["name"]}Animation_${key};`);
      }
    }
    output.push(``);
  }
  
  output.push(`/** Game Boy-format tile data. */`);
  output.push(`extern const GBUInt8 ${options["name"]}[];`);
  output.push("");
  
  output.push(`/** The number of tiles in \`${options["name"]}\`. */`);
  output.push(`#define ${options["name"]}Count ${bytes.length / 16 }`);
  output.push("");
  
  output.push(`/** The number of bytes in \`${options["name"]}\`. */`);
  output.push(`#define ${options["name"]}Length ${bytes.length }`);
  output.push("");
  
  output.push("#endif");
  output.push("");
  
  output = output.join("\n");
  fs.writeFileSync(filePath, output);
}

function processImplementation(filePath, bytes, options) {
  let output = [];
  output.push("// IMPORTANT: Tool-generated file. Do not modify.");
  output.push("");
  
  const fileName = path.basename(filePath);
  output.push(`#include "${fileName.substr(0, fileName.length - path.extname(fileName).length) + ".h"}"`);
  output.push("");
  
  if(options["bank"] !== undefined) {
    output.push(`#pragma bank ${options["bank"]}`);
    output.push("");
  }
  
  if(options["frames"] !== undefined) {
    output.push(`const SpriteFrame2x2 ${options["name"]}Frames[] = {`);
    options["frames"].forEach((frame, index) => {
      const bytes = [frame["tl"], frame["tr"], frame["bl"], frame["br"]];
      output.push(`  { ${bytes.map((byte) => {return "0x" + formatter.toHex(byte)}).join(", ")} }${index == options["frames"].length - 1 ? "" : ","}`);
    });
    output.push(`};`)
    output.push(``);
  }
  
  if(options["animations"] !== undefined) {
    for (const key in options["animations"]) {
      if (options["animations"].hasOwnProperty(key)) {
        const animation = options["animations"][key];
        output.push(`const SpriteAnimationFrame ${options["name"]}AnimationFrames_${key}[] = {`);
        animation["frames"].forEach((frame, index) => {
          output.push(`  { ${options["frameNames"].indexOf(frame["frame"])}, ${frame["duration"]} }${index == animation["frames"].length - 1 ? "" : ","}`);
        });
        output.push(`};`);
        output.push(``);
        
        output.push(`const SpriteAnimation ${options["name"]}Animation_${key} = {`);
        if(animation["next"]) {
          output.push(`  &${options["name"]}Animation_${animation["next"]},`)
        } else {
          output.push(`  0,`);
        }
        output.push(`  ${animation["frames"].length},`);
        output.push(`  ${options["name"]}AnimationFrames_${key}`);
        output.push(`};`);
        output.push(``);
      }
    }
  }
  
  output.push(`const GBUInt8 ${options["name"]}[] = {`);
  let index = 0;
  while(index < bytes.length) {
    let elements = [];
    for(let counter=0; counter<16 && index < bytes.length; counter++, index++) {
      elements.push("0x" + formatter.toHex(bytes[index]));
    }
    output.push(`  ${elements.join(",")}${index == bytes.length ? "" : ","}`);
  }
  output.push("};");
  output.push("");
  
  output = output.join("\n");
  fs.writeFileSync(filePath, output);
}

function process(filePath, bytes, options) {
  const headerFilePath = filePath.substr(0, filePath.length - path.extname(filePath).length) + ".h";
  
  processHeader(headerFilePath, bytes, options);
  processImplementation(filePath, bytes, options);
}

module.exports = process;
