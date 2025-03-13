// https://flatuicolors.com/palette/ca
const palette = ["#ff9ff3", "#feca57", "#ff6b6b", "#48dbfb", "#1dd1a1"];
window.addEventListener("mousedown", (e) => {
  const color = palette.shift();
  document.documentElement.style.setProperty("--highlight-background", color);
  palette.push(color);
});

window.addEventListener("DOMContentLoaded", () => {
  const color = palette[Math.floor(Math.random() * palette.length)];
  document.documentElement.style.setProperty("--highlight-color", color);
});