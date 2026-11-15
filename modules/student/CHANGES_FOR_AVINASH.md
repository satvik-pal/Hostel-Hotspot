# Change to your Student module for the database (please review)

Your restructured module is unchanged except for one addition needed to load accounts from SQLite:

- `include/AuthManager.h`, `src/AuthManager.cpp`: `bool restoreAccount(const UserAccount& account)`
  inserts a saved account with its stored password hash (the password itself is never stored, so the
  account cannot be re-created with `createAccount`). Returns false for an empty ID, empty hash or
  an existing ID.
- `tests/test_student.cpp`: 3 new checks (now 31, all passing).
- `README.md`: short "Persistence support" section.

Commit this version to `Avinash-Student` instead of the previous zip (same commands as before).
