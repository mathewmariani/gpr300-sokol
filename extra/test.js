const fs = require("fs");

// args
const args = process.argv.slice(2);

// string constants
const build_path = args.includes("debug") ? "build/assignments/Debug" : "build/assignments/Release";
const public_path = "public/assignments";

// helper functions
function _copyDirectory(source, destination) {
  fs.cp(source, destination, { recursive: true, force: true }, (err) => {
    if (err) {
      console.error(`Error copying directory from ${source} to ${destination}:`, err);
    }
  });
}

function _copyFile(source, destination) {
  fs.copyFile(source, destination, (err) => {
    if (err) {
      console.error(`Error copying file from ${source} to ${destination}:`, err);
    }
  });
}

function _writeFile(destination, content) {
  fs.writeFile(destination, content, (err) => {
    if (err) {
      console.error(`Error writing file to ${destination}:`, err);
    }
  });
}

function _buildWebsite() {
  console.log("Building public...");

  // create website directory for output
  if (!fs.existsSync("public")) {
    fs.mkdirSync("public");
  }

  // copy to output
  _copyDirectory(build_path, public_path);
}

// main.js
_buildWebsite();
