/**
 * @file project.c
 * @author Matti Kivelä
 * @brief ELEC-A7100 Basic course in C programming - Programming Project
 * @version 1.0
 * @date 2024-08-16
 *
 * @copyright Copyright (c) 2024
 */

#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* The following block is only used for testing purposes. Comment out "#define TEST" to run
the program normally.*/
//#define TEST
#ifdef TEST
#include "test.h"   // Include test.h if in TEST mode
#endif

enum Command {
	ADD = 'A',
	UPDATE = 'U',
	LIST = 'L',
	WRITE = 'W',
	LOAD = 'O',
	QUIT = 'Q'
};

/**
 * @brief Counts the number of user given arguments in user input string (including
 * command character). One argument is assumed to end either in space or newline.
 *
 * @param input modifiable user input string
 * @return number of arguments found in the input string, error code otherwise
 */
int count_arguments(char *input) {
	// Make a copy of the input to prevent modification of the original
	char *input_copy = malloc(strlen(input) + 1);
	if (input_copy == NULL) return ERR_MEM_ALLOC_FAIL;  // Handle alloc failure
	strcpy(input_copy, input);

	char *token = strtok(input_copy, " \n");
	if (token == NULL) {    // Return error if input not viable
		free(input_copy);
		return ERR_NON_VIABLE_INP;
	}

	// Count arguments
	int num = 0;
	while (token != NULL) {
		num++;
		token = strtok(NULL, " \n");
	}

	free(input_copy);
	return num;
}

/**
 * @brief Checks that the given string can be successfully converted into an integer and is
 * within bounds. Returns the converted integer when successful, error code otherwise.
 *
 * @param str string to be converted
 * @param allow_neg TRUE/FALSE depending if negative numbers are allowed
 * @return converted int when successful, error code otherwise
 */
int validate_int_input(char *str, int allow_neg) {
	char *endp;     // Used by strtol() to denote last viable character
	long val;       // Returned value

	val = strtol(str, &endp, 10);

	/* If end pointer did not move OR end pointer is not pointing to the end of the string
	the input cannot be converted to a valid integer. */
	if (endp == str || *endp != '\0') return ERR_INT_CNV;

	// Check that the value can be stored in an int
	if (val < INT_MIN || val > INT_MAX) return ERR_INT_OOB;

	// If allow_neg == FALSE, check that the value is non-negative
	if (allow_neg == FALSE && val < 0) return ERR_INT_NEG;

	return (int)val;
}

/**
 * @brief Checks that the given student ID is valid.
 *
 * @param student_id
 * @return 0 if valid, error code otherwise
 */
int validate_id(char *student_id) {
	// Check length
	int len = strlen(student_id);
	if      (len > STDNT_ID_LEN) return ERR_ID_TOO_LONG;
	else if (len == 0) return ERR_ID_EMPTY;  // Handle exception: empty ID

	// Check that the ID only contains numbers and letters
	while (*student_id) {
		if (!isalnum(*student_id)) return ERR_ID_NOT_ALNUM;
		student_id++;
	}

	return 0;
}

/**
 * @brief Checks the input is a viable round number.
 *
 * @param round_str input string for round number
 * @return round num when successful, error code otherwise
 */
int validate_rounds(char *round_str) {
	int val = validate_int_input(round_str, FALSE);
	if (val == ERR_INT_CNV) return ERR_ROUND_CNV;   // Round num not integer
	else if (val < INT_ERR_BOUND) return val;       // Return other error codes
	else if (val < 1 || val > EXCRS_RNDS) return ERR_RND_NUM_OOB; // Round num out-of-bounds
	else return val;
}

/**
 * @brief Checks the input is a viable point amount.
 *
 * @param points_str input string for points
 * @return points when successful, error code otherwise
 */
int validate_points(char *points_str) {
	int val = validate_int_input(points_str, FALSE);
	if (val == ERR_INT_CNV) return ERR_POINTS_CNV;  // Points not integer
	else if (val < INT_ERR_BOUND) return val;       // Return other error codes
	else if (val < 0 || val > EXCRS_PTS) return ERR_PTS_OOB; // Points out-of-bounds
	else return val;
}

/**
 * @brief Checks the filename is not too long, doesn't contain illegal characters and
 * doesn't end in a period.
 *
 * @param filename
 * @return 0 when successful, error code otherwise
 */
