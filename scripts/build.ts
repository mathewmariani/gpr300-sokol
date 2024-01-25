import fs from "fs";
import path from "path";
import Markdown from "markdown-it";
import { globSync } from "glob";

function _buildWebsite() {
  console.log("Building website...");

  // create website directory for output
  if (!fs.existsSync("website")) {
    fs.mkdirSync("website");
  }
  if (!fs.existsSync("website/demo")) {
    fs.mkdirSync("website/demo", { recursive: true });
  }

  // glob all .md files
  const md_glob = globSync("course/*.md");
  for (const file of md_glob) {
    console.log("found a .md file:", file);
    try {
      const name = path.basename(file, ".md");
      const body = fs.readFileSync(file, "utf8");
      fs.writeFile(`website/${name}.html`, Markdown({ html: true }).render(body), (err) => {
        if (err) {
          console.error(err);
        }
      });
    } catch (err) {
      console.error(err);
    }
  }

  // glob demo
  const demo_glob = globSync("build/assignments/Release/*.{html,js,wasm}");
  for (const file of demo_glob) {
    const name = path.basename(file);
    console.log("found a file:", name);
    fs.copyFile(file, `website/demo/${name}`, (err) => {
      if (err) {
        console.error(err);
      }
    });
  }

  // copy assets directory
  fs.cp("assignments/assets", "website/demo/assets", { recursive: true }, (err) => {
    if (err) {
      console.error(err);
    }
  });
}

// main.js
_buildWebsite();