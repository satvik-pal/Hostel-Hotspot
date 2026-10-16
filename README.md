# HostelPulse — Complaint Management (50% Checkpoint)

This version intentionally implements the first half of the Complaint Management module for a progress checkpoint.

## Implemented in this checkpoint

- `Complaint` class with private data members
- Complaint information: ID, student ID, category, description, location, priority and status
- Automatic complaint IDs (`C001`, `C002`, ...)
- Basic keyword-based High / Medium / Low priority assignment
- Add a complaint
- Search a complaint by ID
- Display all complaints
- `unordered_map` for complaint storage and average O(1) ID lookup

## Not implemented yet

These are planned next steps, not completed features:

- `priority_queue` scheduling
- Updating complaint status and closing/removing complaints
- Saving/loading complaints from a file or database
- Maintenance worker allocation and actual module integration
- Website/frontend

## Compile and run

```bash
g++ -std=c++14 -Wall -Wextra -pedantic main.cpp -o hostelpulse
```

On Windows PowerShell:

```powershell
.\hostelpulse.exe
```

## Suggested demo for the checkpoint

1. Choose `1` and add a complaint:
   - Student ID: `S101`
   - Category: `Electrical`
   - Description: `Sparking from exposed wire`
   - Location: `Block A Room 12`
2. Show the generated complaint ID and High priority.
3. Choose `2` and search for the generated ID.
4. Choose `3` to show all complaints.
5. Explain that priority-queue scheduling and persistence are the next development steps.

## Important

This is a learning prototype. The priority rules are illustrative and should be reviewed with the project team. Complaints are currently stored in memory, so they disappear when the program exits.
