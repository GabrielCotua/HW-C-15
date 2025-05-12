// Gabriel Cotua HW #15

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#define DEBUG
#define FLAG_LEAVE   (1 << 0)   // bit 0 = “user wants to quit”
#define FLAG_LOADED_FILE  (1 << 1)   // bit 1 = “books have been loaded”
#define STRSIZE 100
#define FILENAME "HW14Data.txt"
#define BINARY_FILENAME "Books.bin"
#define FLUSH while(getchar() != '\n');

typedef struct Book {
  char Title[STRSIZE];
  char Author[STRSIZE];
  int Year;
  int Location;
} BOOK;

typedef struct Node {
  BOOK Item;
  struct Node * Next;
} NODE;

int AppendNode(NODE ** head, BOOK value);
void FreeList(NODE * head, int * TotalLength);
void DisplayList(NODE * head);
void DisplayBook(NODE * head, int TotalLength);
int WriteBinary(NODE * head, char * filename);
int GettingFileInfo(NODE ** node, char * filename, int * TotalLength);

int main(void) {

  printf("\n\n|#####################|\n|HW #15, Gabriel Cotua|\n|#####################|\n\n");

  NODE * head = NULL;                                   // Node head of Linked LIst
  char userfile[STRSIZE] = FILENAME;                    // File requested by the user
  char bin_userfile[STRSIZE] = BINARY_FILENAME;         // Binary file used
  char ans;                                             // used to go through the if-else statements
  //int leave = 0;                                      // flag to leave the loop
  int TotalValue = 0;                                   // number of books in the list
  char flags = 0;                                       /*
                                                        from right to left <- each value one bit
                                                        [BooksLoaded][leave]
                                                        */

  // MENU
  while (!(flags & FLAG_LEAVE)) {
    if (!(flags & FLAG_LOADED_FILE)) {
      puts( "_________START MENU________\n"
                "[i] - Get Books Information\n"
                "[q] - Quit\n");
      flags |= FLAG_LOADED_FILE;
    } else {
      puts( "____________MENU____________\n"
                "[i] - Get Book Information From Another File\n"
                "[a] - Add Book\n"
                "[w] - Write Data to Binary\n"
                "[d] - Display Book List\n"
                "[b] - Book Information\n"
                "[q] - Quit\n");
    }
    ans = getchar();
    FLUSH;

    switch (tolower(ans)) {

      case 'i':
        // load from text file.
        printf("Would you like to gather book information from '%s'? (y/n) ", userfile);
        ans = getchar();
        FLUSH;

        if (tolower(ans) == 'y') {
          if (head != NULL) {
            FreeList(head, &TotalValue);
            head = NULL;
          }
          GettingFileInfo(&head, userfile, &TotalValue);
          DisplayList(head);
          break;
        }
        if (tolower(ans) == 'n') {
          printf("Insert the new file name (max %d chars): ", STRSIZE-1);
          fgets(userfile, STRSIZE, stdin);
          userfile[strcspn(userfile, "\n")] = '\0';

          if (head != NULL) {
            FreeList(head, &TotalValue);
            head = NULL;
          }

          printf("Gathering information from %s.\n", userfile);
          GettingFileInfo(&head, userfile, &TotalValue);
          DisplayList(head);
          flags |= FLAG_LOADED_FILE; // Marks the flag as loaded "xxxx xx1x"
          break;
        }
        break;

      case 'a':
        BOOK book;
        puts("Enter book title: ");
        fgets(book.Title, STRSIZE, stdin);
        book.Title[strcspn(book.Title, "\n")] = '\0';

        puts("Enter book author: ");
        fgets(book.Author, STRSIZE, stdin);
        book.Author[strcspn(book.Author, "\n")] = '\0';

        puts("Enter book year: ");
        scanf("%d", &book.Year);
        FLUSH;

        book.Location = ++TotalValue;

        if (AppendNode(&head, book) == 0) {
          puts("Book added successfully!\n");
        } else {
          puts("Failed to add the book.\n");
        }
        break;

      case 'w':
        printf("Would you like to write information to %s? (y/n)", bin_userfile);
        ans = getchar();
        FLUSH;
        if (tolower(ans) == 'y') {
          WriteBinary(head, bin_userfile);
          break;
        }

        if (tolower(ans) == 'n') {
          printf("Insert the new file name (max %d chars): ", STRSIZE-1);
          fgets(bin_userfile, STRSIZE, stdin);
          bin_userfile[strcspn(bin_userfile, "\n")] = '\0';

          printf("Writing information to %s.\n", bin_userfile);
          WriteBinary(head, bin_userfile);
          break;
        }
        break;

      case 'd':
        if (head == NULL) {
          puts("There is no books in the list.\n");
          break;
        }
        puts("Books in the list: \n");
        DisplayList(head);
        break;

      case 'b':
        DisplayBook(head, TotalValue);
        break;


      case 'q':
        flags |= FLAG_LEAVE;
        break;

      default:
        puts("Please enter a valid option: \n");
        break;
    }
  }
  FreeList(head, &TotalValue);
  return 0;
}

