/*  bookDelGenre.c - CGI to delete a source from the bookDelSource.html webpage
 *  Author: Geoffrey Jarman
 *  Started: 13-Dec-2021
 *  References:
 *      http://www6.uniovi.es/cscene/topics/web/cs2-12.xml.html
 *  Log:
 *      13-Dec-2021 start by copying bookDelGenre.c
 *      13-Dec-2021 check if no genre deleted
 *      12-Jun-2022 move to gjarman2020.com
 *  Enhancements:
*/

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../shared/rf50.h"

#define SQL_LEN 5000

#define MAXLEN 1024

// global declarations

char *sgServer = "35.188.123.150";                                              // mysqlServer IP address
// char *sgServer = "192.168.0.13";                                             // mysqlServer IP address$
char *sgUsername = "root";                                                      // mysqlSerer logon username$
// char *sgUsername = "gjarman";                                                // mysqlSerer logon username$
char *sgPassword = "Mpa4egu$";                                                  // password to connect to mysqlserver$
char *sgDatabase = "risingfast";                                                // default database name on mysqlserver$

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
MYSQL_FIELD *fields;

char *sParam = NULL;
int  iGenreID = 0;
int  iDelRows = 0;

int main(void) {

    char caSQL[SQL_LEN] = {'\0'};

// print the html content type and <head> block -----------------------------------------------------------------------

    printf("Content-type: text/html\n\n");

// Initialize a connection and connect to the database

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, sgServer, sgUsername, sgPassword, sgDatabase, 0, NULL, 0))
    {
        printf("Failed to connect to MySQL Server %s in module %s()", sgServer, __func__);
        printf("\n\n");
        printf("Error: %s\n", mysql_error(conn));
        printf("\n");
        return  EXIT_FAILURE;
    }

// check for a NULL query string -------------------------------------------------------------------------------------=

//    setenv("QUERY_STRING", "genreID=38", 1);

    sParam = getenv("QUERY_STRING");

    if(sParam == NULL) {
        printf("Query string is empty. Terminating program");
        printf("\n\n");
        return EXIT_FAILURE;
    }

//  get the content from QUERY_STRING and tokenize the ratingID value ---------------------------------------------------

    sscanf(sParam, "genreID=%d", &iGenreID);

// set a SQL query to insert the new author ---------------------------------------------------------------------------

    sprintf(caSQL, "DELETE FROM risingfast.`Book Genres` "
                   "WHERE `Genre ID` = %d;", iGenreID);


    if(mysql_query(conn, caSQL) != 0)
    {
        printf("\n");
        printf("mysql_query() error in function %s():\n\n%s", __func__, mysql_error(conn));
        printf("\n\n");
        return EXIT_FAILURE;
    }

    iDelRows = (int) mysql_affected_rows(conn);

    if(iDelRows == 0) {
        printf("No rows deleted. Genre ID not found\n");
    } else {
        printf("Genre ID '%d' deleted", iGenreID);
    }

    return EXIT_SUCCESS;
}
