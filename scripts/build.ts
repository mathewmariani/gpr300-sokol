import fs from "fs";
import path from "path";
import Markdown from "markdown-it";
import Mustache from "mustache";
import { globSync } from "glob";

// mustache partials
const page = fs.readFileSync("extra/mustache/page.mustache", "utf8");
const header = fs.readFileSync("extra/mustache/header.mustache", "utf8");
const footer = fs.readFileSync("extra/mustache/footer.mustache", "utf8");
const demo = fs.readFileSync("extra/mustache/demo.mustache", "utf8");

// builds a single page
function _buildPage(body: string) {
  console.log("Building page...");

  // render markdown to html
  const md = Markdown({ html: true });
  const view = { body: md.render(body) };
  const partials = { header: header, footer: footer };

  // template using mustache
  return Mustache.render(page, view, partials)
}

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
      const page = _buildPage(body);
      fs.writeFile(`website/${name}.html`, Markdown({ html: true }).render(page), (err) => {
        if (err) {
          console.error(err);
        }
      });
    } catch (err) {
      console.error(err);
    }
  }

  // glob all .js files
  const js_glob = globSync("build/assignments/Release/*.js");
  for (const file of js_glob) {
    const name = path.parse(file).name;
    console.log("found a file:", name);
    let demo_page = Mustache.render(demo, { name: name, script: `${name}.js` })

    // render .html, and copy .js
    fs.writeFile(`website/demo/${name}.html`, demo_page, (err) => { if (err) { console.error(err); } });
    fs.copyFile(file, `website/demo/${name}.js`, (err) => { if (err) { console.error(err); } });
  }

  // glob all .wasm files
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