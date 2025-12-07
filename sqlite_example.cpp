#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>

// Callback function for sqlite3_exec. Not used for SELECT in this example,
// but useful for simple commands.
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   for(int i = 0; i < argc; i++) {
      // Print column name and value
      std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
   }
   std::cout << std::endl;
   return 0;
}

int main(int argc, char* argv[]) {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   const char* db_file = "people.db";

   // 1. Open the database
   rc = sqlite3_open(db_file, &db);

   if(rc) {
      std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
      return(0);
   } else {
      std::cout << "Opened database successfully" << std::endl;
   }

   // 2. Create a table if it doesn't exist
   const char* sql_create_table =
      "CREATE TABLE IF NOT EXISTS PERSON("
      "ID INT PRIMARY KEY     NOT NULL,"
      "NAME           TEXT    NOT NULL,"
      "AGE            INT     NOT NULL);";

   rc = sqlite3_exec(db, sql_create_table, callback, 0, &zErrMsg);
   
   if(rc != SQLITE_OK){
      std::cerr << "SQL error: " << zErrMsg << std::endl;
      sqlite3_free(zErrMsg);
   } else {
      std::cout << "Table created or already exists." << std::endl;
   }

   // 3. Insert some data (first, clear existing data for a clean run)
   const char* sql_delete = "DELETE FROM PERSON;";
   sqlite3_exec(db, sql_delete, 0, 0, &zErrMsg); // Ignore errors here

   const char* sql_insert =
      "INSERT INTO PERSON (ID,NAME,AGE) VALUES (1, 'Alice', 32);"
      "INSERT INTO PERSON (ID,NAME,AGE) VALUES (2, 'Bob', 28);"
      "INSERT INTO PERSON (ID,NAME,AGE) VALUES (3, 'Charlie', 45);";

   rc = sqlite3_exec(db, sql_insert, 0, 0, &zErrMsg);

   if(rc != SQLITE_OK){
      std::cerr << "SQL error: " << zErrMsg << std::endl;
      sqlite3_free(zErrMsg);
   } else {
      std::cout << "Records inserted successfully." << std::endl;
   }

   // 4. Prepare the SELECT statement to fetch data
   std::cout << "\n--- Reading data from database ---" << std::endl;
   sqlite3_stmt *stmt;
   const char* sql_select = "SELECT NAME, AGE FROM PERSON;";

   rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, NULL);
   if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
   }

   // 5. Loop through the results
   while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      // Get data from columns
      const unsigned char* name = sqlite3_column_text(stmt, 0);
      int age = sqlite3_column_int(stmt, 1);
      
      std::cout << "Name: " << name << ", Age: " << age << std::endl;
   }

   if (rc != SQLITE_DONE) {
        std::cerr << "Error stepping through statement: " << sqlite3_errmsg(db) << std::endl;
   }

   // 6. Finalize the statement to free resources
   sqlite3_finalize(stmt);

   // 7. Close the database
   sqlite3_close(db);

   return 0;
}
