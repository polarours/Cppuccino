# Third-Party Notices

Cppuccino is MIT-licensed. The following third-party assets are distributed
with the documentation site under their own licenses.

## Fonts

The site ships Latin subsets of these fonts as WOFF2 files under
`site-src/assets/fonts/`. No request is made to Google Fonts at runtime.

| Font | Upstream | License | License copy |
|------|----------|---------|--------------|
| Inter | <https://github.com/rsms/inter> | SIL Open Font License 1.1 | `site-src/assets/fonts/OFL-Inter.txt` |
| Space Grotesk | <https://github.com/floriankarsten/space-grotesk> | SIL Open Font License 1.1 | `site-src/assets/fonts/OFL-SpaceGrotesk.txt` |
| JetBrains Mono | <https://github.com/JetBrains/JetBrainsMono> | SIL Open Font License 1.1 | `site-src/assets/fonts/OFL-JetBrainsMono.txt` |

The WOFF2 files are subsets generated with `pyftsubset` and
`woff2_compress`; the unmodified upstream fonts and their license texts remain
the authoritative source.

Chinese text deliberately falls back to the reader's system CJK font
(`Noto Sans CJK SC`, `Noto Sans SC`, or the platform default) rather than
shipping a multi-megabyte webfont.

## Images

`assets/images/cppuccino-banner-*.svg`, `assets/images/value-categories.png`,
and `assets/images/cppuccino-social.svg` are original project assets.
`assets/images/cppuccino-social.png` is a rendering of the SVG.