int AppendNode(NODE **head, BOOK value) {
  NODE *temp = malloc(sizeof(NODE));
  if (!temp) {
    printf("Memory allocation failed!\n");
    return -1;
  }

  temp->Next = NULL;
  temp->Item = value;

  if (*head == NULL) {
    *head = temp;
  } else {
    NODE *current = *head;
    while (current->Next != NULL) {
      current = current->Next;
    }
    current->Next = temp;
  }

  return 0;
}


void FreeList(NODE * head, int * TotalLength) {
  NODE * temp;
  while (head != NULL) {
    temp = head;
    head = head->Next;
    free(temp);
  }
  head = NULL;
  *TotalLength = 0;
}

int GettingFileInfo(NODE **node, char *filename, int * TotalLength) {
  char title[STRSIZE];
  char author[STRSIZE];
  char yearStr[10];
  int  year;
  int count = 0;

  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error opening the file. File does not exist.\n");
    return -1;
  }

  while (fgets(title, STRSIZE, fp)) {
    if (!fgets(author, STRSIZE, fp) || !fgets(yearStr, 10, fp)) {
      printf("File format error: incomplete book entry.\n");
      fclose(fp);
      return -2;
    }

    title[strcspn(title, "\n")] = '\0';
    author[strcspn(author, "\n")] = '\0';
    yearStr[strcspn(yearStr, "\n")] = '\0';

    year = atoi(yearStr);
    count++;

    BOOK tempBook;
    strcpy(tempBook.Title, title);
    strcpy(tempBook.Author, author);
    tempBook.Year = year;
    tempBook.Location = count;

    AppendNode(node, tempBook);
  }
  *TotalLength = count;

  fclose(fp);
  return 0;
}


void DisplayList(NODE * head) {
  NODE * tempNode = head; 
   // printf("Opening DisplayListLOOP\n");
  while (tempNode != NULL) {
    printf("[%d] - BookTitle: %s by [%s] (%d)\n", tempNode->Item.Location, tempNode->Item.Title, tempNode->Item.Author, tempNode->Item.Year);
    tempNode = tempNode->Next; 
  }
}
void DisplayBook(NODE * head, int TotalLength) {

  char ans = 'y';
  int location;
  while (tolower(ans) != 'n') {
    NODE * tempNode = head;
    printf("Insert book location for information (1 - %d): ", TotalLength);
    scanf( "%d", &location);
    FLUSH;
    if (!isdigit(location) && (location <= 0 || location > TotalLength) && ans != 'n') {
      while (!isdigit(location) && (location <= 0 || location > TotalLength) && ans != 'n') {
        printf("Invalid option, please insert a valid one ( 1 - %d | q - quit ): ", TotalLength);
        ans = getchar();
        FLUSH;
        if (ans == 'q') {
          ans = 'n';
        }
      }


    } else {

      while (location != tempNode->Item.Location) {
        tempNode = tempNode->Next;
      }

      printf("[%d] - BookTitle: %s by [%s] (%d)\n\n", tempNode->Item.Location, tempNode->Item.Title, tempNode->Item.Author, tempNode->Item.Year);
      puts("Would you like to continue? (y/n)\n");
      ans = getchar();
      FLUSH;
      while (tolower(ans) != 'y' && tolower(ans) != 'n') {
        puts("Invalid choice, Insert a valid choice\nWould you like to look for another book? (y/n).\n");
        ans = getchar();
        FLUSH;
      }

    }
  }
}

int WriteBinary(NODE * head, char * filename) {
  if (head == NULL) {
    printf("No books to write.\n");
    return 0;
  }

  char ans = 'n';
  FILE * pfile = fopen(filename, "r");

  while (tolower(ans) != 'y') {
    if (pfile == NULL) {
      printf("Error opening the file. File does not exist.\n");
      printf("Would you like to create it? (y/n): ");
      ans = getchar(); FLUSH;

      if (tolower(ans) == 'y') {
        pfile = fopen(filename, "wb");
        if (pfile == NULL) {
          printf("Error creating the file.\n");
          return -1;
        }
      } else {
        printf("File not created. Returning to main menu.\n");
        return 0;
      }
    } else {
      fclose(pfile);
      printf("File already exists. Overwrite %s? (y/n): ", filename);
      ans = getchar(); FLUSH;

      if (tolower(ans) == 'n') {
        printf("Back to Main Menu...\n");
        return 0;
      } else if (tolower(ans) == 'y') {
        pfile = fopen(filename, "wb");
        if (pfile == NULL) {
          printf("Error opening file for writing.\n");
          return -1;
        }
      } else {
        printf("Please enter a valid choice.\n");
        ans = 'n'; // Re-prompt
      }
    }
  }

  int count = 0;
  for (NODE *cur = head; cur; cur = cur->Next) {
    #ifdef DEBUG
    printf("Writing book: %s by %s (%d)\n", cur->Item.Title, cur->Item.Author, cur->Item.Year);
    #endif
    if (fwrite(&cur->Item, sizeof(BOOK), 1, pfile) != 1) {
      perror("Error writing BOOK to binary file");
      cur->Item.Location = count;
      fclose(pfile);
      return -2;
    }
    count++;
  }

  fclose(pfile);
  #ifdef DEBUG
  printf("Successfully wrote %d book(s) to %s\n", count, filename);
  #endif
  return 0;
}



