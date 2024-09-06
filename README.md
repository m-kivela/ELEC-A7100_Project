# ELEC-A7100 Basic course in C programming

This is the final programming project on Aalto University course ELEC-A7100 Basic course in C programming (summer 2024).

## Task description ("Course exercise points")

Implement a system that tracks the exercise points for a course. The course has six exercise rounds, for which points are tracked. For each  student registered to the course, the system is required to store:

- **student number**, which is a string that can contain at most 6 characters.

- **first name**, which is a string that can have arbitrary length.

- **last name**, which is a string that can have arbitrary length.

The program should be able to support arbitrary number of students.

System must be able to process the following commands:

- **Add student** command, which adds a new student to the database.

    1. The command has the following format:

            A <student-number> <surname> <name>

        where

        `<student-number>` is the student number, which must be valid as specified above.

        `<surname>` is the student’s last name.
    
        `<name>` is the student’s first name.

        For example, a valid add student command is as follows:

            A 234567 Jones Jack

    2. Initially the student has 0 points for all exercises.

    3. If the student is already in the database, the program must print an error message.

- **Update points** command, which updates the exercise round points of a student.

    1. The command has the following format:

            U <student-number> <round> <points>

        where

        `<student-number>` is the student number.

        `<round>` is one of the 6 rounds between 1 and 6 (inclusive). This argument must be an integer.
    
        `<points>` is the round points. This must be a non-negative integer.

        For example, a valid update points command is as follows:

            U 234567 2 6

    2. If the given student does not exist in the database, an error message should be printed.

- **Print database** command, which displays the database content.

    1. The command has the following format:

            L

    2. It prints the stored students on a separate line as follows:

        `<student-number> <surname> <name> <exercise-1> ... <exercise-6> <total-points>`

        For example, print database command output is as follows:

            234567 Jones Jack 0 6 0 0 0 0 6
            123 Joel Daniel 2 0 10 0 0 0 12
            SUCCESS

    3. The database entries should be printed in the order of total points such that the student with higher points will be printed first.

    The worth of this command is two points. The worth of printing all the database entries  is 1 point. If their order is also correct, you will get another point.

- **Save to file** command, which saves the database to a text file.

    1. The command has the following format:

            W <filename>

        where

        `<filename>` is the name of the text file.

    2. It writes the stored students on separate lines using the same formatting as **print database**.

    3. If an error occurs, it must print an error message.

- **Load from file** command, which loads students from a text file saved using **Save to file** command.

    1. The command has the following format:

            O <filename>

        where

        `<filename>` is the name of the text file.

    2. It assumes the stored students are on a separate line in the format specified above.

    3. If an error occurs, it should print an error message.

    4. If the file is valid and loaded correctly, the current database must be replaced with the entries loaded from the file.

- **Quit program** command, which releases all allocated memory and exits the program.

    1. The command has the following format:

            Q

    2. This operation must be implemented so that you can prevent memory leaks.

Below is an example of a possible command sequence:

    A 234567 Opiskelija Osmo
    A 111111 Ahkera Antti
    U 234567 1 7
    U 111111 1 14
    U 111111 2 12
    L
    W pisteet
    Q

As a result, Osmo Opiskelija will have altogether 7 points, and Antti Ahkera a total of 26 points. In the results table Antti Ahkera will be output first, and the the database will be stored to the text file “pisteet”.