int validate_filename(char *filename) {
	// Characters that can't be part of a filename on Windows
	const char *invalid_chars = "<>:\"/\\|?*";

	// Check the filename doesn't contain any illegal characters
	for (int i = 0; filename[i] != '\0'; i++) {
		if (strchr(invalid_chars, filename[i]) != NULL) return ERR_FILENAME_INV;
	}

	int len = strlen(filename);

	// Check filename not too long
	if (len > 255) return ERR_FILENAME_LEN;

	// Check filename doesn't end in period
	if (filename[len - 1] == '.') return ERR_FILENAME_INV;  // [len - 1] to account for '\0'

	return 0;
}

/**
 * @brief Takes a line of either user given or file read input string. Parses the possible
 * command character and arguments and populates an Input struct with the info. Returns a
 * pointer to the Input struct when successful, NULL to signal an error.
 *
 * @attention Use free_parsed_input() to free dynamically allocated memory
 *
 * @param input string from user or file
 * @param error pointer to a variable that holds possible errors
 * @param user_input TRUE/FALSE depending whether the Input comes from user or FILE
 * @return pointer to Input struct when successful, NULL otherwise
 *
 * @note NULL return signifies an error, read "error" var for exact error code
 */
Input *parse_input(char *input, int *error, int user_input) {
	// Check for empty input
	if (strlen(input) <= 1) {   // If only "\n" present
		if (user_input) *error = ERR_EMPTY_INP;     // User input empty
		else *error = ERR_FILE_CORR;                // Empty line from file -> file corrupt
		return NULL;
	}

	// Alloc memory for Input structure
	Input *parsed_inp = malloc(sizeof(Input));
	if (parsed_inp == NULL) {   // Handle alloc error
		*error = ERR_MEM_ALLOC_FAIL;
		return NULL;
	}

	// Parse the input for a command character
	char command = parse_command(input, error);

	/* =====================================================================================
	Handles a special case to meet project requirements. This particular error code is only
	used to print: "Invalid command <command char>\n" into stdout. Otherwise unnecessary and
	only serves to make code less readable.*/
	if (*error == ERR_INV_CMND_CHAR) {
		parsed_inp -> cmnd = command;
		return parsed_inp;
	}
	// =====================================================================================

	// If user input ...
	if (user_input) {
		// ... and no valid command char was found, return error
		if (command == ERR_CHAR) {
			*error = ERR_NO_CMND_CHAR;
			free(parsed_inp);
			return NULL;
		}
		// ... and valid command char, populate the struct
		else parsed_inp -> cmnd = command;
	}
	// If FILE input, populate cmnd with NO_CMND_CHAR
	else parsed_inp -> cmnd = NO_CMND_CHAR;

	// Populate argument count
	int arg_count = count_arguments(input);
	if (arg_count < 0) {    // Handle error
		free(parsed_inp);
		*error = arg_count;
		return NULL;
	}
	parsed_inp -> arg_count = arg_count;    // Populate Input struct with arg count

	// Populate Input struct with arguments
	char *token = NULL;
	char *arg_ptr = NULL;
	for (int i = 0; i < MAX_ARGS; i++) {
		if (i == 0) token = strtok(input, " \n");   // First argument
		else        token = strtok(NULL, " \n");    // Subsequent arguments

		// Only alloc memory for next argument if token not NULL
		if (token != NULL) arg_ptr = malloc(strlen(token) + 1);

		// Handle alloc failure
		if (arg_ptr == NULL && token != NULL) {
			for (int j = 0; j < i; j++) free(parsed_inp -> arg_arr[j]); // Free elements
			free(parsed_inp);   // Free Input struct
			*error = ERR_MEM_ALLOC_FAIL;
			return NULL;
		}

		// Place the argument into the Input struct array
		if (token != NULL) {
			strcpy(arg_ptr, token);
			parsed_inp -> arg_arr[i] = arg_ptr;
		}
		else parsed_inp -> arg_arr[i] = NULL;   // Blanks any unused elements
	}

	return parsed_inp;
}

/**
 * @brief Frees dynamically allocated memory allocated by parse_input().
 *
 * @param parsed_inp Input struct to be free'd
 */
