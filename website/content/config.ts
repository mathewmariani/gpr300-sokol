import { defineCollection, z } from 'astro:content';

const assignments = defineCollection({
	type: 'content',
	// Type-check frontmatter using a schema
	// schema: z.object({
    //     title: z.string(),
    // }),
});

const worksessions = defineCollection({
	type: 'content',
	// Type-check frontmatter using a schema
	// schema: z.object({
    //     title: z.string(),
    // }),
});

export const collections = { assignments, worksessions };