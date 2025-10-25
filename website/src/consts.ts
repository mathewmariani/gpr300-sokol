export const SITE_TITLE = 'GPR-300';
export const SITE_DESCRIPTION = '';

const folder = import.meta.env.PROD ? 'Release' : 'Debug';
export const SYMLINKS = [
  { source: `build/demos`, target: `website/public/demos` },
] as const;