void free_parsed_input(Input *parsed_inp) {
	// Free elements in the argument array
	for (int i = 0; i < parsed_inp -> arg_count; i++) free(parsed_inp -> arg_arr[i]);

	// Free Input struct
	free(parsed_inp);
}

/**
 * @brief Checks the input for valid command character. Returns the command character when
 * successful. Returns error, if the input could not be parsed for a valid command.
 *
 * @param input input string
 * @param error pointer to a variable that holds possible errors
 * @return Command character when successful, error otherwise.
 */
char parse_command(char *input, int *error) {
	// First char must be capital and second char must be whitespace, otherwise return error
	if (!isupper(input[0]) || !isspace(input[1])) return ERR_CHAR;

	// Switch for command type. Check that user argument count matches given command
	switch (input[0]) {
		case ADD:       return ADD;
		case UPDATE:    return UPDATE;
		case LIST:      return LIST;
		case WRITE:     return WRITE;
		case LOAD:      return LOAD;
		case QUIT:      return QUIT;
		default:
			*error = ERR_INV_CMND_CHAR;     // Invalid command character
			return input[0];    // Return invalid command char to meet project requirements
	}
}

/**
 * @brief Checks that the given Input struct is formatted correctly. Returns 0 when Input
 * can be used to create a Student, error code otherwise.
 *
 * @param parsed_inp parsed Input struct
 * @return 0 when successful, error code otherwise
 */
int validate_input(Input *parsed_inp) {
	int err = 0;    // Holds various error values
	char command = parsed_inp -> cmnd;
	int arg_count = parsed_inp -> arg_count;
	char **arg_arr = parsed_inp -> arg_arr;

	// If command is NO_CMND_CHAR, the input must come from file instead of user
	if (command != NO_CMND_CHAR) {
		switch (command) {
			case ADD:       // ADD: <'A'> <student ID> <last name> <first name>
				if      (arg_count > ADD_ARGS) return ERR_TOO_MANY_ARGS;
				else if (arg_count < ADD_ARGS) return ERR_TOO_FEW_ARGS;

				// Check for valid student ID
				err = validate_id(arg_arr[1]);
				if (err) return err;

				break;

			case UPDATE:    // UPDATE: <'U'> <student ID> <round> <points>
				if      (arg_count > UPDATE_ARGS) return ERR_TOO_MANY_ARGS;
				else if (arg_count < UPDATE_ARGS) return ERR_TOO_FEW_ARGS;

				// Check for valid student ID
				err = validate_id(arg_arr[1]);
				if (err) return err;

				// Check for valid round number
				err = validate_rounds(arg_arr[2]);
				if (err < 1) return err;

				// Check for valid points
				err = validate_points(arg_arr[3]);
				if (err < 0) return err;

				break;

			case LIST:      // LIST: <'L'>
				if      (arg_count > LIST_ARGS) return ERR_TOO_MANY_ARGS;
				else break;

			case WRITE:     // WRITE: <'W'> <file name>
				if      (arg_count > WRITE_ARGS) return ERR_TOO_MANY_ARGS;
				else if (arg_count < WRITE_ARGS) return ERR_TOO_FEW_ARGS;

				// Validate filename
				err = validate_filename(arg_arr[1]);
				if (err) return err;

				break;

			case LOAD:      // LOAD: <'O'> <file name>
				if      (arg_count > LOAD_ARGS) return ERR_TOO_MANY_ARGS;
				else if (arg_count < LOAD_ARGS) return ERR_TOO_FEW_ARGS;

				// Validate filename
				err = validate_filename(arg_arr[1]);
				if (err) return err;

				break;

			case QUIT:      // QUIT: <'Q'>
				if      (arg_count > QUIT_ARGS) return ERR_TOO_MANY_ARGS;
				else break;

			default:    // Not viable command (should never happen due to other checks)
				return ERR_UNKNOWN;
		}
	}

	// FILE: <ID> <lname> <fname> <rnd1> ... <rnd6> <totalpts>
	else {
		// FILE should always have arg_count == MAX_ARGS
		if (arg_count != MAX_ARGS) return ERR_FILE_CORR;

		// Validate ID
		err = validate_id(arg_arr[0]);
		if (err) return ERR_FILE_CORR;

		// Validate points
		for (int i = 0; i < (EXCRS_RNDS + 1); i++) {    // +1 for <totalpts>
			err = validate_points(arg_arr[i + 3]);
			if (err < 0) return ERR_FILE_CORR;
		}
	}

	// Return success if all tests passed
	return 0;
}

