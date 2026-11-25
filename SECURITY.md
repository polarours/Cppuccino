# Security Policy

## Supported versions

Security fixes are applied to the `master` branch and to the latest tagged
release. Older snapshots are not maintained.

## Reporting a vulnerability

Please use GitHub's private vulnerability reporting:

**[Report a vulnerability](https://github.com/polarours/Cppuccino/security/advisories/new)**

Do not open a public issue for a security problem. Include:

- the affected file, workflow, or URL;
- the smallest reproduction you can provide;
- the version, commit, or date you tested;
- the impact you believe it has;
- any suggested fix, if you have one.

You can expect an initial response within seven days. This is a volunteer
project, so there is no bug bounty and no guaranteed fix deadline.

## Scope

In scope:

- the documentation site, Arcade, Camp, Labs, and their JavaScript;
- repository tooling and CI workflows;
- dependency and supply-chain issues;
- links or embeds that could execute untrusted content.

Out of scope:

- the intentional undefined behavior, segfaults, data races, and double frees
  in the teaching examples. They are the lesson, not a vulnerability.
- reports that require a compromised browser, extension, or local machine;
- social engineering or physical access.

## Disclosure

Please give the maintainer a reasonable chance to fix the issue before public
disclosure. Credit will be given in the advisory or release notes unless you
ask to remain anonymous.
