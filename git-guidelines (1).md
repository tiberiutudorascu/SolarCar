* Branch etiquette
  * Naming
      * Each participant to this repo is allowed to create a branch titled
        firstnameLastname, should they want to have a clone of the repo to
        work on as a scratchpad (AND ONLY AS A SCRATCHPAD!).
        Changes in these branches will not be merged into the main repo.
      * Branches should be all-lowercase in their naming. Abbreviations
        (explained inside the readmes) are preferred to having long-winded names.
      * Each branch should have a name following a pattern as described
        [here](https://github.com/LeDuble/Git-Etiquette#gtst):
        * docs - Changes to documentation only
        * feature - Since there is a great change of confusion over what
          could constitute a feature, the definition we will use is:
          "A feature represents any standalone module present on the
          car, encompassing both its hardware and software". We would
          have, therefore, branches such as ```"feature-central-compute-unit"```.
          Following the abbreviation note, it would be preferred if it were
          named ```feature-ccu```
        * test - Adding/correcting existing tests
        * refactor - A code change which isn't bug fix or adds a feature
          . It's restructure of the code without changing the
          functionality. This also covers aspects such as typo fixing.
        * bugfix - A bug fix: this should be named to include the bug ID
          ,as referenced on our tracked. e.g.: BugFix-#100 (where the
          number after # is the issue number in GitHub issues).
        * chore - Maintenance or change to auxiliary tools. This covers
          updates to SDKs included, or common component libraries (if
          applicable)
        * add - Adding essentials to the repository
          (e.g. .gitignore or example files)
      * An example of branch names would be:
        * ```feature-eeprom-driver```
        * ```bugfix-#100```

  * After work on a branch is done, code and hardware reviews must
  be conducted by at least one (1) other member. A pull request must be
  created, signed off by at least these two members (the author of the
  branch, and the reviewer).

  * After this step is done, work may be merged into the master repo.
  * Master repo merges MUST be announced on Mattermost, in ~electronics.
  Upon announcement, it is the responsibility of each other team member
  to merge these repo changes into their own branches, at their own
  digression.
  * Commits should not touch files from other branches, unless
  ABSOLUTELY necessary: e.g. a bugfix of an invalid CAN speed set in
  the CCU should not be addressed with a commit from a branch such as
  feature:pitSoftware

* Commit etiquette:
  * Observe the following paradigm:
    * Pull frequently
    * Push infrequently
    * Commit frequently.
  * Your commit message should follow the following template:
    ```
    Short (72 chars or less) summary

    More detailed explanatory text. Wrap it to 72 characters. The blank
    line separating the summary from the body is critical (unless you omit
    the body entirely).

    Further paragraphs come after blank lines.

    - Bullet points are okay, too.
    - Typically a hyphen or asterisk is used for the bullet, followed by a
      single space. Use a hanging indent.

    ```
  * Information in commit messages
    * Describe why a change is being made.
    * How does it address the issue?
    * What effects does the patch have?
    * Do not assume the reviewer understands what the original problem was.
    * Do not assume the code is self-evident/self-documenting.
    * Read the commit message to see if it hints at improved code structure.
    * The first commit line is the most important.
    * Describe any limitations of the current code.

  * References in commit messages
    * If the commit refers to an issue, add this information to the
    commit message header or body. e.g. the GitHub web platform
    automatically converts issue ids (e.g. #123) to links referring to
    the related issue.
    * You should note what issue this commit fixes:
        ```
        Fixes #123, #124
        ```
      This also is automatically picked up by GitHub's issues tab,
      closing the issue.
* Artefacts (executables, compiled code etc.) SHOULD NEVER BE PLACED
  ONTO GIT! Instead, if you would like to create a snapshot of a certain
  feature, in compiled form, you may use the "Releases" tab to create a
  new software release. These releases will contain the repo's source
  at build time, as well as whatever files are built.
* Use ample .gitignores! This file contains all files to be ignored by
  GitHub when uploading. You may find some awesome templates
  [here](https://github.com/github/gitignore). The default template repo
  has a .gitignore based around KiCAD and ESP32 build artifacts, but
  you may need to add more.

## References
* [https://github.com/LeDuble/Git-Etiquette#bed](https://github.com/LeDuble/Git-Etiquette#bed)
* [https://github.com/github/gitignore](https://github.com/github/gitignore)
* [https://gist.github.com/robertpainsi/b632364184e70900af4ab688decf6f53#commit-message-guidelines](https://gist.github.com/robertpainsi/b632364184e70900af4ab688decf6f53#commit-message-guidelines)