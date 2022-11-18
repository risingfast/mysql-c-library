/*  bookChgSourceNme.c - CGI to change a source name from the bookInquiry.html webpage
 *  Author: Geoffrey Jarman
 *  Started: 15-Dec-2021
 *  References:
 *      http://www6.uniovi.es/cscene/topics/web/cs2-12.xml.html
 *  Log:
 *      15-Dec-2021 start by copying bookChgGenreNme.c and modifying
 *      14-Sep-2022 add Access-Control-Allow-Origin: * CORS html header
 *      10-Oct-2022 clean up comments
 *      10-Oct-2022 use EXIT_SUCCESS and EXIT_FAILURE on returns
 *      10-Oct-2022 validate QUERY_STRING not NULL or empty
 *      20-Oct-2022 extend MySQL initialization and shutdown operations
 *      11-Nov-2022 change sprintf() to asprintf()
 *      16-Nov-2022 change strcpy() to strncpy()
 *  Enhancements:
*/

#define _GNU_SOURCE                                                                           // required for asprintf()
#define MAXLEN 1024

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../shared/rf50.h"

// global declarations -------------------------------------------------------------------------------------------------

char *sgServer = "192.168.0.13";                                                                //mysqlServer IP address
char *sgUsername = "gjarman";                                                               // mysqlSerer logon username
char *sgPassword = "Mpa4egu$";                                                     // password to connect to mysqlserver
char *sgDatabase = "risingfast";                                                 // default database name on mysqlserver

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
MYSQL_FIELD *fields;

char *sParam = NULL;
char *sSource = NULL;
char caSourceName[MAXLEN + 1] = {'\0'};
char *sSourceID = NULL;
int  iSourceID = 0;
char caDelimiter[] = "&";

int main(void) {

    char *strSQL = NULL;

// print the html content type and CORS <head> block -------------------------------------------------------------------

    printf("Content-type: text/html\n");
    printf("Access-Control-Allow-Origin: *\n\n");

// check for a NULL query string ---------------------------------------------------------------------------------------

    sParam = getenv("QUERY_STRING");

    if(sParam == NULL) {
        printf("Query string is NULL. Expecting QUERY_STRING=\"sourceID=<99>&sourceName=<chngdsourcename>\". Terminating bookChgSourceNme.cgi");
        printf("\n\n");
        return EXIT_FAILURE;
    }

// test for an empty QUERY_STRING --------------------------------------------------------------------------------------

    if (sParam[0] == '\0') {
        printf("Query string is empty (non-NULL). Expecting QUERY_STRING=\"sourceID=<99>&sourceName=<chngdsourcename>\". Terminating bookChgSourceNme.cgi");
        printf("\n\n");
        return EXIT_FAILURE;
    }

//  get the content from QUERY_STRING and tokenize based on '&' character-----------------------------------------------

    sSourceID = strtok(sParam, caDelimiter);
    sscanf(sSourceID, "sourceID=%d", &iSourceID);

    if (iSourceID == 0) {
        printf("Source ID is 0. Expecting QUERY_STRING=\"sourceID=<99>&sourceName=<chngdsourcename>\". Terminating bookChgSourceNme.cgi");
        printf("\n\n");
        return EXIT_FAILURE;
    }

    sSource = strtok(NULL, caDelimiter);
    sscanf(sSource, "sourceName=%s", caSourceName);

    if (caSourceName[0] == '\0') {
        printf("Source Name is empty. Expecting QUERY_STRING=\"sourceID=<99>&sourceName=<chngdsourcename>\". Terminating bookChgSourceNme.cgi");
        printf("\n\n");
        return EXIT_FAILURE;
    }

    sSource = fUrlDecode(caSourceName);
    strncpy(caSourceName, sSource, MAXLEN);
    free(sSource);

// * initialize the MySQL client library -------------------------------------------------------------------------------

   if (mysql_library_init(0, NULL, NULL)) {
       printf("Cannot initialize MySQL Client library\n");
       return EXIT_FAILURE;
   }

// Initialize a connection and connect to the database -----------------------------------------------------------------

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, sgServer, sgUsername, sgPassword, sgDatabase, 0, NULL, 0))
    {
        printf("\n");
        printf("Failed to connect to MySQL Server %s in module %s()", sgServer, __func__);
        printf("\n\n");
        printf("Error: %s\n", mysql_error(conn));
        printf("\n");
        return EXIT_FAILURE;
    }

// set a SQL query to insert the new author ----------------------------------------------------------------------------

    asprintf(&strSQL, "UPDATE risingfast.`Book Sources` BS "
                   "SET BS.`Source Name` = '%s' "
                   "WHERE BS.`Source ID` = %d;", caSourceName, iSourceID);

// Call the function to print the SQL results to stdout and terminate the program --------------------------------------

    if(mysql_query(conn, strSQL) != 0)
    {
        printf("\n");
        printf("mysql_query() error in function %s():\n\n%s", __func__, mysql_error(conn));
        printf("\n\n");
        return EXIT_FAILURE;
    }

    printf("Source ID %d updated to '%s'", iSourceID, caSourceName);

// * close the database connection created by mysql_init(NULL) ---------------------------------------------------------

    mysql_close(conn);

// * free resources used by the MySQL library --------------------------------------------------------------------------

    mysql_library_end();

// free resources used by strSQL ---------------------------------------------------------------------------------------

    free(strSQL);

    return EXIT_SUCCESS;
}
