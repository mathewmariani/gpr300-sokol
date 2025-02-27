import { SYMLINK_SRC, SYMLINK_DST } from "../consts";
import type { AstroIntegration } from "astro";
import fs from 'fs';
import path from 'path';

export const symlinkIntegration = () => {
  const symlinks = [
    { source: SYMLINK_SRC, target: SYMLINK_DST },
    { source: 'assets', target: 'website/public/assets' },
  ] as const;

  let integration: AstroIntegration = {
    name: "astro-lifecycle-logs",
    hooks: {
      'astro:build:setup': async () => {
        try {
          for (const symlink of symlinks) {
            const sourcePath = path.resolve(process.cwd(), symlink.source);
            const targetPath = path.resolve(process.cwd(), symlink.target);
            if (!fs.existsSync(targetPath)) {
              fs.symlinkSync(sourcePath, targetPath, 'dir');
              console.log(`Symlink created: ${sourcePath} -> ${targetPath}`);
            } else {
              console.log(`Symlink already exists: ${targetPath}`);
            }
          }
        } catch (error: any) {
          console.log(`Failed to create symlink: ${error.message}`);
        }
      },
    },
  };

  return integration;
};

export default symlinkIntegration;
