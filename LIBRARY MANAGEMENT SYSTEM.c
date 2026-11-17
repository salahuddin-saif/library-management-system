
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define WIDTH 80
#define BOOK_FILE "books.txt"
#define MEMBER_FILE "members.txt"
#define ADMIN_FILE "admins.txt"

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

/* =====================================================
   1. DATA STRUCTURES
   ===================================================== */

/* BOOK - BINARY SEARCH TREE */

typedef struct Book {
    int id;
    char title[100];
    char author[100];
    char publisher[100];
    int year;
    int available;
    int borrowedBy;
    char auditInfo[180]; /* Who added/updated/issued/returned this book */

    struct Book *left;
    struct Book *right;
} Book;

/* MEMBER - LINKED LIST */

typedef struct Member {
    int id;
    char name[100];
    char phone[30];
    char email[100];
    char auditInfo[180]; /* Who added/updated/borrowed/returned */

    struct Member *next;
} Member;

/* GLOBAL POINTERS AND CURRENT LOGIN */

Book *root = NULL;
Member *head = NULL;
char currentAdmin[100] = "";

/* =====================================================
   2. FUNCTION DECLARATIONS
   All functions are declared before their definitions.
   ===================================================== */

/* COMMON UI */

void clearScreen(void);
void centerText(const char *text);
void line(void);
void heading(const char *title);
int getInt(const char *message);
void getString(const char *message, char *value, int size);
void pressEnter(void);
void welcomeScreen(void);

/* BOOK BST */

Book *insertBook(Book *node, Book *newBook);
Book *searchBookByID(Book *node, int id);
Book *findMin(Book *node);
Book *deleteBook(Book *node, int id);
int containsText(const char *text, const char *keyword);
void displayBook(Book *book);
void displayAllBooks(Book *node);
void searchTitleRecursive(Book *node, const char *title, int *found);
void countBooks(Book *node, int *available, int *borrowed);

/* BOOK MANAGEMENT */

void addBook(void);
void updateBook(void);
void removeBook(void);
void viewBooks(void);
void searchByID(void);
void searchByTitle(void);
void bookStatistics(void);

/* MEMBER LINKED LIST */

Member *searchMember(int id);
void addMember(void);
void viewMembers(void);
void searchMemberInfo(void);
void updateMember(void);
int hasBorrowedBook(Book *node, int memberID);
void deleteMember(void);

/* ISSUE AND RETURN */

void issueBook(void);
void returnBook(void);

/* FILE HANDLING */

void saveBooks(Book *node, FILE *file);
void saveBookFile(void);
void loadBookFile(void);
void saveMemberFile(void);
void loadMemberFile(void);
void saveAllData(void);

/* ADMIN AUTHENTICATION AND ACTIVITY LOG */

unsigned long hashPassword(const char *password);
int usernameExists(const char *username);
void registerAdmin(void);
int loginAdmin(void);
int authenticationMenu(void);

/* MENUS */

void bookMenu(void);
void searchMenu(void);
void memberMenu(void);
void mainMenu(void);

/* =====================================================
   3. COMMON USER INTERFACE FUNCTIONS
   ===================================================== */

/* CLEAR SCREEN */

void clearScreen(void) {
    system(CLEAR_SCREEN);
}

/* CENTER TEXT */

void centerText(const char *text) {

    int spaces;

    spaces = (WIDTH - (int)strlen(text)) / 2;

    if (spaces < 0)
        spaces = 0;

    printf("%*s%s\n", spaces, "", text);
}

/* DIVIDER */

void line(void) {
    centerText("================================================");
}

/* PAGE HEADING */

void heading(const char *title) {

    clearScreen();

    printf("\n\n");

    line();
    centerText("LIBRARY MANAGEMENT SYSTEM");
    line();

    printf("\n");

    centerText(title);

    printf("\n");

    line();

    printf("\n");
}

/* INTEGER INPUT */

