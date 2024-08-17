#ifndef _PROJECT__H_
#define _PROJECT__H_

#include <limits.h>

#define TRUE 1
#define FALSE 0
#define INPUT_BUFFER_SIZE 1024  // Maximum input string size
#define STDNT_ID_LEN 6          // Student ID length, not including '\0'
#define EXCRS_RNDS 6            // Number of excercise rounds
#define EXCRS_PTS 999           // Maximum amount of points per round
#define QUIT_FLAG 1
#define NO_CMND_CHAR '*'        // Flag to denote no viable command in Input struct
#define INT_ERR_BOUND (INT_MIN+10) // Nums below this (very small) num are considered errors

// Error codes
#define ERR_UNKNOWN -1          // Generic error code
#define ERR_CRITICAL -2         // Critical error -> exit program gracefully
#define ERR_MEM_ALLOC_FAIL -3   // Memory allocation failed
#define ERR_NON_VIABLE_INP -4   // Input could not be parsed for any arguments
#define ERR_INV_CMND_CHAR -5    // Invalid command character
#define ERR_NO_CMND_CHAR -6     // Could not find a command character in user input
#define ERR_UPD_PTS_ON_EMPT -7  // Attempting update_points() on an empty list
#define ERR_STDNT_NOT_FND -8    // Student could not be found
#define ERR_STDNT_IN_LIST -9    // Student is already in the list
#define ERR_EMPTY_INP -20       // Empty input
#define ERR_TOO_MANY_ARGS -21   // Input has too many arguments
#define ERR_TOO_FEW_ARGS -22    // Input has too few arguments
#define ERR_ROUND_CNV -30       // Cannot convert rounds to an integer
#define ERR_RND_NUM_OOB -31     // Given round number out-of-bounds
#define ERR_POINTS_CNV -40      // Cannot convert points to an integer
#define ERR_PTS_OOB -41         // Given amount of points is out-of-bounds
#define ERR_NULL_STRM -50       // Attempting print to NULL stream
#define ERR_WRT_EMPT_LST -51    // Attempting to write an empty list to file
#define ERR_FILE_OPEN -52       // File could not be opened
#define ERR_FILE_CORR -53       // File could not be parsed successfully
#define ERR_FILENAME_INV -54    // Filename invalid
#define ERR_FILENAME_LEN -55    // Filename too long
#define ERR_ID_TOO_LONG -60     // Given student ID is too long
#define ERR_ID_EMPTY -61        // Given student ID is empty
#define ERR_ID_NOT_ALNUM -62    // Given student ID is not alphanumeric
#define ERR_INT_CNV (INT_MIN)   // Conversion of str to int not possible
#define ERR_INT_OOB (INT_MIN+1) // Given number out-of-bounds for int type
#define ERR_INT_NEG (INT_MIN+2) // Given int is neg when only pos is allowed
#define ERR_CHAR '1'

// Expected amount of user arguments for each command
#define ADD_ARGS 4      // ADD: <'A'> <student ID> <last name> <first name>
#define UPDATE_ARGS 4   // UPDATE: <'U'> <student ID> <round> <points>
#define LIST_ARGS 1     // LIST: <'L'>
#define WRITE_ARGS 2    // WRITE: <'W'> <file name>
#define LOAD_ARGS 2     // LOAD: <'O'> <file name>
#define QUIT_ARGS 1     // QUIT: <'Q'>
#define MAX_ARGS 10     // FILE: <ID> <lname> <fname> <rnd1> ... <rnd6> <totalpts>

#include <stdio.h>

/**
 * @brief Student info stored as a linked list. The first "Student" in the linked list is
 * permanently fixed dummy that is denoted by setting student_id, lastname and firstname to
 * NULL.
 * 
 * @param student_id
 * @param lastname
 * @param firstname
 * @param points array of size EXCRS_RNDS that contains points for each round
 * @param next pointer to next Student node
 * 
 * @attention Initialize the linked list by calling init_linked_list() to create the dummy
 * node and pointer to it.
 */
typedef struct student {
	char *student_id;
	char *lastname;
	char *firstname;
	int points[EXCRS_RNDS];
	struct student *next;
} Student;

/**
 * @brief One instance of Input struct holds either one parsed line of user given input, or
 * one parsed line read from a file.
 * 
 * @param arg_count number of arguments found in input string (including command char)
 * @param cmnd holds a valid command character, NO_CMND_CHAR if input comes from FILE
 * @param arg_arr pointer to an array that holds all found arguments (includes the command
 * character, if present)
 * 
 * @note Initialized by parse_input()
 * @note Dynamically allocated memory free'd by free_parsed_input()
 */
typedef struct {
	int arg_count;
	char cmnd;
	char *arg_arr[MAX_ARGS];
} Input;

/**
 * @brief One ErrorCode instance pairs an error code to an error message.
 */
