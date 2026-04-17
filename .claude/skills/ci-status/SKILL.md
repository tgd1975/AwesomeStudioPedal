---
name: ci-status
description: Summarize recent GitHub Actions run statuses per workflow
---

# ci-status

Fetch the most recent GitHub Actions runs and print a per-workflow status summary.

Steps:

1. Run:

   ```bash
   gh run list --limit 40 --json workflowName,status,conclusion,createdAt,headBranch,headCommit,url
   ```

2. Group by `workflowName`. For each workflow show only the most recent run.

3. Print a table:

   ```
   Workflow              Branch   Status    Conclusion  Age    URL
   test.yml              main     completed success     2h ago https://...
   static-analysis.yml   main     completed failure     2h ago https://...
   codeql-analysis.yml   main     in_progress —         5m ago https://...
   docs.yml              main     completed success     3d ago https://...
   ```

4. Below the table, list any workflows with `conclusion == "failure"` and print their
   URL for quick access.

5. If `gh` is not authenticated or the repo has no runs yet, report the error and stop.

Do not open URLs — just print them.
