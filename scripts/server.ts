import express from "express";
import path from "path";

const app = express();
const PORT: number = 3000;

app.use(express.static(path.join(__dirname, "../website")));
app.use("/assets", express.static(path.join(__dirname, "../website/public/assets")));
app.use("/demo", express.static(path.join(__dirname, "../website/public/demo")));

app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "../website", "index.html"));
});

app.listen(PORT, () => {
  console.log(`Server is running at http://localhost:${PORT}`);
});