/*

|#####################|
|HW #14, Gabriel Cotua|
|#####################|

____________MENU____________
[i] - Get Books Information
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[q] - Quit
i

Would you like to gather book information from 'HW14Data.txt'? (y/n)y

BookTitle: On Basilisk Station by [Weber, David] (1993)
BookTitle: The Sum of All Fears by [Clancy, Tom] (1992)
BookTitle: Battle Born by [Brown, Dale] (1999)
BookTitle: Between Planets by [Heinlein, Robert A.] (1953)
BookTitle: Stranger in a Strange Land by [Heinlein, Robert A.] (1959)
BookTitle: A Soldier's Duty by [Johnson, Jean] (2011)
BookTitle: Swords Against Wizardry by [Leiber, Fritz] (1968)
BookTitle: The Mote in God's Eye by [Niven, Larry] (1992)
BookTitle: Uncharted Stars by [Norton, Andre] (1968)
BookTitle: Raising Steam by [Pratchett, Terry] (2013)
BookTitle: Ender's Game by [Card, Orson Scott] (1977)
BookTitle: Foundation and Empire by [Asimov, Issac] (1951)
BookTitle: I, Robot by [Asimov, Issac] (1950)
BookTitle: The Hitchiker's guide to the Universe by [Adams, Douglas] (1979)
BookTitle: C Primer Plus by [Prata, Stephen] (2014)
____________MENU____________
[i] - Get Books Information
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[q] - Quit
a

Enter book title:Stitches

Enter book author:Ito, Junji

Enter book year:2024

Book added successfully!
____________MENU____________
[i] - Get Books Information
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[q] - Quit
d

Books in the list:
BookTitle: On Basilisk Station by [Weber, David] (1993)
BookTitle: The Sum of All Fears by [Clancy, Tom] (1992)
BookTitle: Battle Born by [Brown, Dale] (1999)
BookTitle: Between Planets by [Heinlein, Robert A.] (1953)
BookTitle: Stranger in a Strange Land by [Heinlein, Robert A.] (1959)
BookTitle: A Soldier's Duty by [Johnson, Jean] (2011)
BookTitle: Swords Against Wizardry by [Leiber, Fritz] (1968)
BookTitle: The Mote in God's Eye by [Niven, Larry] (1992)
BookTitle: Uncharted Stars by [Norton, Andre] (1968)
BookTitle: Raising Steam by [Pratchett, Terry] (2013)
BookTitle: Ender's Game by [Card, Orson Scott] (1977)
BookTitle: Foundation and Empire by [Asimov, Issac] (1951)
BookTitle: I, Robot by [Asimov, Issac] (1950)
BookTitle: The Hitchiker's guide to the Universe by [Adams, Douglas] (1979)
BookTitle: C Primer Plus by [Prata, Stephen] (2014)
BookTitle: Stitches by [Ito, Junji] (2024)
____________MENU____________
[i] - Get Books Information
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[q] - Quit
w

Would you like to write information to Books.bin? (y/n)y

File already exists. Overwrite Books.bin? (y/n):y

Writing book: On Basilisk Station by Weber, David (1993)
Writing book: The Sum of All Fears by Clancy, Tom (1992)
Writing book: Battle Born by Brown, Dale (1999)
Writing book: Between Planets by Heinlein, Robert A. (1953)
Writing book: Stranger in a Strange Land by Heinlein, Robert A. (1959)
Writing book: A Soldier's Duty by Johnson, Jean (2011)
Writing book: Swords Against Wizardry by Leiber, Fritz (1968)
Writing book: The Mote in God's Eye by Niven, Larry (1992)
Writing book: Uncharted Stars by Norton, Andre (1968)
Writing book: Raising Steam by Pratchett, Terry (2013)
Writing book: Ender's Game by Card, Orson Scott (1977)
Writing book: Foundation and Empire by Asimov, Issac (1951)
Writing book: I, Robot by Asimov, Issac (1950)
Writing book: The Hitchiker's guide to the Universe by Adams, Douglas (1979)
Writing book: C Primer Plus by Prata, Stephen (2014)
Writing book: Stitches by Ito, Junji (2024)
Successfully wrote 16 book(s) to Books.bin
____________MENU____________
[i] - Get Books Information
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[q] - Quit
q


Process finished with exit code 0

 */