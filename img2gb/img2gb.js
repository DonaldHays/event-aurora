"use strict";

const path = require("path");
const fs = require("fs");
const commander = require("commander");

commander
  .usage("[options] <in_file> <out_file>")
  .option("-n, --name [value]", "The output variable name", "tiles")
  .parse(process.argv);

if(commander.args.length != 2) {
  console.error("must provide both an input and output file");
  process.exit(1);
}

const inputFilePath = path.resolve(commander.args[0]);
const outputFilePath = path.resolve(commander.args[1]);

const inputBasename = path.basename(inputFilePath);
const metaFilePath = path.resolve(path.dirname(inputFilePath), inputBasename.substring(0, inputBasename.length - path.extname(inputBasename).length) + ".meta.json");

const inBuffer = fs.readFileSync(inputFilePath);

const inputExtension = path.extname(inputFilePath).toLowerCase();

let imageData = null;
if(inputExtension == ".png") {
  imageData = require("./png_parser")(inBuffer);
} else {
  console.error(`Unrecognized image format "${inputExtension}". Supported formats: png`);
  process.exit(1);
}

const outputExtension = path.extname(outputFilePath).toLowerCase();
const outputOptions = {
  "name" : commander["name"]
};

if(fs.existsSync(metaFilePath)) {
  const meta = JSON.parse(fs.readFileSync(metaFilePath));
  
  if(meta["bank"] !== undefined) {
    outputOptions["bank"] = meta["bank"];
  }
  
  if(meta["size"] !== undefined) {
    outputOptions["size"] = meta["size"];
  }
  
  if(meta["frames"] !== undefined) {
    outputOptions["frames"] = [];
    outputOptions["frameNames"] = [];
    for (var name in meta["frames"]) {
      if (meta["frames"].hasOwnProperty(name)) {
        const frame = meta["frames"][name];
        outputOptions["frameNames"].push(name);
        if(meta["size"] == "2x2") {
          outputOptions["frames"].push({
            "tl" : frame["indices"]["tl"]["x"] + frame["indices"]["tl"]["y"] * imageData["width"],
            "tr" : frame["indices"]["tr"]["x"] + frame["indices"]["tr"]["y"] * imageData["width"],
            "bl" : frame["indices"]["bl"]["x"] + frame["indices"]["bl"]["y"] * imageData["width"],
            "br" : frame["indices"]["br"]["x"] + frame["indices"]["br"]["y"] * imageData["width"]
          });
        } else {
          outputOptions["frames"].push({
            "index" : frame["index"]["x"] + frame["index"]["y"] * imageData["width"]
          });
        }
      }
    }
  }
  
  if(meta["animations"] !== undefined) {
    outputOptions["animations"] = meta["animations"];
  }
  
  if(meta["count"] !== undefined) {
    const count = meta["count"];
    const byteOffset = count * 16;
    imageData["bytes"].splice(byteOffset);
  }
}

if(outputExtension == ".c") {
  require("./c_output")(outputFilePath, imageData["bytes"], outputOptions);
} else {
  console.error(`Unrecognized output format "${outputExtension}". Supported formats: c, z80, asm`);
  process.exit(1);
}
