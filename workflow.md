# Git workflow for contributors (fork-based)

# This document explains how to contribute to the `SolarCar` repository using forks
# and feature branches.
# The goal: clean history, predictable workflow, and a single source of truth
# in the main repo.

# ---------------------------------------------------------------------------
# 1. Roles and repos
# ---------------------------------------------------------------------------

# Main repository (canonical source of truth):
#   https://github.com/tiberiutudorascu/SolarCar
#
# Each contributor:
#   - has their own fork: https://github.com/<your-username>/SolarCar
#   - works only in their fork and local clone
#   - contributes via Pull Requests (PRs) back into the main repo
#
# Only the maintainer(s) of the main repo merge into `main`.

# ---------------------------------------------------------------------------
# 2. One-time setup
# ---------------------------------------------------------------------------

# 2.1. Fork the main repo
#
# 1) Go to: https://github.com/tiberiutudorascu/SolarCar
# 2) Click "Fork"
# 3) GitHub creates: https://github.com/<your-username>/SolarCar

# 2.2. Clone your fork locally
git clone https://github.com/<your-username>/SolarCar.git
cd SolarCar

# 2.3. Add upstream remote (main repo)
git remote add upstream https://github.com/tiberiutudorascu/SolarCar.git
git remote -v
# You should see:
#   origin   -> your fork
#   upstream -> main repository

# ---------------------------------------------------------------------------
# 3. Branch naming
# ---------------------------------------------------------------------------

# All branches should be lowercase, with words separated by `-`.
#
# Use these prefixes:
#   feature-  – new functionality, module, or major change
#       e.g. feature-ccu-can-interface, feature-bms-parser
#   bugfix-   – bug fixes
#       e.g. bugfix-bms-frame-offset
#   docs-     – documentation only
#       e.g. docs-can-protocol
#   refactor- – restructuring code without changing behavior
#       e.g. refactor-can-driver-layout
#   test-     – tests only
#       e.g. test-ccu-unit-tests
#   chore-    – project maintenance (tooling, configs, scripts)
#       e.g. chore-update-gitignore
#   add-      – adding essentials (e.g. .gitignore, template files)
#       e.g. add-ci-template
#
# Optional scratch branches (for experiments only, no PRs):
#   firstnameLastname
#       e.g. tiberiuTudorascu
#   These are SCRATCHPADS ONLY, not meant to be merged into main.

# ---------------------------------------------------------------------------
# 4. Daily workflow (for each new task / feature)
# ---------------------------------------------------------------------------

# 4.1. Sync your local `main` with upstream
#
# Always start from an up-to-date `main`:
git checkout main
git fetch upstream
git merge upstream/main        # or: git rebase upstream/main (if you are comfortable with it)
git push origin main           # keep your fork's main in sync (recommended)

# 4.2. Create a feature branch from `main`
#
# Pick a meaningful name using the conventions above:
git checkout -b feature-ccu-can-interface
# All your work for this feature lives in this branch.

# ---------------------------------------------------------------------------
# 5. Working on the branch
# ---------------------------------------------------------------------------

# 5.1. Edit, build, test
#
# Make your changes as usual.
# You can close your PC anytime.
# Your branch and commits stay in your local repo until you push.

# 5.2. Stage and commit changes
#
# Commit often, with small, logical changes.
git status
git add .
git commit -m "feature: add basic CAN RX skeleton for CCU"

# Commit message guideline:
#   First line: short summary (<= 72 chars), imperative style if possible:
#       feature: implement basic BMS frame decode
#       bugfix: fix CAN bitrate for CCU
#   Optional body:
#       - explain why the change was made
#       - mention important details or limitations
#       - reference issues if relevant (Fixes #12)
#
# Example commit message body (for reference, not a command):
#   feature: implement basic BMS frame decode
#
#   Parse voltage and temperature frames from the BMS CAN stream.
#   Currently handles only nominal operating range; error frames are
#   logged but not processed yet.
#
#   Fixes #12.

# ---------------------------------------------------------------------------
# 6. Publishing your work (pushing the branch)
# ---------------------------------------------------------------------------

# When your branch is ready for review (or you want to back it up online):
git push -u origin feature-ccu-can-interface

# The `-u` (or `--set-upstream`) sets the tracking relationship so later you
# can just use:
git push

# ---------------------------------------------------------------------------
# 7. Creating a Pull Request (PR)
# ---------------------------------------------------------------------------

# 1) Go to your fork on GitHub:
#    https://github.com/<your-username>/SolarCar
# 2) GitHub should show a banner: "Compare & pull request" – click it
#    If not, go to the "Pull requests" tab and create a new one manually.
# 3) Make sure the PR is:
#       base repository:  tiberiutudorascu/SolarCar
#       base branch:      main
#       head repository:  <your-username>/SolarCar
#       head branch:      feature-ccu-can-interface
# 4) Write a clear PR description:
#       - what the change does
#       - how to test it
#       - any known limitations
#       - reference issues if applicable (Fixes #12)
# 5) Assign / request review from the maintainer(s).

# ---------------------------------------------------------------------------
# 8. Review and changes
# ---------------------------------------------------------------------------

# - The maintainer or another team member reviews your PR.
# - You may be asked to adjust code, docs, or tests.
#
# To update the PR:
#   Make changes locally on the same branch:
git add .
git commit -m "bugfix: adjust CAN RX buffer length"
git push
#   The PR automatically updates with the new commits.
#
# When the PR is approved, the maintainer will merge it into `main`
# in the main repo.

# ---------------------------------------------------------------------------
# 9. After your PR is merged
# ---------------------------------------------------------------------------

# Once your changes are merged into the main repo, you should:

# 9.1. Sync your local and fork `main` again
git checkout main
git fetch upstream
git merge upstream/main
git push origin main

# 9.2. Clean up old feature branches

# Locally:
git branch -d feature-ccu-can-interface

# On your fork (GitHub UI):
#   - Go to "Branches" and delete the merged branch
#   - OR use the "Delete branch" button shown after merge
#
# For the next task, repeat from step 4.1:
#   - sync main
#   - create a new feature-... branch

# ---------------------------------------------------------------------------
# 10. What NOT to commit
# ---------------------------------------------------------------------------

# - No build artefacts:
#     - no build/, Debug/, Release/, .o, .elf, .hex, etc.
# - No IDE-generated junk unless explicitly agreed:
#     - STM32CubeIDE .settings noise, user-specific workspace files, etc.
# - Use .gitignore extensively to keep the repo clean.
#     - Templates: https://github.com/github/gitignore
#
# If you are not sure whether a file should be versioned, ask before committing.
