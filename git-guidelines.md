* Branch etiquette
  * Naming
      * Each participant to this repo is allowed to create a branch titled
        `firstnameLastname`, should they want to have a clone of the repo to
        work on as a scratchpad (AND ONLY AS A SCRATCHPAD!).
        Changes in these branches will not be merged into the main repo.
      * Branches should be all-lowercase in their naming. Abbreviations
        are preferred to having long-winded names.
      * Each branch should have a name following a pattern similar to
        the one described here:
        * `docs` - Changes to documentation only.
        * `feature` - A feature represents any standalone module present on the
          car, encompassing both its hardware and software.
          We would have, therefore, branches such as
          `"feature-central-compute-unit"`. Following the abbreviation note, it
          would be preferred if it were named `feature-ccu`.
        * `test` - Adding/correcting existing tests.
        * `refactor` - A code change which isn't a bug fix and does not add a feature.
          It is a restructure of the code without changing functionality.
          This also covers aspects such as typo fixing.
        * `bugfix` - A bug fix. This should be named to include the bug/issue ID
          as referenced in the tracker. e.g.: `bugfix-#100`
          (where the number after `#` is the issue number in GitHub issues).
        * `chore` - Maintenance or changes to auxiliary tools. This covers
          updates to SDKs, build scripts, or common component libraries.
        * `add` - Adding essentials to the repository
          (e.g. `.gitignore` or example files).
      * Example branch names:
        * `feature-eeprom-driver`
        * `feature-ccu-can-interface`
        * `bugfix-#100`
        * `docs-can-protocol`

  * Reviews and merges
      * After work on a branch is done, code and/or hardware reviews must
        be conducted by at least one (1) other member whenever possible.
      * A pull request must be created and signed off by at least these
        two members:
        * the author of the branch
        * the reviewer
      * After this step is done, work may be merged into the main repo.
      * Master/main repo merges SHOULD be announced on the team communication
        channel (e.g. Discord/WhatsApp/Teams/etc.).
        Upon announcement, it is the responsibility of each team member
        to merge these repo changes into their own branches, at their own
        discretion.
      * Commits should not touch files that clearly belong to other features,
        unless ABSOLUTELY necessary.
        For example, a bugfix of an invalid CAN speed set in the CCU should
        not be addressed from a branch such as `feature-pit-software`,
        but rather from a dedicated `bugfix-...` branch.

* Commit etiquette
  * General paradigm
      * Pull frequently.
      * Push infrequently.
      * Commit frequently (small, logical steps).

  * Commit message structure
      * Your commit message should follow the template:
        ```
        Short (72 chars or less) summary

        More detailed explanatory text. Wrap it to ~72 characters.
        The blank line separating the summary from the body is critical
        (unless you omit the body entirely).

        Further paragraphs come after blank lines.

        - Bullet points are okay, too.
        - Typically a hyphen or asterisk is used for the bullet, followed by a
          single space. Use a hanging indent.
        ```
      * The first line is the most important: keep it short and meaningful.
        Example styles:
        * `feature: add basic CAN RX skeleton for CCU`
        * `bugfix: fix BMS frame offset`
        * `docs: describe CAN protocol for dashboard`

  * Information in commit messages
      * Describe WHY a change is being made, not only WHAT changed.
      * Explain how it addresses the issue.
      * State what effects the patch has.
      * Do not assume the reviewer understands what the original problem was.
      * Do not assume the code is self-evident/self-documenting.
      * Use the commit message body to note:
        * design decisions
        * limitations of the current code
        * TODOs or follow-ups, if relevant.

  * References in commit messages
      * If the commit refers to an issue, add this information to the
        commit message header or body. e.g. the GitHub web platform
        automatically converts issue ids (e.g. `#123`) to links referring to
        the related issue.
      * You should note what issue this commit fixes, when applicable:
        ```
        Fixes #123, #124
        ```
        This will be automatically picked up by GitHub's issues tab,
        closing the referenced issues upon merge.

* Repository contents
  * Artefacts
      * Artefacts (executables, compiled code, etc.) SHOULD NEVER BE PLACED
        INTO GIT.
      * Instead, if you would like to create a snapshot of a certain
        feature in compiled form, use the "Releases" tab to create a
        new software release.
      * These releases will contain:
        * the repo's source at build time
        * any additional built files you explicitly upload.

  * .gitignore usage
      * Use ample `.gitignore` rules.
      * This file contains all files to be ignored by Git when tracking.
      * Good templates can be found here:
        * https://github.com/github/gitignore
      * The default template for this project can include:
        * STM32CubeIDE build artefacts (e.g. `Debug/`, `Release/`)
        * intermediate object files, logs, user-specific settings
      * You may need to add more patterns over time as the project grows.

