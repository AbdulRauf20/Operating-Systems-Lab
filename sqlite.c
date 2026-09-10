#include <stdio.h>
#include <sqlite3.h>

int main() {
    sqlite3 *db;

    // Open an in-memory SQLite database
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        printf("Cannot open database\n");
        return 1;
    }

    printf("Database opened successfully!\n");

    // Create a table
    char *error = NULL;

    sqlite3_exec(
        db,
        "CREATE TABLE students (id INTEGER, name TEXT);",
        NULL,
        NULL,
        &error
    );

    // Insert some data
    sqlite3_exec(
        db,
        "INSERT INTO students VALUES (1, 'Rauf');",
        NULL,
        NULL,
        &error
    );

    printf("Table created and data inserted!\n");

    // Close database
    sqlite3_close(db);

    return 0;
}
