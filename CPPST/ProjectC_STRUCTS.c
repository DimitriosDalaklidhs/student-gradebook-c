#include <stdio.h>
#include <string.h>

#define MAX_NAME 50
#define MAX_GRADES 10
#define MAX_STUDENTS 100
#define DATA_FILE "students.txt"

struct Student {
    char name[MAX_NAME];
    int id;
    float grades[MAX_GRADES];
    int grade_count; // how many grades are actually used
};

struct Student students[MAX_STUDENTS];
int student_count = 0;

/* ---- Function prototypes ---- */
void add_student(void);
void add_grade_to_student(void);
void list_students(void);
void show_student_details(void);
float calculate_average(struct Student s);
struct Student* find_student_by_id(int id);

void save_to_file(const char *filename);
void load_from_file(const char *filename);

/* Optional: clear leftover input from stdin */
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* discard */
    }
}

int main(void) {
    int choice;

    /* Try to load existing data */
    load_from_file(DATA_FILE);

    do {
        printf("\n--- Student Gradebook ---\n");
        printf("1. Add student\n");
        printf("2. Add grade to student\n");
        printf("3. List all students\n");
        printf("4. Show student details\n");
        printf("5. Save to file\n");
        printf("6. Load from file\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }

        if (choice == 1) {
            add_student();
        } else if (choice == 2) {
            add_grade_to_student();
        } else if (choice == 3) {
            list_students();
        } else if (choice == 4) {
            show_student_details();
        } else if (choice == 5) {
            save_to_file(DATA_FILE);
        } else if (choice == 6) {
            load_from_file(DATA_FILE);
        } else if (choice != 0) {
            printf("Unknown option.\n");
        }

    } while (choice != 0);

    /* Auto-save on exit (optional) */
    save_to_file(DATA_FILE);
    printf("Goodbye!\n");

    return 0;
}

/* ---- Core Functions ---- */

void add_student(void) {
    if (student_count >= MAX_STUDENTS) {
        printf("Cannot add more students (max reached).\n");
        return;
    }

    struct Student s;

    printf("Enter name: ");
    clear_input_buffer(); // clear leftover '\n' from previous scanf
    /* read up to 49 chars including spaces; stop at newline */
    if (fgets(s.name, sizeof(s.name), stdin) == NULL) {
        printf("Error reading name.\n");
        return;
    }
    /* Remove trailing newline if present */
    size_t len = strlen(s.name);
    if (len > 0 && s.name[len - 1] == '\n') {
        s.name[len - 1] = '\0';
    }

    printf("Enter id: ");
    if (scanf("%d", &s.id) != 1) {
        printf("Invalid ID input.\n");
        clear_input_buffer();
        return;
    }

    s.grade_count = 0;  // no grades yet

    students[student_count] = s;
    student_count++;

    printf("Student added!\n");
}

struct Student* find_student_by_id(int id) {
    for (int i = 0; i < student_count; i++) {
        if (students[i].id == id) {
            return &students[i];
        }
    }
    return NULL;
}

void add_grade_to_student(void) {
    int id;
    printf("Enter student ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID input.\n");
        clear_input_buffer();
        return;
    }

    struct Student *s = find_student_by_id(id);
    if (s == NULL) {
        printf("Student with ID %d not found.\n", id);
        return;
    }

    if (s->grade_count >= MAX_GRADES) {
        printf("This student already has the maximum number of grades.\n");
        return;
    }

    float grade;
    printf("Enter grade (0 - 100): ");
    if (scanf("%f", &grade) != 1) {
        printf("Invalid grade input.\n");
        clear_input_buffer();
        return;
    }

    if (grade < 0.0f || grade > 100.0f) {
        printf("Grade must be between 0 and 100.\n");
        return;
    }

    s->grades[s->grade_count] = grade;
    s->grade_count++;

    printf("Grade added to student %s (ID: %d).\n", s->name, s->id);
}

