// File: grades.c

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "linked-list.h"
#include "grades.h"

/**
 * @brief Duplicates a string by allocating memory and copying its content
 * @param str The string to duplicate
 * @returns A pointer to the duplicated string, or NULL on failure
 */
static char* string_duplicate(const char* str) {
    size_t len = strlen(str) + 1; // Include space for the null terminator
    char* copy = malloc(len);
    if (copy) {
        memcpy(copy, str, len);
    }
    return copy;
}

/**
 * @brief Structure representing a course with name and grade
 */
struct course {
    char* name;
    int grade;
};

/**
 * @brief Structure representing a student with name, id and list of courses
 */
struct student {
    char* name;
    int id;
    struct list* courses;
};

/**
 * @brief Structure representing the grades system containing list of students
 */
struct grades {
    struct list* students;
};

/**
 * @brief Finds a student by their ID in the students list
 * @param students The list of students to search in
 * @param id The ID to search for
 * @returns Pointer to the found student, or NULL if not found
 */
static struct student* find_student_by_id(struct list* students, int id) {
    for (struct iterator* it = list_begin(students); it != NULL; it = list_next(it)) {
        struct student* student = list_get(it);
        if (student && student->id == id) {
            return student;
        }
    }
    return NULL;
}

/**
 * @brief Clones a course element
 * @param element The course to clone
 * @param output Where to store the cloned course
 * @returns 0 on success, 1 on failure
 */
static int clone_course(void* element, void** output) {
    struct course* orig = element;
    struct course* clone = malloc(sizeof(*clone));
    if (!clone) return 1;
    
    clone->name = string_duplicate(orig->name); // Use string_duplicate instead of strdup
    if (!clone->name) {
        free(clone);
        return 1;
    }
    clone->grade = orig->grade;
    *output = clone;
    return 0;
}

/**
 * @brief Destroys a course element and frees its memory
 * @param element The course to destroy
 */
static void destroy_course(void* element) {
    if (!element) return;
    struct course* course = element;
    free(course->name);
    free(course);
}

/**
 * @brief Clones a student element including all their courses
 * @param element The student to clone
 * @param output Where to store the cloned student
 * @returns 0 on success, 1 on failure
 */
static int clone_student(void* element, void** output) {
    struct student* orig = element;
    struct student* clone = malloc(sizeof(*clone));
    if (!clone) return 1;
    
    clone->name = string_duplicate(orig->name); // Use string_duplicate instead of strdup
    if (!clone->name) {
        free(clone);
        return 1;
    }
    
    clone->id = orig->id;
    clone->courses = list_init(clone_course, destroy_course);
    if (!clone->courses) {
        free(clone->name);
        free(clone);
        return 1;
    }
    
    // Clone all courses from original student
    struct iterator* it = list_begin(orig->courses);
    while (it) {
        if (list_push_back(clone->courses, list_get(it))) {
            list_destroy(clone->courses);
            free(clone->name);
            free(clone);
            return 1;
        }
        it = list_next(it);
    }
    
    *output = clone;
    return 0;
}

/**
 * @brief Destroys a student element and frees all associated memory
 * @param element The student to destroy
 */
static void destroy_student(void* element) {
    if (!element) return;
    struct student* student = element;
    free(student->name);
    list_destroy(student->courses);
    free(student);
}

struct grades* grades_init() {
    struct grades* grades = malloc(sizeof(*grades));
    if (!grades) return NULL;
    
    grades->students = list_init(clone_student, destroy_student);
    if (!grades->students) {
        free(grades);
        return NULL;
    }
    return grades;
}

void grades_destroy(struct grades *grades) {
    if (!grades) return;
    list_destroy(grades->students);
    free(grades);
}

int grades_add_student(struct grades *grades, const char *name, int id) {
    if (!grades || !name) return 1;
    
    // Check for existing student using helper function
    if (find_student_by_id(grades->students, id)) {
        return 1;
    }
    
    struct student* new_student = malloc(sizeof(*new_student));
    if (!new_student) return 1;
    
    new_student->name = string_duplicate(name); // Use string_duplicate instead of strdup
    if (!new_student->name) {
        free(new_student);
        return 1;
    }
    
    new_student->id = id;
    new_student->courses = list_init(clone_course, destroy_course);
    if (!new_student->courses) {
        free(new_student->name);
        free(new_student);
        return 1;
    }
    
    if (list_push_back(grades->students, new_student)) {
        list_destroy(new_student->courses);
        free(new_student->name);
        free(new_student);
        return 1;
    }
    
    destroy_student(new_student);  // list_push_back creates a copy
    return 0;
}

int grades_add_grade(struct grades *grades, const char *name, int id, int grade) {
    if (!grades || !name || grade < 0 || grade > 100) return 1;
    
    struct student* student = find_student_by_id(grades->students, id);
    if (!student) return 1;
    
    // Check for duplicate course name
    for (struct iterator* it = list_begin(student->courses); it != NULL; it = list_next(it)) {
        struct course* course = list_get(it);
        if (strcmp(course->name, name) == 0) {
            return 1;
        }
    }
    
    struct course* new_course = malloc(sizeof(*new_course));
    if (!new_course) return 1;
    
    new_course->name = string_duplicate(name); // Use string_duplicate instead of strdup
    if (!new_course->name) {
        free(new_course);
        return 1;
    }
    
    new_course->grade = grade;
    
    if (list_push_back(student->courses, new_course)) {
        free(new_course->name);
        free(new_course);
        return 1;
    }
    
    destroy_course(new_course);
    return 0;
}

float grades_calc_avg(struct grades *grades, int id, char **out) {
    if (!grades || !out) {
        if (out) *out = NULL;
        return -1;
    }
    
    struct student* student = find_student_by_id(grades->students, id);
    if (!student) {
        *out = NULL;
        return -1;
    }
    
    size_t course_count = list_size(student->courses);
    if (course_count == 0) {
        *out = string_duplicate(student->name); // Use string_duplicate instead of strdup
        return (*out) ? 0 : -1;
    }
    
    float sum = 0;
    for (struct iterator* it = list_begin(student->courses); it != NULL; it = list_next(it)) {
        struct course* course = list_get(it);
        sum += course->grade;
    }
    
    *out = string_duplicate(student->name); // Use string_duplicate instead of strdup
    if (!*out) return -1;
    
    return sum / course_count;
}

int grades_print_student(struct grades *grades, int id) {
    if (!grades) return 1;
    
    struct student* student = find_student_by_id(grades->students, id);
    if (!student) return 1;
    
    printf("%s %d:", student->name, student->id);
    
    for (struct iterator* it = list_begin(student->courses); it != NULL; it = list_next(it)) {
        struct course* course = list_get(it);
        printf(" %s %d", course->name, course->grade);
        if (list_next(it)) printf(",");
    }
    printf("\n");
    
    return 0;
}

int grades_print_all(struct grades *grades) {
    if (!grades) return 1;
    
    struct iterator* it = list_begin(grades->students);
    while (it) {
        struct student* student = list_get(it);
        if (grades_print_student(grades, student->id)) {
            return 1;
        }
        it = list_next(it);
    }
    
    return 0;
}
