import fs from "fs";
import path from "path";
import Markdown from "markdown-it";
import Mustache from "mustache";
import hljs from "highlight.js"
import { globSync } from "glob";

// args
const args = process.argv.slice(2);

// initialize markdown
hljs.registerLanguage("cpp", require("highlight.js/lib/languages/cpp"));
hljs.registerLanguage("glsl", require("highlight.js/lib/languages/glsl"));
const md = Markdown({ html: true }).use(require('markdown-it-highlightjs'), { hljs })

// mustache partials
const page = fs.readFileSync("resources/mustache/page.mustache", "utf8");
const meta = fs.readFileSync("resources/mustache/meta.mustache", "utf8");
const header = fs.readFileSync("resources/mustache/header.mustache", "utf8");
const footer = fs.readFileSync("resources/mustache/footer.mustache", "utf8");
const demo = fs.readFileSync("resources/mustache/demo.mustache", "utf8");

const content_path = "resources/content";
const assignments_path = "resources/content/assignments";
const build_path = args.includes("debug") ? "build/assignments/Debug" : "build/assignments/Release";

// helper functions
function _copyDirectory(source: string, destination: string) {
  fs.cp(source, destination, { recursive: true, force: true }, (err) => { if (err) { console.error(`Error copying directory from ${source} to ${destination}:`, err); } });
}
function _copyFile(source: string, destination: string) {
  fs.copyFile(source, destination, (err) => { if (err) { console.error(`Error copying file from ${source} to ${destination}:`, err); } });
}
function _writeFile(destination: string, content: string) {
  fs.writeFile(destination, content, (err) => { if (err) { console.error(`Error writting file to ${destination}:`, err); } });
}

// builds a single page
function _buildPage(body: string) {
  // render markdown to html
  const view = { body: md.render(body) };
  const partials = { meta: meta, header: header, footer: footer };

  // template using mustache
  return Mustache.render(page, view, partials);
}

function _buildAssignments() {
  console.log("Building assignments...");

  // glob all .md files
  const md_glob = globSync(`${assignments_path}/*.md`);
  md_glob.forEach((file: string) => {
    const name = path.parse(file).name;
    const body = fs.readFileSync(file, "utf8");
    const page = Mustache.render(body, { demo: demo });
    const content = Mustache.render(_buildPage(page), { script: `demos/${name}.js` });

    // copy to output
    _writeFile(`website/${name}.html`, content);
  });
}

function _buildWebsite() {
  console.log("Building website...");

  // create website directory for output
  if (!fs.existsSync("website")) {
    fs.mkdirSync("website");
  }
  if (!fs.existsSync("website/demos")) {
    fs.mkdirSync("website/demos", { recursive: true });
  }
  if (!fs.existsSync("website/assets")) {
    fs.mkdirSync("website/assets", { recursive: true });
  }

  _buildAssignments();

  // glob all .md files
  const md_glob = globSync(`${content_path}/*.md`);
  md_glob.forEach((file: string) => {
    const name = path.parse(file).name;
    const body = fs.readFileSync(file, "utf8");
    const content = md.render(_buildPage(body));

    // copy to output
    _writeFile(`website/${name}.html`, content);
  });

  // glob all .wasm files
  const wasm_glob = globSync(`${build_path}/*.wasm`);
  wasm_glob.forEach((file: string) => {
    const name = path.parse(file).name;

    // copy to output
    _copyFile(`${build_path}/${name}.js`, `website/demos/${name}.js`);
    _copyFile(`${build_path}/${name}.wasm`, `website/demos/${name}.wasm`);
  });

  // copy to output
  _copyDirectory("resources/images", "website/images");
  _copyDirectory("resources/assets", "website/assets");
  _copyDirectory("assignments/assets", "website/assets");
}

// main.js
_buildWebsite();