float calculate_average(struct Student s) {
    if (s.grade_count == 0) return 0.0f;

    float sum = 0.0f;
    for (int i = 0; i < s.grade_count; i++) {
        sum += s.grades[i];
    }
    return sum / s.grade_count;
}

void list_students(void) {
    if (student_count == 0) {
        printf("No students to show.\n");
        return;
    }

    printf("\n--- All Students ---\n");
    for (int i = 0; i < student_count; i++) {
        float avg = calculate_average(students[i]);
        printf("%d) Name: %s | ID: %d | Grades: %d | Average: %.2f\n",
               i + 1,
               students[i].name,
               students[i].id,
               students[i].grade_count,
               avg);
    }
}

void show_student_details(void) {
    int id;
    printf("Enter student ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID input.\n");
        clear_input_buffer();
        return;
    }

    struct Student *s = find_student_by_id(id);
    if (s == NULL) {
        printf("Student with ID %d not found.\n", id);
        return;
    }

    printf("\n--- Student Details ---\n");
    printf("Name: %s\n", s->name);
    printf("ID: %d\n", s->id);
    printf("Number of grades: %d\n", s->grade_count);

    if (s->grade_count == 0) {
        printf("No grades recorded yet.\n");
    } else {
        printf("Grades: ");
        for (int i = 0; i < s->grade_count; i++) {
            printf("%.2f ", s->grades[i]);
        }
        printf("\nAverage: %.2f\n", calculate_average(*s));
    }
}

/* ---- Save/Load to File (Level-up) ---- */

void save_to_file(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Error opening file '%s' for writing.\n", filename);
        return;
    }

    fprintf(f, "%d\n", student_count);
    for (int i = 0; i < student_count; i++) {
        struct Student *s = &students[i];
        fprintf(f, "%s\n", s->name);
        fprintf(f, "%d\n", s->id);
        fprintf(f, "%d\n", s->grade_count);
        for (int j = 0; j < s->grade_count; j++) {
            fprintf(f, "%.2f ", s->grades[j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    printf("Data saved to '%s'.\n", filename);
}

void load_from_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        // It's okay if file doesn't exist yet
        printf("No existing data file found ('%s'). Starting fresh.\n", filename);
        return;
    }

    int count;
    if (fscanf(f, "%d\n", &count) != 1) {
        printf("Error reading student count from file.\n");
        fclose(f);
        return;
    }

    if (count > MAX_STUDENTS) {
        printf("File has %d students, but max is %d. Truncating.\n", count, MAX_STUDENTS);
        count = MAX_STUDENTS;
    }

    student_count = 0; // reset current data

    for (int i = 0; i < count; i++) {
        struct Student s;

        // Read name line
        if (fgets(s.name, sizeof(s.name), f) == NULL) {
            printf("Error reading name for student %d.\n", i + 1);
            break;
        }
        // Remove newline
        size_t len = strlen(s.name);
        if (len > 0 && s.name[len - 1] == '\n') {
            s.name[len - 1] = '\0';
        }

        if (fscanf(f, "%d\n", &s.id) != 1) {
            printf("Error reading ID for student %d.\n", i + 1);
            break;
        }

        if (fscanf(f, "%d\n", &s.grade_count) != 1) {
            printf("Error reading grade count for student %d.\n", i + 1);
            break;
        }

        if (s.grade_count < 0) s.grade_count = 0;
        if (s.grade_count > MAX_GRADES) {
            printf("Warning: student %d has too many grades in file. Truncating to %d.\n",
                   i + 1, MAX_GRADES);
            s.grade_count = MAX_GRADES;
        }

        for (int j = 0; j < s.grade_count; j++) {
            if (fscanf(f, "%f", &s.grades[j]) != 1) {
                printf("Error reading grade %d for student %d.\n", j + 1, i + 1);
                s.grade_count = j;
                break;
            }
        }

        // consume the rest of the line with grades
        int c;
        while ((c = fgetc(f)) != '\n' && c != EOF) {
            /* discard */
        }

        students[student_count++] = s;
    }

    fclose(f);
    printf("Loaded %d students from '%s'.\n", student_count, filename);
}