/**
 * @brief Counts the Student's total points.
 *
 * @param student
 * @return int
 */
int count_points(Student *student) {
	int points = 0;
	for (int i = 0; i < EXCRS_RNDS; i++) points += student -> points[i];
	return points;
}

/**
 * @brief Sorts two students based on order: total points > last name > first name > student
 * number. For negative return values a comes before b, for positive b comes before a.
 * Function returns zero for identical Students (should never be possible). If either a or b
 * points to NULL, Student comes before NULL.
 *
 * @attention Function assumes both arguments cannot be NULL.
 *
 * @param a Student a
 * @param b Student b
 * @return <0 if a before b, >0 if b before a, 0 if a and b identical
 */
int sort_students(Student *a, Student *b) {
	// Check for NULL pointers: -1 if b is NULL, 1 if a is NULL
	if ((a == NULL) || (b == NULL)) return (a == NULL) ? 1 : -1;

	char *a_lname = a -> lastname;
	char *b_lname = b -> lastname;
	char *a_fname = a -> firstname;
	char *b_fname = b -> firstname;
	char *a_num = a -> student_id;
	char *b_num = b -> student_id;
	int ret = 0;    // Holds return values

	/* Handle comparison against the head of the linked list. If last name, first name and
	student number all equal NULL, the Student is the head of the linked list (i.e. the
	dummy node). This makes sure the head of the list always wins the comparison and won't
	be moved.*/
	if (a_lname == NULL && a_fname == NULL && a_num == NULL) return -1;
	if (b_lname == NULL && b_fname == NULL && b_num == NULL) return 1;

	// If a's and b's points are not equal, return -1 if a bigger, and 1 if b bigger.
	int a_pts = count_points(a);
	int b_pts = count_points(b);
	if (a_pts != b_pts) return (a_pts > b_pts) ? -1 : 1;

	// If a's and b's last names are not equal, return <0 if a comes first, >0 if b.
	if      (a_lname == NULL) return 1;     // Guard (should be unnecessary if not mis-used)
	else if (b_lname == NULL) return -1;
	ret = strcmp(a_lname, b_lname);
	if (ret != 0) return ret;

	// If a's and b's first names are not equal, return <0 if a comes first, >0 if b.
	if      (a_fname == NULL) return 1;     // Guard (should be unnecessary if not mis-used)
	else if (b_fname == NULL) return -1;
	ret = strcmp(a_fname, b_fname);
	if (ret != 0) return ret;

	// If all else identical, sort by student number.
	if      (a_num == NULL) return 1;       // Guard (should be unnecessary if not mis-used)
	else if (b_num == NULL) return -1;
	return strcmp(a_num, b_num);
}

/**
 * @brief Initializes a linked list of Students. Returns the head of the list, which will
 * remain as the head permanently. Student number, last name and first name are all NULL to
 * denote the head.
 *
 * @attention Must be free'd after use.
 *
 * @return Head of Student linked list, NULL if memory allocation failed.
 */
Student *init_linked_list(void) {
	Student *list_head = malloc(sizeof(Student));
	if (list_head == NULL) return NULL;     // Handle alloc failure

	list_head -> student_id = NULL;
	list_head -> lastname = NULL;
	list_head -> firstname = NULL;
	list_head -> next = NULL;

	return list_head;
}

/**
 * @brief Initializes a new Student instance.
 *
 * @attention Use free_student() to free memory allocated by this function.
 *
 * @param student_id
 * @param lastname
 * @param firstname
 * @return pointer to new Student instance, NULL if memory allocation failed
 */
