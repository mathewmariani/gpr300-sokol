// @ts-check
import { defineConfig } from 'astro/config';
import { symlinkIntegration } from './website/src/integrations/symlink';

import path from 'path';

// https://astro.build/config
export default defineConfig({
    integrations: [symlinkIntegration()],
    site: 'https://www.mathewmariani.com',
    base: '/gpr300-sokol/',
    srcDir: './website/src',
    publicDir: './website/public',
    vite: {
        resolve: {
            preserveSymlinks: true,
            alias: {
                '@': path.resolve('./website/src'),
            },
        },
    },
});