int getInt(const char *message) {

    char input[100];
    char extra;
    int value;

    while (1) {

        printf("\n");

        centerText(message);

        printf("\n                    > ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            continue;

        if (sscanf(input, " %d %c", &value, &extra) == 1)
            return value;

        centerText("Invalid input! Enter a valid number.");
    }
}

/* STRING INPUT */

void getString(const char *message, char *value, int size) {

    printf("\n");

    centerText(message);

    printf("\n                    > ");

    if (fgets(value, size, stdin) != NULL) {

        value[strcspn(value, "\n")] = '\0';
    }
}

/* PRESS ENTER */

void pressEnter(void) {

    printf("\n");

    centerText("Press ENTER to continue...");

    getchar();
}

/* WELCOME SCREEN */

void welcomeScreen(void) {

    clearScreen();

    printf("\n\n\n");

    line();

    centerText("WELCOME TO");

    centerText("LIBRARY MANAGEMENT SYSTEM");

    line();

    printf("\n");

    centerText("Manage Books, Members and Library Records");


    printf("\n\n");

    pressEnter();
}

/* =====================================================
   4. BST - BOOK INSERTION
   ===================================================== */

Book *insertBook(Book *node, Book *newBook) {

    if (node == NULL)
        return newBook;

    if (newBook->id < node->id) {

        node->left = insertBook(node->left, newBook);

    }
    else if (newBook->id > node->id) {

        node->right = insertBook(node->right, newBook);

    }

    return node;
}

/* =====================================================
   5. BST - SEARCH BOOK BY ID
   ===================================================== */

Book *searchBookByID(Book *node, int id) {

    if (node == NULL || node->id == id)
        return node;

    if (id < node->id)
        return searchBookByID(node->left, id);

    return searchBookByID(node->right, id);
}

/* =====================================================
   6. BOOK TITLE SEARCH
   ===================================================== */

/* CASE-INSENSITIVE SEARCH */

int containsText(const char *text, const char *keyword) {

    char tempText[100];
    char tempKeyword[100];
    int i;

    snprintf(tempText, sizeof(tempText), "%s", text);

    snprintf(tempKeyword, sizeof(tempKeyword), "%s", keyword);

    for (i = 0; tempText[i] != '\0'; i++) {

        tempText[i] = (char)tolower((unsigned char)tempText[i]);
    }

    for (i = 0; tempKeyword[i] != '\0'; i++) {

        tempKeyword[i] = (char)tolower((unsigned char)tempKeyword[i]);
    }

    return strstr(tempText, tempKeyword) != NULL;
}

/* =====================================================
   7. BOOK DISPLAY
   ===================================================== */

/* DISPLAY SINGLE BOOK */

void displayBook(Book *book) {

    char buffer[200];

    if (book == NULL)
        return;

    printf("\n");

    centerText("-----------------------------------------------");

    sprintf(buffer, "Book ID     : %d", book->id);
    centerText(buffer);

    sprintf(buffer, "Title       : %s", book->title);
    centerText(buffer);

    sprintf(buffer, "Author      : %s", book->author);
    centerText(buffer);

    sprintf(buffer, "Publisher   : %s", book->publisher);
    centerText(buffer);

    sprintf(buffer, "Year        : %d", book->year);
    centerText(buffer);

    sprintf(buffer, "Status      : %s",
            book->available ? "Available" : "Borrowed");

    centerText(buffer);

    if (!book->available) {

        sprintf(buffer, "Borrowed By : Member ID %d",
                book->borrowedBy);

        centerText(buffer);
    }

    sprintf(buffer, "Record Info : %.160s", book->auditInfo);
    centerText(buffer);

    centerText("-----------------------------------------------");
}

/* DISPLAY ALL BOOKS USING INORDER TRAVERSAL */

void displayAllBooks(Book *node) {

    if (node == NULL)
        return;

    displayAllBooks(node->left);

    displayBook(node);

    displayAllBooks(node->right);
}

/* SEARCH TITLE RECURSIVELY */

void searchTitleRecursive(Book *node, const char *title, int *found) {

    if (node == NULL)
        return;

    searchTitleRecursive(node->left, title, found);

    if (containsText(node->title, title)) {

        displayBook(node);

        *found = 1;
    }

    searchTitleRecursive(node->right, title, found);
}

/* =====================================================
   8. BST - FIND MINIMUM NODE
   ===================================================== */

Book *findMin(Book *node) {

    while (node != NULL && node->left != NULL) {

        node = node->left;
    }

    return node;
}

/* =====================================================
   9. BST - DELETE BOOK
   ===================================================== */

Book *deleteBook(Book *node, int id) {

    Book *temp;

    if (node == NULL)
        return NULL;

    if (id < node->id) {

        node->left = deleteBook(node->left, id);

    }
    else if (id > node->id) {

        node->right = deleteBook(node->right, id);

    }
    else {

        /* NO LEFT CHILD */

        if (node->left == NULL) {

            temp = node->right;

            free(node);

            return temp;
        }

        /* NO RIGHT CHILD */

        if (node->right == NULL) {

            temp = node->left;

            free(node);

            return temp;
        }

        /* TWO CHILDREN */

        temp = findMin(node->right);

        node->id = temp->id;

        strcpy(node->title, temp->title);

        strcpy(node->author, temp->author);

        strcpy(node->publisher, temp->publisher);

        node->year = temp->year;

        node->available = temp->available;

        node->borrowedBy = temp->borrowedBy;

        node->right = deleteBook(node->right, temp->id);
    }

    return node;
}

/* =====================================================
   10. BOOK MANAGEMENT - ADD BOOK
   ===================================================== */

void addBook(void) {

    Book *book;

    heading("ADD NEW BOOK");

    book = (Book *)malloc(sizeof(Book));

    if (book == NULL) {

        centerText("Memory allocation failed!");

        pressEnter();

        return;
    }

    book->id = getInt("Enter Book ID:");

    if (book->id <= 0) {

        centerText("Book ID must be positive!");

        free(book);

        pressEnter();

        return;
    }

    if (searchBookByID(root, book->id) != NULL) {

        centerText("Book ID already exists!");

        free(book);

        pressEnter();

        return;
    }

    getString("Enter Book Title:", book->title, sizeof(book->title));

    getString("Enter Author Name:", book->author, sizeof(book->author));

    getString("Enter Publisher:", book->publisher, sizeof(book->publisher));

    book->year = getInt("Enter Publication Year:");

    book->available = 1;

    book->borrowedBy = 0;
    snprintf(book->auditInfo, sizeof(book->auditInfo), "Added by %s", currentAdmin);

    book->left = NULL;

    book->right = NULL;

    root = insertBook(root, book);

    saveAllData();

    centerText("BOOK ADDED SUCCESSFULLY!");

    pressEnter();
}

/* =====================================================
   11. BOOK MANAGEMENT - UPDATE BOOK
   ===================================================== */

void updateBook(void) {

    int id;

    Book *book;

    heading("UPDATE BOOK");

    id = getInt("Enter Book ID:");

    book = searchBookByID(root, id);

    if (book == NULL) {

        centerText("Book not found!");

        pressEnter();

        return;
    }

    if (!book->available) {

        centerText("This book is currently borrowed!");

        centerText("Return the book before updating.");

        pressEnter();

        return;
    }

    getString("Enter New Title:", book->title, sizeof(book->title));

    getString("Enter New Author:", book->author, sizeof(book->author));

    getString("Enter New Publisher:", book->publisher, sizeof(book->publisher));

    book->year = getInt("Enter New Publication Year:");
    snprintf(book->auditInfo, sizeof(book->auditInfo), "Updated by %s", currentAdmin);

    saveAllData();

    centerText("BOOK UPDATED SUCCESSFULLY!");

    pressEnter();
}

/* =====================================================
   12. BOOK MANAGEMENT - DELETE BOOK
   ===================================================== */

void removeBook(void) {

    int id;

    Book *book;

    heading("DELETE BOOK");

    id = getInt("Enter Book ID to Delete:");

    book = searchBookByID(root, id);

    if (book == NULL) {

        centerText("Book not found!");

    }
    else if (!book->available) {

        centerText("Cannot delete this book!");

        centerText("This book is currently borrowed.");

    }
    else {

        root = deleteBook(root, id);

        saveAllData();

        centerText("BOOK DELETED SUCCESSFULLY!");
    }

    pressEnter();
}

/* =====================================================
   13. BOOK MANAGEMENT - VIEW ALL BOOKS
   ===================================================== */

void viewBooks(void) {

    heading("ALL BOOK RECORDS");

    if (root == NULL) {

        centerText("No books available!");

    }
    else {

        displayAllBooks(root);
    }

    pressEnter();
}

/* =====================================================
   14. BOOK SEARCH - SEARCH BY ID
   ===================================================== */

void searchByID(void) {

    int id;

    Book *book;

    heading("SEARCH BOOK BY ID");

    id = getInt("Enter Book ID:");

    book = searchBookByID(root, id);

    if (book == NULL) {

        centerText("Book not found!");

    }
    else {

        displayBook(book);
    }

    pressEnter();
}

/* =====================================================
   15. BOOK SEARCH - SEARCH BY TITLE
   ===================================================== */

void searchByTitle(void) {

    char title[100];

    int found = 0;

    heading("SEARCH BOOK BY TITLE");

    getString("Enter Book Title or Keyword:", title, sizeof(title));

    if (strlen(title) == 0) {

        centerText("Search keyword cannot be empty!");

    }
    else {

        searchTitleRecursive(root, title, &found);

        if (!found) {

            centerText("No matching book found!");
        }
    }

    pressEnter();
}

/* =====================================================
   16. BOOK AVAILABILITY
   ===================================================== */

/* COUNT AVAILABLE AND BORROWED BOOKS */

void countBooks(Book *node, int *available, int *borrowed) {

    if (node == NULL)
        return;

    countBooks(node->left, available, borrowed);

    if (node->available)
        (*available)++;
    else
        (*borrowed)++;

    countBooks(node->right, available, borrowed);
}

/* SHOW BOOK STATISTICS */

void bookStatistics(void) {

    int available = 0;

    int borrowed = 0;

    char buffer[100];

    heading("BOOK AVAILABILITY");

    countBooks(root, &available, &borrowed);

    sprintf(buffer, "Total Books     : %d", available + borrowed);

    centerText(buffer);

    sprintf(buffer, "Available Books : %d", available);

    centerText(buffer);

    sprintf(buffer, "Borrowed Books  : %d", borrowed);

    centerText(buffer);

    printf("\n");

    line();

    pressEnter();
}

/* =====================================================
   17. LINKED LIST - SEARCH MEMBER
   ===================================================== */

Member *searchMember(int id) {

    Member *temp = head;

    while (temp != NULL) {

        if (temp->id == id)
            return temp;

        temp = temp->next;
    }

    return NULL;
}

/* =====================================================
   18. MEMBER MANAGEMENT - ADD MEMBER
   ===================================================== */

void addMember(void) {

    Member *newMember;

    heading("ADD NEW MEMBER");

    newMember = (Member *)malloc(sizeof(Member));

    if (newMember == NULL) {

        centerText("Memory allocation failed!");

        pressEnter();

        return;
    }

    newMember->id = getInt("Enter Member ID:");

    if (newMember->id <= 0) {

        centerText("Member ID must be positive!");

        free(newMember);

        pressEnter();

        return;
    }

    if (searchMember(newMember->id) != NULL) {

        centerText("Member ID already exists!");

        free(newMember);

        pressEnter();

        return;
    }

    getString("Enter Member Name:", newMember->name, sizeof(newMember->name));

    getString("Enter Phone Number:", newMember->phone, sizeof(newMember->phone));

    getString("Enter Email Address:", newMember->email, sizeof(newMember->email));
    snprintf(newMember->auditInfo, sizeof(newMember->auditInfo), "Added by %s", currentAdmin);

    newMember->next = head;

    head = newMember;

    saveAllData();

    centerText("MEMBER ADDED SUCCESSFULLY!");

    pressEnter();
}

/* =====================================================
   19. MEMBER MANAGEMENT - VIEW MEMBERS
   ===================================================== */

void viewMembers(void) {

    Member *temp;

    char buffer[200];

    heading("ALL MEMBER RECORDS");

    temp = head;

    if (temp == NULL) {

        centerText("No members found!");

    }

    while (temp != NULL) {

        centerText("-----------------------------------------------");

        sprintf(buffer, "Member ID : %d", temp->id);

        centerText(buffer);

        sprintf(buffer, "Name      : %s", temp->name);

        centerText(buffer);

        sprintf(buffer, "Phone     : %s", temp->phone);

        centerText(buffer);

        sprintf(buffer, "Email     : %s", temp->email);

        centerText(buffer);

        sprintf(buffer, "Record Info: %.160s", temp->auditInfo);
        centerText(buffer);

        centerText("-----------------------------------------------");

        temp = temp->next;
    }

    pressEnter();
}

/* =====================================================
   20. MEMBER MANAGEMENT - SEARCH MEMBER
   ===================================================== */

void searchMemberInfo(void) {

    int id;

    Member *member;

    char buffer[200];

    heading("SEARCH MEMBER");

    id = getInt("Enter Member ID:");

    member = searchMember(id);

    if (member == NULL) {

        centerText("Member not found!");

    }
    else {

        sprintf(buffer, "Member ID : %d", member->id);

        centerText(buffer);

        sprintf(buffer, "Name      : %s", member->name);

        centerText(buffer);

        sprintf(buffer, "Phone     : %s", member->phone);

        centerText(buffer);

        sprintf(buffer, "Email     : %s", member->email);

        centerText(buffer);
        sprintf(buffer, "Record Info: %.160s", member->auditInfo);
        centerText(buffer);
    }

    pressEnter();
}

/* =====================================================
   21. MEMBER MANAGEMENT - UPDATE MEMBER
   ===================================================== */

void updateMember(void) {

    int id;

    Member *member;

    heading("UPDATE MEMBER");

    id = getInt("Enter Member ID:");

    member = searchMember(id);

    if (member == NULL) {

        centerText("Member not found!");

        pressEnter();

        return;
    }

    getString("Enter New Name:", member->name, sizeof(member->name));

    getString("Enter New Phone:", member->phone, sizeof(member->phone));

    getString("Enter New Email:", member->email, sizeof(member->email));
    snprintf(member->auditInfo, sizeof(member->auditInfo), "Updated by %s", currentAdmin);

    saveAllData();

    centerText("MEMBER UPDATED SUCCESSFULLY!");

    pressEnter();
}

/* =====================================================
   22. MEMBER DELETE - CHECK BORROWED BOOK
   ===================================================== */

/* CHECK WHETHER MEMBER HAS AN UNRETURNED BOOK */

int hasBorrowedBook(Book *node, int memberID) {

    if (node == NULL)
        return 0;

    if (!node->available && node->borrowedBy == memberID)
        return 1;

    return hasBorrowedBook(node->left, memberID) ||
           hasBorrowedBook(node->right, memberID);
}

/* =====================================================
   23. MEMBER MANAGEMENT - DELETE MEMBER
   ===================================================== */

void deleteMember(void) {

    int id;

    Member *temp;

    Member *previous = NULL;

    heading("DELETE MEMBER");

    id = getInt("Enter Member ID:");

    temp = head;

    while (temp != NULL && temp->id != id) {

        previous = temp;

        temp = temp->next;
    }

    if (temp == NULL) {

        centerText("Member not found!");

    }
    else if (hasBorrowedBook(root, id)) {

        centerText("CANNOT DELETE MEMBER!");

        centerText("Member has an unreturned book.");

        centerText("Please return the book first.");

    }
    else {

        if (previous == NULL)
            head = temp->next;

        else
            previous->next = temp->next;

        free(temp);

        saveAllData();

        centerText("MEMBER DELETED SUCCESSFULLY!");
    }

    pressEnter();
}

/* =====================================================
   24. BOOK ISSUE MANAGEMENT
   ===================================================== */

void issueBook(void) {

    int bookID;

    int memberID;

    Book *book;

    Member *member;

    char buffer[100];

    heading("ISSUE BOOK");

    bookID = getInt("Enter Book ID:");

    memberID = getInt("Enter Member ID:");

    book = searchBookByID(root, bookID);

    member = searchMember(memberID);

    /* CHECK BOOK */

    if (book == NULL) {

        centerText("Book not found!");

    }

    /* CHECK MEMBER */

    else if (member == NULL) {

        centerText("Member not found!");

    }

    /* PREVENT DOUBLE ISSUE */

    else if (book->available == 0) {

        centerText("BOOK IS ALREADY BORROWED!");

        sprintf(buffer, "Borrowed By Member ID: %d", book->borrowedBy);

        centerText(buffer);

        centerText("Another member cannot issue this book.");

    }

    /* SUCCESSFUL ISSUE */

    else {

        book->available = 0;

        book->borrowedBy = memberID;
        snprintf(book->auditInfo, sizeof(book->auditInfo), "Issued by %.55s to %.55s (Member ID %d)", currentAdmin, member->name, member->id);
        snprintf(member->auditInfo, sizeof(member->auditInfo), "Borrowed %.65s (ID %d), issued by %.55s", book->title, book->id, currentAdmin);

        saveAllData();

        centerText("BOOK ISSUED SUCCESSFULLY!");

        displayBook(book);
    }

    pressEnter();
}

/* =====================================================
   25. BOOK RETURN MANAGEMENT
   ===================================================== */

void returnBook(void) {
    int id;
    int oldMemberID;
    Book *book;
    Member *oldMember;

    heading("RETURN BOOK");
    id = getInt("Enter Book ID:");
    book = searchBookByID(root, id);

    if (book == NULL) {
        centerText("Book not found!");
    }
    else if (book->available == 1) {
        centerText("This book is already available!");
    }
    else {
        oldMemberID = book->borrowedBy;
        oldMember = searchMember(oldMemberID);
        book->available = 1;
        book->borrowedBy = 0;
        snprintf(book->auditInfo, sizeof(book->auditInfo),
                 "Returned by %.100s from Member ID %d", currentAdmin, oldMemberID);
        if (oldMember != NULL) {
            snprintf(oldMember->auditInfo, sizeof(oldMember->auditInfo),
                     "Returned %.65s (ID %d), received by %.55s",
                     book->title, book->id, currentAdmin);
        }
        saveAllData();
        centerText("BOOK RETURNED SUCCESSFULLY!");
        centerText("This book is now available for other members.");
    }
    pressEnter();
}

/* =====================================================
   26. FILE HANDLING - SAVE BOOKS
   ===================================================== */

/* SAVE BST RECORDS USING INORDER TRAVERSAL */

void saveBooks(Book *node, FILE *file) {

    if (node == NULL)
        return;

    saveBooks(node->left, file);

    fprintf(file, "%d|%s|%s|%s|%d|%d|%d|%s\n",
            node->id, node->title, node->author, node->publisher,
            node->year, node->available, node->borrowedBy, node->auditInfo);

    saveBooks(node->right, file);
}

/* SAVE BOOK DATA */

void saveBookFile(void) {

    FILE *file;

    file = fopen(BOOK_FILE, "w");

    if (file == NULL)
        return;

    saveBooks(root, file);

    fclose(file);
}

/* =====================================================
   27. FILE HANDLING - LOAD BOOKS
   ===================================================== */

void loadBookFile(void) {

    FILE *file;

    char buffer[500];

    file = fopen(BOOK_FILE, "r");

    if (file == NULL)
        return;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {

        Book *book;

        book = (Book *)malloc(sizeof(Book));

        if (book == NULL)
            break;

        book->auditInfo[0] = '\0';
        if (sscanf(buffer,
                   "%d|%99[^|]|%99[^|]|%99[^|]|%d|%d|%d|%179[^\n]",
                   &book->id, book->title, book->author, book->publisher,
                   &book->year, &book->available, &book->borrowedBy,
                   book->auditInfo) >= 7) {
            if (book->auditInfo[0] == '\0')
                strcpy(book->auditInfo, "Older record - admin information unavailable");

            if (book->id > 0 &&
                (book->available == 0 || book->available == 1) &&
                searchBookByID(root, book->id) == NULL) {

                book->left = NULL;

                book->right = NULL;

                root = insertBook(root, book);

            }
            else {

                free(book);
            }
        }
        else {

            free(book);
        }
    }

    fclose(file);
}

/* =====================================================
   28. FILE HANDLING - SAVE MEMBERS
   ===================================================== */

void saveMemberFile(void) {

    FILE *file;

    Member *temp;

    file = fopen(MEMBER_FILE, "w");

    if (file == NULL)
        return;

    temp = head;

    while (temp != NULL) {

        fprintf(file, "%d|%s|%s|%s|%s\n",
                temp->id, temp->name, temp->phone, temp->email, temp->auditInfo);

        temp = temp->next;
    }

    fclose(file);
}

/* =====================================================
   29. FILE HANDLING - LOAD MEMBERS
   ===================================================== */

void loadMemberFile(void) {

    FILE *file;

    char buffer[400];

    file = fopen(MEMBER_FILE, "r");

    if (file == NULL)
        return;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {

        Member *member;

        member = (Member *)malloc(sizeof(Member));

        if (member == NULL)
            break;

        member->auditInfo[0] = '\0';
        if (sscanf(buffer,
                   "%d|%99[^|]|%29[^|]|%99[^|]|%179[^\n]",
                   &member->id, member->name, member->phone,
                   member->email, member->auditInfo) >= 4) {
            if (member->auditInfo[0] == '\0')
                strcpy(member->auditInfo, "Older record - admin information unavailable");

            if (member->id > 0 &&
                searchMember(member->id) == NULL) {

                member->next = head;

                head = member;

            }
            else {

                free(member);
            }
        }
        else {

            free(member);
        }
    }

    fclose(file);
}

/* =====================================================
   30. FILE HANDLING - SAVE ALL DATA
   ===================================================== */

void saveAllData(void) {

    saveBookFile();

    saveMemberFile();
}

/* =====================================================
   ADMIN AUTHENTICATION AND ACTIVITY LOG
   Passwords are stored as a simple hash for this academic
   console project. This is not production-grade security.
   ===================================================== */

/* SIMPLE FNV-1a HASH (NOT A PASSWORD-SECURITY STANDARD) */

unsigned long hashPassword(const char *password) {
    unsigned long hash = 2166136261UL;
    int i;

    for (i = 0; password[i] != '\0'; i++) {
        hash ^= (unsigned char)password[i];
        hash = (hash * 16777619UL) & 0xFFFFFFFFUL;
    }
    return hash;
}

/* CHECK WHETHER USERNAME ALREADY EXISTS */

int usernameExists(const char *username) {
    FILE *file;
    char savedUsername[50];
    char savedName[100];
    unsigned long savedHash;

    file = fopen(ADMIN_FILE, "r");
    if (file == NULL)
        return 0;

    while (fscanf(file, "%49[^|]|%99[^|]|%lu\n",
                  savedUsername, savedName, &savedHash) == 3) {
        if (strcmp(savedUsername, username) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* ADMIN REGISTRATION */

void registerAdmin(void) {
    FILE *file;
    char username[50];
    char name[100];
    char password[100];
    char confirmPassword[100];
    unsigned long passwordHash;

    heading("ADMIN REGISTRATION");

    getString("Enter Admin Username (no | character):",
              username, sizeof(username));

    if (strlen(username) == 0 || strchr(username, '|') != NULL) {
        centerText("Invalid username!");
        pressEnter();
        return;
    }

    if (usernameExists(username)) {
        centerText("This username is already registered!");
        pressEnter();
        return;
    }

    getString("Enter Admin Full Name:", name, sizeof(name));
    if (strlen(name) == 0 || strchr(name, '|') != NULL) {
        centerText("Invalid admin name!");
        pressEnter();
        return;
    }

    getString("Enter Password:", password, sizeof(password));
    getString("Confirm Password:", confirmPassword, sizeof(confirmPassword));

    if (strlen(password) < 4) {
        centerText("Password must contain at least 4 characters!");
        pressEnter();
        return;
    }

    if (strcmp(password, confirmPassword) != 0) {
        centerText("Password does not match!");
        pressEnter();
        return;
    }

    passwordHash = hashPassword(password);
    file = fopen(ADMIN_FILE, "a");

    if (file == NULL) {
        centerText("Unable to open admin file!");
        pressEnter();
        return;
    }

    fprintf(file, "%s|%s|%lu\n", username, name, passwordHash);
    fclose(file);

    centerText("ADMIN REGISTRATION SUCCESSFUL!");
    centerText("You can now login with your account.");
    pressEnter();
}

/* ADMIN LOGIN */

int loginAdmin(void) {
    FILE *file;
    char username[50];
    char password[100];
    char savedUsername[50];
    char savedName[100];
    unsigned long savedHash;
    unsigned long enteredHash;

    heading("ADMIN LOGIN");

    getString("Enter Username:", username, sizeof(username));
    getString("Enter Password:", password, sizeof(password));

    enteredHash = hashPassword(password);

    file = fopen(ADMIN_FILE, "r");
    if (file == NULL) {
        centerText("No admin account found. Please register first.");
        pressEnter();
        return 0;
    }

    while (fscanf(file, "%49[^|]|%99[^|]|%lu\n",
                  savedUsername, savedName, &savedHash) == 3) {
        if (strcmp(savedUsername, username) == 0 &&
            savedHash == enteredHash) {
            fclose(file);
            snprintf(currentAdmin, sizeof(currentAdmin), "%.60s (%.30s)", savedName, savedUsername);

            centerText("LOGIN SUCCESSFUL!");
            printf("\n");
            centerText("Welcome to the Library Management System.");
            pressEnter();
            return 1;
        }
    }

    fclose(file);
    centerText("Invalid username or password!");
    pressEnter();
    return 0;
}

/* LOGIN / REGISTRATION START PAGE */

int authenticationMenu(void) {
    int choice;

    while (1) {
        heading("ADMIN AUTHENTICATION");

        centerText("1. Admin Login");
        centerText("2. Admin Registration");
        centerText("3. Exit Program");

        choice = getInt("Enter Your Choice:");

        switch (choice) {
            case 1:
                if (loginAdmin())
                    return 1;
                break;

            case 2:
                registerAdmin();
                break;

            case 3:
                return 0;

            default:
                centerText("Invalid Choice!");
                pressEnter();
        }
    }
}

/* =====================================================
   31. BOOK MANAGEMENT MENU
   ===================================================== */

void bookMenu(void) {

    int choice;

    while (1) {

        heading("BOOK MANAGEMENT");

        centerText("1. Add New Book");

        centerText("2. Update Book");

        centerText("3. Delete Book");

        centerText("4. View All Books");

        centerText("5. Back to Main Menu");

        choice = getInt("Enter Your Choice:");

        switch (choice) {

            case 1:
                addBook();
                break;

            case 2:
                updateBook();
                break;

            case 3:
                removeBook();
                break;

            case 4:
                viewBooks();
                break;

            case 5:
                return;

            default:
                centerText("Invalid Choice!");
                pressEnter();
        }
    }
}

/* =====================================================
   32. BOOK SEARCH MENU
   ===================================================== */

void searchMenu(void) {

    int choice;

    while (1) {

        heading("BOOK SEARCHING");

        centerText("1. Search by Book ID");

        centerText("2. Search by Book Title");

        centerText("3. Back to Main Menu");

        choice = getInt("Enter Your Choice:");

        switch (choice) {

            case 1:
                searchByID();
                break;

            case 2:
                searchByTitle();
                break;

            case 3:
                return;

            default:
                centerText("Invalid Choice!");
                pressEnter();
        }
    }
}

/* =====================================================
   33. MEMBER MANAGEMENT MENU
   ===================================================== */

void memberMenu(void) {

    int choice;

    while (1) {

        heading("MEMBER MANAGEMENT");

        centerText("1. Add New Member");

        centerText("2. Update Member");

        centerText("3. Delete Member");

        centerText("4. Search Member");

        centerText("5. View All Members");

        centerText("6. Back to Main Menu");

        choice = getInt("Enter Your Choice:");

        switch (choice) {

            case 1:
                addMember();
                break;

            case 2:
                updateMember();
                break;

            case 3:
                deleteMember();
                break;

            case 4:
                searchMemberInfo();
                break;

            case 5:
                viewMembers();
                break;

            case 6:
                return;

            default:
                centerText("Invalid Choice!");
                pressEnter();
        }
    }
}

/* =====================================================
   34. MAIN MENU
   ===================================================== */

void mainMenu(void) {

    int choice;

    while (1) {

        heading("MAIN MENU");

        {
            char welcome[150];
            snprintf(welcome, sizeof(welcome), "Logged in Admin: %s", currentAdmin);
            centerText(welcome);
        }

        centerText("1. Book Management");
        centerText("2. Search Books");
        centerText("3. Book Availability");
        centerText("4. Issue Book");
        centerText("5. Return Book");
        centerText("6. Member Management");
        centerText("7. Save Records");
        centerText("8. Logout");
        centerText("9. Exit Program");

        printf("\n");

        choice = getInt("Enter Your Choice:");

        switch (choice) {

            case 1:
                bookMenu();
                break;

            case 2:
                searchMenu();
                break;

            case 3:
                bookStatistics();
                break;

            case 4:
                issueBook();
                break;

            case 5:
                returnBook();
                break;

            case 6:
                memberMenu();
                break;

            case 7:
                heading("SAVE RECORDS");
                saveAllData();
                centerText("ALL RECORDS SAVED SUCCESSFULLY!");
                pressEnter();
                break;

            case 8:
                heading("ADMIN LOGOUT");
                centerText("You have been logged out successfully.");
                pressEnter();
                currentAdmin[0] = '\0';
                return;

            case 9:
                heading("EXIT SYSTEM");
                saveAllData();
                centerText("ALL RECORDS SAVED SUCCESSFULLY!");
                centerText("THANK YOU FOR USING OUR LIBRARY!");
                printf("\n\n");
                exit(0);

            default:
                centerText("Invalid Choice!");
                pressEnter();
        }
    }
}

/* =====================================================
   36. MAIN FUNCTION
   ===================================================== */

int main(void) {

    /* LOAD PREVIOUS BOOK AND MEMBER RECORDS */

    loadBookFile();
    loadMemberFile();

    /* ADMIN MUST LOGIN BEFORE USING THE SYSTEM */

    while (authenticationMenu()) {
        mainMenu();
    }

    heading("PROGRAM CLOSED");
    centerText("Thank you for using Library Management System!");

    return 0;
}

/* =====================================================
   END OF LIBRARY MANAGEMENT SYSTEM
   ===================================================== */