Student *init_student(char *student_id, char *lastname, char *firstname) {
	char *student_num_ptr = malloc(STDNT_ID_LEN + 1);
	if (student_num_ptr == NULL) {  // Handle alloc failure
		return NULL;
	}
	strcpy(student_num_ptr, student_id);   // Init student number

	char *lastname_ptr = malloc(strlen(lastname) + 1);
	if (lastname_ptr == NULL) {     // Handle alloc failure
		free(student_num_ptr);
		return NULL;
	}
	strcpy(lastname_ptr, lastname);         // Init lastname

	char *firstname_ptr = malloc(strlen(firstname) + 1);
	if (firstname_ptr == NULL) {    // Handle alloc failure
		free(lastname_ptr);
		free(student_num_ptr);
		return NULL;
	}
	strcpy(firstname_ptr, firstname);       // Init firstname

	Student *new_student = malloc(sizeof(Student));
	if (new_student == NULL) {      // Handle alloc failure
		free(student_num_ptr);
		free(lastname_ptr);
		free(firstname_ptr);
		return NULL;
	}

	// Populate fields
	new_student -> student_id = student_num_ptr;
	new_student -> lastname = lastname_ptr;
	new_student -> firstname = firstname_ptr;
	for (int i = 0; i < EXCRS_RNDS; i++) new_student -> points[i] = 0;
	new_student -> next = NULL;

	return new_student;
}

/**
 * @brief Frees dynamically allocated memory allocated by init_student().
 *
 * @param student pointer to Student instance
 */
void free_student(Student *student) {
	free(student -> student_id);
	free(student -> lastname);
	free(student -> firstname);
	free(student);
}

/**
 * @brief Sorts the given Student into the right place in a linked list.
 *
 * @param student
 * @param students_head pointer to the head of the linked list
 */
void place_into_list(Student *student, Student *students_head) {
	Student *curr_student = students_head -> next;  // Current target
	Student *prev_student = students_head;          // Student before target

	// Keep comparing Student to others until sort_students() returns >0 or end of list
	while (curr_student != NULL && sort_students(curr_student, student) < 0) {
		prev_student = curr_student;
		curr_student = curr_student -> next;
	}

	prev_student -> next = student;     // Place given student
	student -> next = curr_student;     // Re-link rest of list
}

/**
 * @brief Creates a new Student instance and sorts it into a linked list. Assumes the given
 * linked list is already otherwise sorted. Assumes valid input.
 *
 * @attention The linked list must be first initialized using init_linked_list().
 *
 * @param student_id
 * @param lastname
 * @param firstname
 * @param students_head pointer to the head of the linked list
 * @return 0 if successful, error code otherwise
 */
int add_student(char *student_id, char *lastname, char *firstname, Student *students_head) {
	// Check the student ID is not already in use
	Student *curr_student = students_head -> next;
	while (curr_student != NULL) {
		if (strcmp(curr_student -> student_id, student_id) == 0) return ERR_STDNT_IN_LIST;
		curr_student = curr_student -> next;
	}

	// Allocate memory for new student and populate fields
	Student *new_student = init_student(student_id, lastname, firstname);
	if (new_student == NULL) return ERR_MEM_ALLOC_FAIL;     // Handle alloc failure

	// Sort into list
	place_into_list(new_student, students_head);

	return 0;
}

/**
 * @brief Frees all memory in the linked list onward from the given Student node.
 *
 * @param student pointer to a Student node from which free'ing starts
 */
void free_linked_list(Student *student) {
	Student *curr_student = student;
	Student *next_student;

	// Iterates through all nodes in the list
	while (curr_student != NULL) {
		next_student = curr_student -> next;
		free_student(curr_student);
		curr_student = next_student;
	}
}

/**
 * @brief Updates a Student's points and re-sorts the linked list. Assumes the linked list
 * is already otherwise sorted. Assumes valid input.
 *
 * @param student_id
 * @param round
 * @param points
 * @param students_head pointer to the head of the linked list
 * @return 0 if successful, error code otherwise
 */
int update_points(char *student_id, char *round, char *points, Student *students_head) {
	Student *trgt_student = students_head -> next;  // For holding target student
	Student *prev_student = students_head;          // Student before target

	// Handle exception: empty list
	if (trgt_student == NULL) return ERR_UPD_PTS_ON_EMPT;

	// Search for the target student by student number
	while (strcmp(trgt_student -> student_id, student_id) != 0) {
		prev_student = trgt_student;            // Keep track of previous Student
		trgt_student = trgt_student -> next;    // Move on to next

		if (trgt_student == NULL) return ERR_STDNT_NOT_FND; // Student not found error
	}

	// Convert strings into int
	int round_int = atoi(round);
	int points_int = atoi(points);

	// Update points
	trgt_student -> points[round_int - 1] = points_int;

	// Re-sort list
	prev_student -> next = trgt_student -> next;    // Bridge over target Student node
	place_into_list(trgt_student, students_head);   // Insert target Student back into list

	return 0;
}