## References
* https://github.com/LeDuble/Git-Etiquette#bed
* https://github.com/github/gitignore
* https://gist.github.com/robertpainsi/b632364184e70900af4ab688decf6f53#commit-message-guidelines

* Fork-based workflow
  * Roles
      * Main repository (source of truth):
        `https://github.com/tiberiutudorascu/SolarCar`
      * Each contributor has their own fork:
        `https://github.com/<your-username>/SolarCar`
      * Work is done locally and on the contributor’s fork.
      * Contributions are merged into the main repository ONLY via
        Pull Requests (PRs) into `main`.
      * Only maintainers of the main repository merge into `main`.

  * One-time setup (per contributor)
      * Fork the main repository:
        * Go to `https://github.com/tiberiutudorascu/SolarCar`
        * Click **Fork**
        * You will get `https://github.com/<your-username>/SolarCar`
      * Clone your fork locally:
        * `git clone https://github.com/<your-username>/SolarCar.git`
        * `cd SolarCar`
      * Add the main repository as `upstream`:
        * `git remote add upstream https://github.com/tiberiutudorascu/SolarCar.git`
        * `git remote -v`
          * `origin` → your fork
          * `upstream` → main repository

  * Daily workflow (for each new task / feature)
      * Sync your local `main` with `upstream`:
        * `git checkout main`
        * `git fetch upstream`
        * `git merge upstream/main`  
          (or `git rebase upstream/main` if you are comfortable with rebasing)
        * `git push origin main`  
          (keeps your fork’s `main` in sync with the main repo)
      * Create a feature branch from `main`:
        * `git checkout -b feature-ccu-can-interface`
        * All your work for this feature lives in this branch.
      * Work on the branch:
        * Edit, build, test as usual.
        * You can close your PC anytime; the branch and commits remain
          in your local repo.
      * Stage and commit changes:
        * `git status`
        * `git add .`
        * `git commit -m "feature: add basic CAN RX skeleton for CCU"`
        * Commit often, with small, logical changes.

  * Publishing your work (pushing the branch)
      * When your branch is ready for review (or you want remote backup):
        * `git push -u origin feature-ccu-can-interface`
      * The `-u` (or `--set-upstream`) sets the tracking relationship so
        that later you can simply use:
        * `git push`

  * Creating a Pull Request (PR)
      * Go to your fork on GitHub:
        * `https://github.com/<your-username>/SolarCar`
      * GitHub will usually show a **Compare & pull request** banner.
        If not, go to the **Pull requests** tab and create a new PR manually.
      * Ensure the PR is set up as:
        * base repository: `tiberiutudorascu/SolarCar`
        * base branch: `main`
        * head repository: `<your-username>/SolarCar`
        * head branch: `feature-ccu-can-interface`
      * In the PR description, include:
        * what the change does
        * how to test it
        * any known limitations
        * references to issues, e.g. `Fixes #12`
      * Request review from the maintainer(s) and/or other team members.

  * Review and updates
      * The maintainer or another team member reviews your PR.
      * If changes are requested:
        * apply them locally on the same branch:
          * `git add .`
          * `git commit -m "bugfix: adjust CAN RX buffer length"`
          * `git push`
        * the PR will update automatically with the new commits.
      * After approval, the maintainer merges the PR into `main`
        in the main repository.

  * After your PR is merged
      * Sync your local and fork `main` again:
        * `git checkout main`
        * `git fetch upstream`
        * `git merge upstream/main`
        * `git push origin main`
      * Clean up old feature branches:
        * Locally:
          * `git branch -d feature-ccu-can-interface`
        * On your fork (GitHub UI):
          * Delete the merged branch from the **Branches** section or via
            the **Delete branch** button shown after merge.
      * For the next task, repeat from:
        * sync `main`
        * create a new `feature-...` branch.

  * Notes on files and artefacts
      * Follow the “Repository contents” section above.
      * Do not commit build artefacts, executables, or user-specific
        IDE settings.
      * Use `.gitignore` to keep the repo clean and focused on source code.
