// https://flatuicolors.com/palette/ca
const palette = ["#ff9ff3", "#feca57", "#ff6b6b", "#48dbfb", "#1dd1a1"];
const hearts = ["🩷", "💛", "❤️", "🩵", "💚"];

window.addEventListener("mousedown", (e) => {
  const color = palette.shift();
  document.documentElement.style.setProperty("--highlight-background", color);
  palette.push(color);
});

window.addEventListener("DOMContentLoaded", () => {
  const index = Math.floor(Math.random() * palette.length);
  const color = palette[index];
  document.documentElement.style.setProperty("--highlight-color", color);

  const heart = hearts[index];
  document.getElementById("with-love").textContent = heart;
});