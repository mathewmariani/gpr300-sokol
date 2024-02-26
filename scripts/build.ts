import fs from "fs";
import path from "path";
import Markdown from "markdown-it";
import Mustache from "mustache";
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
      const name = path.parse(file).name;
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

  // mustache templates
  let demo_mustache = fs.readFileSync("extra/mustache/demo.mustache", "utf8");

  const demo_glob = globSync("build/assignments/Release/*.js");
  for (const file of demo_glob) {
    const name = path.parse(file).name;
    console.log("found a file:", name);
    let demo_page = Mustache.render(demo_mustache, { name: name, script: `${name}.js` })

    // render .html, and copy .js
    fs.writeFile(`website/demo/${name}.html`, demo_page, (err) => { if (err) { console.error(err); } });
    fs.copyFile(file, `website/demo/${name}.js`, (err) => { if (err) { console.error(err); } });
  }

  const wasm_glob = globSync("build/assignments/Release/*.wasm");
  for (const file of wasm_glob) {
    const name = path.parse(file).base;
    console.log("found a file:", name);

    // copy js and wasm
    fs.copyFile(file, `website/demo/${name}`, (err) => { if (err) { console.error(err); } });
  }

  // copy all images to output directory
  fs.cp("course/images", "website/images", { recursive: true }, (err) => {
    if (err) {
      console.error(err);
    }
  });

  // copy assets directory
  fs.cp("assignments/assets", "website/demo/assets", { recursive: true }, (err) => {
    if (err) {
      console.error(err);
    }
  });
}

// main.js
_buildWebsite();