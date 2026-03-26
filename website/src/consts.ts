export const PROJECT_NAME = 'GPR-300';
export const SITE_DESCRIPTION = '';

export const HomeLink = (): string => '/';
export const ResumeLink = (): string => '/resume';
export const ProjectsLink = (): string => '/projects';
export const CoursesLink = (): string => '/courses';

export const SYMLINKS = [
  { source: `build/web/demos`, target: `website/public/demos` },
] as const;