typedef struct {
	int code;
	const char *head;
	const char *msg;
} ErrorCode;

// Initialize all possible error codes
ErrorCode err_codes[] = {
	{ERR_UNKNOWN,           "ERR_UNKNOWN",          "Ran into an unknown error."},
	{ERR_CRITICAL,          "ERR_CRITICAL",         "Critical error, exiting..."},
	{ERR_MEM_ALLOC_FAIL,    "ERR_MEM_ALLOC_FAIL",   "Allocation of dynamic memory failed."},
	{ERR_NON_VIABLE_INP,    "ERR_NON_VIABLE_INP",   "Input could not be parsed for any arguments."},
	{ERR_INV_CMND_CHAR,     "ERR_INV_CMND_CHAR",    "Invalid command"}, // This code only for meeting project requirements
	{ERR_NO_CMND_CHAR,      "ERR_NO_CMND_CHAR",     "Could not find a valid command character."},
	{ERR_UPD_PTS_ON_EMPT,   "ERR_UPD_PTS_ON_EMPT",  "Attempting to update points on an empty list."},
	{ERR_STDNT_NOT_FND,     "ERR_STDNT_NOT_FND",    "Student could not be found."},
	{ERR_STDNT_IN_LIST,     "ERR_STDNT_IN_LIST",    "Student ID is already found in the list."},
	{ERR_EMPTY_INP,         "ERR_EMPTY_INP",        "Input is empty."},
	{ERR_TOO_MANY_ARGS,     "ERR_TOO_MANY_ARGS",    "Too many arguments for the given command type."},
	{ERR_TOO_FEW_ARGS,      "ERR_TOO_FEW_ARGS",     "Too few arguments for the given command type."},
	{ERR_ROUND_CNV,         "ERR_ROUND_CNV",        "Round number is not an integer."},
	{ERR_RND_NUM_OOB,       "ERR_RND_NUM_OOB",      "Round number is out of bounds."},
	{ERR_POINTS_CNV,        "ERR_POINTS_CNV",       "Points is not an integer."},
	{ERR_PTS_OOB,           "ERR_PTS_OOB",          "Points is out of bounds."},
	{ERR_NULL_STRM,         "ERR_NULL_STRM",        "Attempting print to NULL stream."},
	{ERR_WRT_EMPT_LST,      "ERR_WRT_EMPT_LST",     "Attempting to write an empty list to file."},
	{ERR_FILE_OPEN,         "ERR_FILE_OPEN",        "File could not be opened."},
	{ERR_FILE_CORR,         "ERR_FILE_CORR",        "File corruption."},
	{ERR_FILENAME_INV,      "ERR_FILENAME_INV",     "File name is invalid."},
	{ERR_FILENAME_LEN,      "ERR_FILENAME_LEN",     "File name is too long."},
	{ERR_ID_TOO_LONG,       "ERR_ID_TOO_LONG",      "Given student ID is too long."},
	{ERR_ID_EMPTY,          "ERR_ID_EMPTY",         "Given student ID is empty."},
	{ERR_ID_NOT_ALNUM,      "ERR_ID_NOT_ALNUM",     "Given student ID contains symbols other than letters and numbers."},
	{ERR_INT_CNV,           "ERR_INT_CNV",          "Conversion of str to int not possible."},
	{ERR_INT_OOB,           "ERR_INT_OOB",          "Given number out of bounds for int type."},
	{ERR_INT_NEG,           "ERR_INT_NEG",          "Given integer is negative when only positive integers are allowed."},
};

// Holds total number of error codes
#define NUM_ERRORS ((int)(sizeof(err_codes) / sizeof(err_codes[0])))

// All function prototypes, mostly included for testing purposes
int count_arguments(char *input);
int validate_int_input(char *str, int allow_neg);
int validate_id(char *student_id);
int validate_rounds(char *round_str);
int validate_points(char *points_str);
int validate_filename(char *filename);
Input *parse_input(char *input, int *error, int user_input);
void free_parsed_input(Input *parsed_inp);
char parse_command(char *input, int *error);
int validate_input(Input *parsed_inp);
int count_points(Student *student);
int sort_students(Student *a, Student *b);
Student *init_linked_list(void);
Student *init_student(char *student_id, char *lastname, char *firstname);
void free_student(Student *student);
void place_into_list(Student *student, Student *students_head);
int add_student(char *student_id, char *lastname, char *firstname, Student *students_head);
void free_linked_list(Student *student);
int update_points(char *student_id, char *round, char *points, Student *students_head);
int print_to_stream(FILE *stream, Student *student);
int print_status(Student *students_head);
int write_to_file(char *filename, Student *students_head);
int load_file(char *filename, Student *students_head);
int run(char *input, Student *students_head);
void print_error(int err_code);

#endif //! _PROJECT__H_