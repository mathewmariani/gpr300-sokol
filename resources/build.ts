import fs from "fs";
import path from "path";
import Markdown from "markdown-it";
import Mustache from "mustache";
import hljs from "highlight.js"
import { globSync } from "glob";

hljs.registerLanguage("cpp", require("highlight.js/lib/languages/cpp"));
hljs.registerLanguage("glsl", require("highlight.js/lib/languages/glsl"));
const md = Markdown({ html: true }).use(require('markdown-it-highlightjs'), { hljs })

// mustache partials
const page = fs.readFileSync("resources/mustache/page.mustache", "utf8");
const meta = fs.readFileSync("resources/mustache/meta.mustache", "utf8");
const header = fs.readFileSync("resources/mustache/header.mustache", "utf8");
const footer = fs.readFileSync("resources/mustache/footer.mustache", "utf8");
const demo = fs.readFileSync("resources/mustache/demo.mustache", "utf8");

// builds a single page
function _buildPage(body: string) {
  // render markdown to html
  const view = { body: md.render(body) };
  const partials = { meta: meta, header: header, footer: footer };

  // template using mustache
  return Mustache.render(page, view, partials);
}

function _copyDirectory(source: string, destination: string) {
  fs.cp(source, destination, { recursive: true }, (err) => {
    if (err) {
      console.error(`Error copying from ${source} to ${destination}:`, err);
    }
  });
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
  const md_glob = globSync("resources/content/*.md");
  md_glob.forEach((file: string) => {
    try {
      const name = path.parse(file).name;
      const body = fs.readFileSync(file, "utf8");
      const page = _buildPage(body);
      const content = md.render(page);

      fs.writeFile(`website/${name}.html`, content, (err) => {
        if (err) {
          console.error("Error writing file:", err);
        }
      });
    } catch (err) {
      console.error("Error processing file:", err);
    }
  });

  // glob all .wasm files
  const wasm_glob = globSync("build/assignments/Debug/*.wasm");
  wasm_glob.forEach((file: string) => {
    const name = path.parse(file).name;
    let demo_page = Mustache.render(demo, { name: name, script: `${name}.js` })

    // render .html, and copy .js .wasm
    fs.writeFile(`website/demo/${name}.html`, demo_page, (err) => { if (err) { console.error("Error writing file:", err); } });
    fs.copyFile(`build/assignments/Debug/${name}.js`, `website/demo/${name}.js`, (err) => { if (err) { console.error("Error copying file:", err); } });
    fs.copyFile(`build/assignments/Debug/${name}.wasm`, `website/demo/${name}.wasm`, (err) => { if (err) { console.error("Error copying file:", err); } });
  });

  // copy assets to output
  _copyDirectory("resources/images", "website/images");
  _copyDirectory("resources/assets", "website/assets");
  _copyDirectory("assignments/assets", "website/demo/assets");
}

// main.js
_buildWebsite();
