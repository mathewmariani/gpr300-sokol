import { SYMLINKS } from "../consts";
import type { AstroIntegration } from "astro";
import fs from "fs";
import path from "path";

export const symlinkIntegration = (): AstroIntegration => {
  return {
    name: "astro-symlinks",
    hooks: {
      "astro:config:setup": async ({ command }) => {
        const suffix = command === "build" ? "Release" : "Debug";
        const symlinkType = process.platform === "win32" ? "junction" : "dir";

        for (const { source, target } of SYMLINKS) {
          const sourcePath = path.resolve(process.cwd(), source, suffix);
          const targetPath = path.resolve(process.cwd(), target);

          fs.mkdirSync(path.dirname(targetPath), { recursive: true });

          if (fs.existsSync(targetPath)) {
            const stat = fs.lstatSync(targetPath);
            if (stat.isSymbolicLink()) {
              fs.unlinkSync(targetPath);
            } else {
              console.log(`Destination exists and is not a symlink: ${targetPath}`);
              continue;
            }
          }

          fs.symlinkSync(sourcePath, targetPath, symlinkType);
          console.log(`Symlink created: ${sourcePath} -> ${targetPath}`);
        }
      },
    },
  };
};

export default symlinkIntegration;