/**
 * @brief Prints the given student's information on a single line in the given stream.
 *
 * @param stream
 * @param student
 * @return 0 if successful, error code otherwise
 */
int print_to_stream(FILE *stream, Student *student) {
	char *number = student -> student_id;
	char *lastname = student -> lastname;
	char *firstname = student -> firstname;

	if (stream == NULL) return ERR_NULL_STRM;     // Handle error

	// Prints student number, last name and first name
	fprintf(stream, "%s %s %s ", number, lastname, firstname);

	// Prints points for each round
	for (int i = 0; i < EXCRS_RNDS; i++) {
		fprintf(stream, "%d ", student -> points[i]);
	}

	// Prints total points
	fprintf(stream, "%d\n", count_points(student));

	return 0;
}

/**
 * @brief Prints the Students in the linked list into stdout.
 *
 * @param students_head pointer to the head of the linked list
 * @return 0 if successful, error code otherwise
 */
int print_status(Student *students_head) {
	Student *current_student = students_head -> next;

	// Go through linked list, printing each Student to stdout
	while (current_student != NULL) {
		int err = print_to_stream(stdout, current_student);
		if (err) return err;    // Handle error

		current_student = current_student -> next;
	}

	return 0;
}

/**
 * @brief Writes the Students in the linked list into file. Assumes valid input.
 *
 * @param filename
 * @param students_head pointer to the head of the linked list
 * @return 0 if successful, error code otherwise
 */
int write_to_file(char *filename, Student *students_head) {
	Student *curr_student = students_head -> next;  // Start from first non-dummy Student

	// Handle exception: empty list
	if (curr_student == NULL) return ERR_WRT_EMPT_LST;

	FILE *file = fopen(filename, "w");
	if (file == NULL) return ERR_FILE_OPEN;     // Handle error

	// Go through linked list, printing each Student to file
	while (curr_student != NULL) {
		int err = print_to_stream(file, curr_student);
		if (err) {  // Handle error
			fclose(file);
			return err;
		}

		curr_student = curr_student -> next;
	}

	fclose(file);
	return 0;
}

/**
 * @brief Loads file contents into memory. Rewrites the old linked list if file could be
 * read successfully. Leaves the old linked list intact if loading fails. Assumes valid
 * input.
 *
 * @param filename
 * @param students_head
 * @return 0 if successful, error code otherwise
 */
int load_file(char *filename, Student *students_head) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) return ERR_FILE_OPEN;     // Handle error

	int err = 0;    // Holds error codes which will terminate load operation

	Student *old_list = students_head -> next;  // Holds the previous linked list
	students_head -> next = NULL;               // Un-link the head node

	char buffer[INPUT_BUFFER_SIZE];

	// Keep reading lines from file until EOF or error flag gets set
	while ((fgets(buffer, sizeof(buffer), file) != NULL) && !err) {
		// Parse the line
		Input *parsed_inp = parse_input(buffer, &err, FALSE);
		if (parsed_inp == NULL) {   // Handle error
			break;
		}

		// Validate the read line
		err = validate_input(parsed_inp);
		if (err) {  // Handle error
			free_parsed_input(parsed_inp);
			break;
		}

		char **arg_arr = parsed_inp -> arg_arr;     // Just as a shorthand

		// Add student into list
		err = add_student(arg_arr[0], arg_arr[1], arg_arr[2], students_head);
		if (err) {  // Handle error
			if (err == ERR_STDNT_IN_LIST) err = ERR_FILE_CORR;  // Change of error code
			free_parsed_input(parsed_inp);
			break;
		}

		// Read student points
		for (int i = 0; i < EXCRS_RNDS; i++) {
			// Convert integer to string in preparation for update_points()
			char i_str[12];
			sprintf(i_str, "%d", i + 1);    // i + 1: "points[0]" means "1st round points"

			err = update_points(arg_arr[0], i_str, arg_arr[3 + i], students_head);
			if (err) break;     // Handle error. Breaks out of for-loop, not while-loop
		}

		free_parsed_input(parsed_inp);
	}

	fclose(file);

	// Start terminating procedure if error flag was set previously
	if (err) {
		free_linked_list(students_head -> next);    // Free the failed list
		students_head -> next = old_list;   // Re-link the old list to revert back
		return err;
	}

	free_linked_list(old_list);
	return 0;
}

/**
 * @brief Attempt to run the user given command.
 *
 * @param input modifiable user input string
 * @param students_head pointer to the head of the linked list
 * @return 0 if successful, 1 if QUIT command given, error code if unsuccessful
 */
int run(char *input, Student *students_head) {
	int err = 0;    // Holds return values

	// Parse the user's input
	Input *parsed_inp = parse_input(input, &err, TRUE);

	/* =====================================================================================
	Handles a special case to meet project requirements. This combination of err and non
	NULL return value is only used to print: "Invalid command <command char>\n" into
	stdout. Otherwise unnecessary and only serves to make code less readable.*/
	if (err && parsed_inp != NULL) {
		printf("Invalid command %c\n", parsed_inp -> cmnd);
		free(parsed_inp);
		return 0; // 0 return fools normal error detection to prevent another error printout
	}
	// =====================================================================================

	// Back to normal error handling
	if (parsed_inp == NULL) {
		if (err) return err;        // Handles known errors
		else return ERR_UNKNOWN;    // Handles unknown errors (should never happen)
	}

	// Validate the user's input
	err = validate_input(parsed_inp);
	if (err) {  // Return error code
		free_parsed_input(parsed_inp);
		return err;
	}

	// Both of these are just for shorthand
	char command = parsed_inp -> cmnd;
	char **arg_arr = parsed_inp -> arg_arr;

	switch(command) {
		case ADD:       // ADD: <'A'> <student ID> <last name> <first name>
			err = add_student(arg_arr[1], arg_arr[2], arg_arr[3], students_head);
			if (!err) puts("SUCCESS");
			break;

		case UPDATE:    // UPDATE: <'U'> <student ID> <round> <points>
			err = update_points(arg_arr[1], arg_arr[2], arg_arr[3], students_head);
			if (!err) puts("SUCCESS");
			break;

		case LIST:      // LIST: <'L'>
			err = print_status(students_head);
			if (!err) puts("SUCCESS");
			break;

		case WRITE:     // WRITE: <'W'> <file name>
			err = write_to_file(arg_arr[1], students_head);
			if (!err) puts("SUCCESS");
			break;

		case LOAD:      // LOAD: <'O'> <file name>
			err = load_file(arg_arr[1], students_head);
			if (!err) puts("SUCCESS");
			break;

		case QUIT:      // QUIT: <'Q'>
			err = QUIT_FLAG;
			puts("SUCCESS");
			break;

		default:        // Should never happen due to other checks
			err = ERR_UNKNOWN;
			break;
	}

	free_parsed_input(parsed_inp);
	return err;
}

/**
 * @brief Prints information to stdout about the given error code.
 *
 * @param err_code
 *
 * @note Error messages are defined in err_codes[]
 */
void print_error(int err_code) {
	// Holds the head and error message
	const char *head = NULL;
	const char *msg = NULL;

	// Search for the matching code
	for (int i = 0; i < NUM_ERRORS; i++) {
		if (err_codes[i].code == err_code) {
			head = err_codes[i].head;
			msg = err_codes[i].msg;
			break;
		}
	}

	// Print error
	printf("ERROR (%d) %s: %s\n", err_code, head, msg);
}

int main(void) {
	#ifndef TEST   // Only for testing purposes

	/* Initializes linked list. If init is unsuccessful, tries again repeatedly up to 10
	times. If init still unsuccessful, gives up and exits the program. */
	Student *students_head = NULL;
	for (int i = 0; students_head == NULL && i < 10; i++) {
		students_head = init_linked_list();
	}
	if (students_head == NULL) return ERR_CRITICAL;

	char input[INPUT_BUFFER_SIZE];      // Input buffer
	int ret = 0;                        // Holds return value from run()

	// Main program loop
	while (ret != QUIT_FLAG) {
		if (fgets(input, sizeof(input), stdin) != NULL) {   // Read user input from stdin
			ret = run(input, students_head);                // Attempt to run user input
		}
		else {
			ret = ERR_NON_VIABLE_INP;
		}

		// Prints possible error message
		if (ret < 0) print_error(ret);
	}

	free_linked_list(students_head);
	return 0;

	#else
	test();
	return 0;
	#endif
}
