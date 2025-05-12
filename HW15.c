// Gabriel Cotua HW #15

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#define DEBUG
#define FLAG_LEAVE   (1 << 0)   // bit 0 = “user wants to quit”
#define FLAG_LOADED_FILE  (1 << 1)   // bit 1 = “books have been loaded”
#define STRSIZE 100
#define FILENAME "HW15Data.txt"
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

  BOOK book;
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
          #ifdef DEBUG
          DisplayList(head);
          #endif
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
  printf("Done Successfully!\n\n");
  fclose(fp);
  return 0;
}


void DisplayList(NODE * head) {
  NODE * tempNode = head; 
   // printf("Opening DisplayListLOOP\n");
  while (tempNode != NULL) {
    printf("[%d] - Book Title: \"%s\"\n", tempNode->Item.Location, tempNode->Item.Title);
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
[Output Example]
[test file was modified to have more entries]

|#####################|
|HW #15, Gabriel Cotua|
|#####################|

_________START MENU________
[i] - Get Books Information
[q] - Quit

i
Would you like to gather book information from 'HW15Data.txt'? (y/n) y
Done Successfully!

____________MENU____________
[i] - Get Book Information From Another File
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[b] - Book Information
[q] - Quit

d
Books in the list: 

[1] - Book Title: "On Basilisk Station"
[2] - Book Title: "The Sum of All Fears"
[3] - Book Title: "Battle Born"
[4] - Book Title: "Between Planets"
[5] - Book Title: "Stranger in a Strange Land"
[6] - Book Title: "A Soldier's Duty"
[7] - Book Title: "Swords Against Wizardry"
[8] - Book Title: "The Mote in God's Eye"
[9] - Book Title: "Uncharted Stars"
[10] - Book Title: "Raising Steam"
[11] - Book Title: "Ender's Game"
[12] - Book Title: "Foundation and Empire"
[13] - Book Title: "I, Robot"
[14] - Book Title: "The Hitchiker's guide to the Universe"
[15] - Book Title: "C Primer Plus"
[16] - Book Title: "On Basilisk Station"
[17] - Book Title: "The Sum of All Fears"
[18] - Book Title: "Battle Born"
[19] - Book Title: "Between Planets"
[20] - Book Title: "Stranger in a Strange Land"
[21] - Book Title: "A Soldier's Duty"
[22] - Book Title: "Swords Against Wizardry"
[23] - Book Title: "The Mote in God's Eye"
[24] - Book Title: "Uncharted Stars"
[25] - Book Title: "Raising Steam"
[26] - Book Title: "Ender's Game"
[27] - Book Title: "Foundation and Empire"
[28] - Book Title: "I, Robot"
[29] - Book Title: "The Hitchiker's guide to the Universe"
[30] - Book Title: "C Primer Plus"
[31] - Book Title: "On Basilisk Station"
[32] - Book Title: "The Sum of All Fears"
[33] - Book Title: "Battle Born"
[34] - Book Title: "Between Planets"
[35] - Book Title: "Stranger in a Strange Land"
[36] - Book Title: "A Soldier's Duty"
[37] - Book Title: "Swords Against Wizardry"
[38] - Book Title: "The Mote in God's Eye"
[39] - Book Title: "Uncharted Stars"
[40] - Book Title: "Raising Steam"
[41] - Book Title: "Ender's Game"
[42] - Book Title: "Foundation and Empire"
[43] - Book Title: "I, Robot"
[44] - Book Title: "The Hitchiker's guide to the Universe"
[45] - Book Title: "C Primer Plus"
[46] - Book Title: "On Basilisk Station"
[47] - Book Title: "The Sum of All Fears"
[48] - Book Title: "Battle Born"
[49] - Book Title: "Between Planets"
[50] - Book Title: "Stranger in a Strange Land"
[51] - Book Title: "A Soldier's Duty"
[52] - Book Title: "Swords Against Wizardry"
[53] - Book Title: "The Mote in God's Eye"
[54] - Book Title: "Uncharted Stars"
[55] - Book Title: "Raising Steam"
[56] - Book Title: "Ender's Game"
[57] - Book Title: "Foundation and Empire"
[58] - Book Title: "I, Robot"
[59] - Book Title: "The Hitchiker's guide to the Universe"
[60] - Book Title: "C Primer Plus"
[61] - Book Title: "On Basilisk Station"
[62] - Book Title: "The Sum of All Fears"
[63] - Book Title: "Battle Born"
[64] - Book Title: "Between Planets"
[65] - Book Title: "Stranger in a Strange Land"
[66] - Book Title: "A Soldier's Duty"
[67] - Book Title: "Swords Against Wizardry"
[68] - Book Title: "The Mote in God's Eye"
[69] - Book Title: "Uncharted Stars"
[70] - Book Title: "Raising Steam"
[71] - Book Title: "Ender's Game"
[72] - Book Title: "Foundation and Empire"
[73] - Book Title: "I, Robot"
[74] - Book Title: "The Hitchiker's guide to the Universe"
[75] - Book Title: "C Primer Plus"
[76] - Book Title: "On Basilisk Station"
[77] - Book Title: "The Sum of All Fears"
[78] - Book Title: "Battle Born"
[79] - Book Title: "Between Planets"
[80] - Book Title: "Stranger in a Strange Land"
[81] - Book Title: "A Soldier's Duty"
[82] - Book Title: "Swords Against Wizardry"
[83] - Book Title: "The Mote in God's Eye"
[84] - Book Title: "Uncharted Stars"
[85] - Book Title: "Raising Steam"
[86] - Book Title: "Ender's Game"
[87] - Book Title: "Foundation and Empire"
[88] - Book Title: "I, Robot"
[89] - Book Title: "The Hitchiker's guide to the Universe"
[90] - Book Title: "C Primer Plus"
[91] - Book Title: "On Basilisk Station"
[92] - Book Title: "The Sum of All Fears"
[93] - Book Title: "Battle Born"
[94] - Book Title: "Between Planets"
[95] - Book Title: "Stranger in a Strange Land"
[96] - Book Title: "A Soldier's Duty"
[97] - Book Title: "Swords Against Wizardry"
[98] - Book Title: "The Mote in God's Eye"
[99] - Book Title: "Uncharted Stars"
[100] - Book Title: "Raising Steam"
[101] - Book Title: "Ender's Game"
[102] - Book Title: "Foundation and Empire"
[103] - Book Title: "I, Robot"
[104] - Book Title: "The Hitchiker's guide to the Universe"
[105] - Book Title: "C Primer Plus"
[106] - Book Title: "On Basilisk Station"
[107] - Book Title: "The Sum of All Fears"
[108] - Book Title: "Battle Born"
[109] - Book Title: "Between Planets"
[110] - Book Title: "Stranger in a Strange Land"
[111] - Book Title: "A Soldier's Duty"
[112] - Book Title: "Swords Against Wizardry"
[113] - Book Title: "The Mote in God's Eye"
[114] - Book Title: "Uncharted Stars"
[115] - Book Title: "Raising Steam"
[116] - Book Title: "Ender's Game"
[117] - Book Title: "Foundation and Empire"
[118] - Book Title: "I, Robot"
[119] - Book Title: "The Hitchiker's guide to the Universe"
[120] - Book Title: "C Primer Plus"
[121] - Book Title: "On Basilisk Station"
[122] - Book Title: "The Sum of All Fears"
[123] - Book Title: "Battle Born"
[124] - Book Title: "Between Planets"
[125] - Book Title: "Stranger in a Strange Land"
[126] - Book Title: "A Soldier's Duty"
[127] - Book Title: "Swords Against Wizardry"
[128] - Book Title: "The Mote in God's Eye"
[129] - Book Title: "Uncharted Stars"
[130] - Book Title: "Raising Steam"
[131] - Book Title: "Ender's Game"
[132] - Book Title: "Foundation and Empire"
[133] - Book Title: "I, Robot"
[134] - Book Title: "The Hitchiker's guide to the Universe"
[135] - Book Title: "C Primer Plus"
[136] - Book Title: "On Basilisk Station"
[137] - Book Title: "The Sum of All Fears"
[138] - Book Title: "Battle Born"
[139] - Book Title: "Between Planets"
[140] - Book Title: "Stranger in a Strange Land"
[141] - Book Title: "A Soldier's Duty"
[142] - Book Title: "Swords Against Wizardry"
[143] - Book Title: "The Mote in God's Eye"
[144] - Book Title: "Uncharted Stars"
[145] - Book Title: "Raising Steam"
[146] - Book Title: "Ender's Game"
[147] - Book Title: "Foundation and Empire"
[148] - Book Title: "I, Robot"
[149] - Book Title: "The Hitchiker's guide to the Universe"
[150] - Book Title: "C Primer Plus"
[151] - Book Title: "On Basilisk Station"
[152] - Book Title: "The Sum of All Fears"
[153] - Book Title: "Battle Born"
[154] - Book Title: "Between Planets"
[155] - Book Title: "Stranger in a Strange Land"
[156] - Book Title: "A Soldier's Duty"
[157] - Book Title: "Swords Against Wizardry"
[158] - Book Title: "The Mote in God's Eye"
[159] - Book Title: "Uncharted Stars"
[160] - Book Title: "Raising Steam"
[161] - Book Title: "Ender's Game"
[162] - Book Title: "Foundation and Empire"
[163] - Book Title: "I, Robot"
[164] - Book Title: "The Hitchiker's guide to the Universe"
[165] - Book Title: "C Primer Plus"
[166] - Book Title: "On Basilisk Station"
[167] - Book Title: "The Sum of All Fears"
[168] - Book Title: "Battle Born"
[169] - Book Title: "Between Planets"
[170] - Book Title: "Stranger in a Strange Land"
[171] - Book Title: "A Soldier's Duty"
[172] - Book Title: "Swords Against Wizardry"
[173] - Book Title: "The Mote in God's Eye"
[174] - Book Title: "Uncharted Stars"
[175] - Book Title: "Raising Steam"
[176] - Book Title: "Ender's Game"
[177] - Book Title: "Foundation and Empire"
[178] - Book Title: "I, Robot"
[179] - Book Title: "The Hitchiker's guide to the Universe"
[180] - Book Title: "C Primer Plus"
[181] - Book Title: "On Basilisk Station"
[182] - Book Title: "The Sum of All Fears"
[183] - Book Title: "Battle Born"
[184] - Book Title: "Between Planets"
[185] - Book Title: "Stranger in a Strange Land"
[186] - Book Title: "A Soldier's Duty"
[187] - Book Title: "Swords Against Wizardry"
[188] - Book Title: "The Mote in God's Eye"
[189] - Book Title: "Uncharted Stars"
[190] - Book Title: "Raising Steam"
[191] - Book Title: "Ender's Game"
[192] - Book Title: "Foundation and Empire"
[193] - Book Title: "I, Robot"
[194] - Book Title: "The Hitchiker's guide to the Universe"
[195] - Book Title: "C Primer Plus"
[196] - Book Title: "On Basilisk Station"
[197] - Book Title: "The Sum of All Fears"
[198] - Book Title: "Battle Born"
[199] - Book Title: "Between Planets"
[200] - Book Title: "Stranger in a Strange Land"
[201] - Book Title: "A Soldier's Duty"
[202] - Book Title: "Swords Against Wizardry"
[203] - Book Title: "The Mote in God's Eye"
[204] - Book Title: "Uncharted Stars"
[205] - Book Title: "Raising Steam"
[206] - Book Title: "Ender's Game"
[207] - Book Title: "Foundation and Empire"
[208] - Book Title: "I, Robot"
[209] - Book Title: "The Hitchiker's guide to the Universe"
[210] - Book Title: "C Primer Plus"
[211] - Book Title: "On Basilisk Station"
[212] - Book Title: "The Sum of All Fears"
[213] - Book Title: "Battle Born"
[214] - Book Title: "Between Planets"
[215] - Book Title: "Stranger in a Strange Land"
[216] - Book Title: "A Soldier's Duty"
[217] - Book Title: "Swords Against Wizardry"
[218] - Book Title: "The Mote in God's Eye"
[219] - Book Title: "Uncharted Stars"
[220] - Book Title: "Raising Steam"
[221] - Book Title: "Ender's Game"
[222] - Book Title: "Foundation and Empire"
[223] - Book Title: "I, Robot"
[224] - Book Title: "The Hitchiker's guide to the Universe"
[225] - Book Title: "C Primer Plus"
[226] - Book Title: "On Basilisk Station"
[227] - Book Title: "The Sum of All Fears"
[228] - Book Title: "Battle Born"
[229] - Book Title: "Between Planets"
[230] - Book Title: "Stranger in a Strange Land"
[231] - Book Title: "A Soldier's Duty"
[232] - Book Title: "Swords Against Wizardry"
[233] - Book Title: "The Mote in God's Eye"
[234] - Book Title: "Uncharted Stars"
[235] - Book Title: "Raising Steam"
[236] - Book Title: "Ender's Game"
[237] - Book Title: "Foundation and Empire"
[238] - Book Title: "I, Robot"
[239] - Book Title: "The Hitchiker's guide to the Universe"
[240] - Book Title: "C Primer Plus"
[241] - Book Title: "On Basilisk Station"
[242] - Book Title: "The Sum of All Fears"
[243] - Book Title: "Battle Born"
[244] - Book Title: "Between Planets"
[245] - Book Title: "Stranger in a Strange Land"
[246] - Book Title: "A Soldier's Duty"
[247] - Book Title: "Swords Against Wizardry"
[248] - Book Title: "The Mote in God's Eye"
[249] - Book Title: "Uncharted Stars"
[250] - Book Title: "Raising Steam"
[251] - Book Title: "Ender's Game"
[252] - Book Title: "Foundation and Empire"
[253] - Book Title: "I, Robot"
[254] - Book Title: "The Hitchiker's guide to the Universe"
[255] - Book Title: "C Primer Plus"
[256] - Book Title: "On Basilisk Station"
[257] - Book Title: "The Sum of All Fears"
[258] - Book Title: "Battle Born"
[259] - Book Title: "Between Planets"
[260] - Book Title: "Stranger in a Strange Land"
[261] - Book Title: "A Soldier's Duty"
[262] - Book Title: "Swords Against Wizardry"
[263] - Book Title: "The Mote in God's Eye"
[264] - Book Title: "Uncharted Stars"
[265] - Book Title: "Raising Steam"
[266] - Book Title: "Ender's Game"
[267] - Book Title: "Foundation and Empire"
[268] - Book Title: "I, Robot"
[269] - Book Title: "The Hitchiker's guide to the Universe"
[270] - Book Title: "C Primer Plus"
[271] - Book Title: "On Basilisk Station"
[272] - Book Title: "The Sum of All Fears"
[273] - Book Title: "Battle Born"
[274] - Book Title: "Between Planets"
[275] - Book Title: "Stranger in a Strange Land"
[276] - Book Title: "A Soldier's Duty"
[277] - Book Title: "Swords Against Wizardry"
[278] - Book Title: "The Mote in God's Eye"
[279] - Book Title: "Uncharted Stars"
[280] - Book Title: "Raising Steam"
[281] - Book Title: "Ender's Game"
[282] - Book Title: "Foundation and Empire"
[283] - Book Title: "I, Robot"
[284] - Book Title: "The Hitchiker's guide to the Universe"
[285] - Book Title: "C Primer Plus"
[286] - Book Title: "On Basilisk Station"
[287] - Book Title: "The Sum of All Fears"
[288] - Book Title: "Battle Born"
[289] - Book Title: "Between Planets"
[290] - Book Title: "Stranger in a Strange Land"
[291] - Book Title: "A Soldier's Duty"
[292] - Book Title: "Swords Against Wizardry"
[293] - Book Title: "The Mote in God's Eye"
[294] - Book Title: "Uncharted Stars"
[295] - Book Title: "Raising Steam"
[296] - Book Title: "Ender's Game"
[297] - Book Title: "Foundation and Empire"
[298] - Book Title: "I, Robot"
[299] - Book Title: "The Hitchiker's guide to the Universe"
[300] - Book Title: "C Primer Plus"
____________MENU____________
[i] - Get Book Information From Another File
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[b] - Book Information
[q] - Quit

b
Insert book location for information (1 - 300): 20
[20] - BookTitle: Stranger in a Strange Land by [Heinlein, Robert A.] (1959)

Would you like to continue? (y/n)

y
Insert book location for information (1 - 300): 200
[200] - BookTitle: Stranger in a Strange Land by [Heinlein, Robert A.] (1959)      

Would you like to continue? (y/n)

n
____________MENU____________
[i] - Get Book Information From Another File
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[b] - Book Information
[q] - Quit

a
Enter book title: 
Nice Book
Enter book author: 
Yo
Enter book year: 
2025
Book added successfully!

____________MENU____________
[i] - Get Book Information From Another File
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[b] - Book Information
[q] - Quit

d
Books in the list: 

[1] - Book Title: "On Basilisk Station"
[2] - Book Title: "The Sum of All Fears"
[3] - Book Title: "Battle Born"
[4] - Book Title: "Between Planets"
[5] - Book Title: "Stranger in a Strange Land"
[6] - Book Title: "A Soldier's Duty"
[7] - Book Title: "Swords Against Wizardry"
[8] - Book Title: "The Mote in God's Eye"
[9] - Book Title: "Uncharted Stars"
[10] - Book Title: "Raising Steam"
[11] - Book Title: "Ender's Game"
[12] - Book Title: "Foundation and Empire"
[13] - Book Title: "I, Robot"
[14] - Book Title: "The Hitchiker's guide to the Universe"
[15] - Book Title: "C Primer Plus"
[16] - Book Title: "On Basilisk Station"
[17] - Book Title: "The Sum of All Fears"
[18] - Book Title: "Battle Born"
[19] - Book Title: "Between Planets"
[20] - Book Title: "Stranger in a Strange Land"
[21] - Book Title: "A Soldier's Duty"
[22] - Book Title: "Swords Against Wizardry"
[23] - Book Title: "The Mote in God's Eye"
[24] - Book Title: "Uncharted Stars"
[25] - Book Title: "Raising Steam"
[26] - Book Title: "Ender's Game"
[27] - Book Title: "Foundation and Empire"
[28] - Book Title: "I, Robot"
[29] - Book Title: "The Hitchiker's guide to the Universe"
[30] - Book Title: "C Primer Plus"
[31] - Book Title: "On Basilisk Station"
[32] - Book Title: "The Sum of All Fears"
[33] - Book Title: "Battle Born"
[34] - Book Title: "Between Planets"
[35] - Book Title: "Stranger in a Strange Land"
[36] - Book Title: "A Soldier's Duty"
[37] - Book Title: "Swords Against Wizardry"
[38] - Book Title: "The Mote in God's Eye"
[39] - Book Title: "Uncharted Stars"
[40] - Book Title: "Raising Steam"
[41] - Book Title: "Ender's Game"
[42] - Book Title: "Foundation and Empire"
[43] - Book Title: "I, Robot"
[44] - Book Title: "The Hitchiker's guide to the Universe"
[45] - Book Title: "C Primer Plus"
[46] - Book Title: "On Basilisk Station"
[47] - Book Title: "The Sum of All Fears"
[48] - Book Title: "Battle Born"
[49] - Book Title: "Between Planets"
[50] - Book Title: "Stranger in a Strange Land"
[51] - Book Title: "A Soldier's Duty"
[52] - Book Title: "Swords Against Wizardry"
[53] - Book Title: "The Mote in God's Eye"
[54] - Book Title: "Uncharted Stars"
[55] - Book Title: "Raising Steam"
[56] - Book Title: "Ender's Game"
[57] - Book Title: "Foundation and Empire"
[58] - Book Title: "I, Robot"
[59] - Book Title: "The Hitchiker's guide to the Universe"
[60] - Book Title: "C Primer Plus"
[61] - Book Title: "On Basilisk Station"
[62] - Book Title: "The Sum of All Fears"
[63] - Book Title: "Battle Born"
[64] - Book Title: "Between Planets"
[65] - Book Title: "Stranger in a Strange Land"
[66] - Book Title: "A Soldier's Duty"
[67] - Book Title: "Swords Against Wizardry"
[68] - Book Title: "The Mote in God's Eye"
[69] - Book Title: "Uncharted Stars"
[70] - Book Title: "Raising Steam"
[71] - Book Title: "Ender's Game"
[72] - Book Title: "Foundation and Empire"
[73] - Book Title: "I, Robot"
[74] - Book Title: "The Hitchiker's guide to the Universe"
[75] - Book Title: "C Primer Plus"
[76] - Book Title: "On Basilisk Station"
[77] - Book Title: "The Sum of All Fears"
[78] - Book Title: "Battle Born"
[79] - Book Title: "Between Planets"
[80] - Book Title: "Stranger in a Strange Land"
[81] - Book Title: "A Soldier's Duty"
[82] - Book Title: "Swords Against Wizardry"
[83] - Book Title: "The Mote in God's Eye"
[84] - Book Title: "Uncharted Stars"
[85] - Book Title: "Raising Steam"
[86] - Book Title: "Ender's Game"
[87] - Book Title: "Foundation and Empire"
[88] - Book Title: "I, Robot"
[89] - Book Title: "The Hitchiker's guide to the Universe"
[90] - Book Title: "C Primer Plus"
[91] - Book Title: "On Basilisk Station"
[92] - Book Title: "The Sum of All Fears"
[93] - Book Title: "Battle Born"
[94] - Book Title: "Between Planets"
[95] - Book Title: "Stranger in a Strange Land"
[96] - Book Title: "A Soldier's Duty"
[97] - Book Title: "Swords Against Wizardry"
[98] - Book Title: "The Mote in God's Eye"
[99] - Book Title: "Uncharted Stars"
[100] - Book Title: "Raising Steam"
[101] - Book Title: "Ender's Game"
[102] - Book Title: "Foundation and Empire"
[103] - Book Title: "I, Robot"
[104] - Book Title: "The Hitchiker's guide to the Universe"
[105] - Book Title: "C Primer Plus"
[106] - Book Title: "On Basilisk Station"
[107] - Book Title: "The Sum of All Fears"
[108] - Book Title: "Battle Born"
[109] - Book Title: "Between Planets"
[110] - Book Title: "Stranger in a Strange Land"
[111] - Book Title: "A Soldier's Duty"
[112] - Book Title: "Swords Against Wizardry"
[113] - Book Title: "The Mote in God's Eye"
[114] - Book Title: "Uncharted Stars"
[115] - Book Title: "Raising Steam"
[116] - Book Title: "Ender's Game"
[117] - Book Title: "Foundation and Empire"
[118] - Book Title: "I, Robot"
[119] - Book Title: "The Hitchiker's guide to the Universe"
[120] - Book Title: "C Primer Plus"
[121] - Book Title: "On Basilisk Station"
[122] - Book Title: "The Sum of All Fears"
[123] - Book Title: "Battle Born"
[124] - Book Title: "Between Planets"
[125] - Book Title: "Stranger in a Strange Land"
[126] - Book Title: "A Soldier's Duty"
[127] - Book Title: "Swords Against Wizardry"
[128] - Book Title: "The Mote in God's Eye"
[129] - Book Title: "Uncharted Stars"
[130] - Book Title: "Raising Steam"
[131] - Book Title: "Ender's Game"
[132] - Book Title: "Foundation and Empire"
[133] - Book Title: "I, Robot"
[134] - Book Title: "The Hitchiker's guide to the Universe"
[135] - Book Title: "C Primer Plus"
[136] - Book Title: "On Basilisk Station"
[137] - Book Title: "The Sum of All Fears"
[138] - Book Title: "Battle Born"
[139] - Book Title: "Between Planets"
[140] - Book Title: "Stranger in a Strange Land"
[141] - Book Title: "A Soldier's Duty"
[142] - Book Title: "Swords Against Wizardry"
[143] - Book Title: "The Mote in God's Eye"
[144] - Book Title: "Uncharted Stars"
[145] - Book Title: "Raising Steam"
[146] - Book Title: "Ender's Game"
[147] - Book Title: "Foundation and Empire"
[148] - Book Title: "I, Robot"
[149] - Book Title: "The Hitchiker's guide to the Universe"
[150] - Book Title: "C Primer Plus"
[151] - Book Title: "On Basilisk Station"
[152] - Book Title: "The Sum of All Fears"
[153] - Book Title: "Battle Born"
[154] - Book Title: "Between Planets"
[155] - Book Title: "Stranger in a Strange Land"
[156] - Book Title: "A Soldier's Duty"
[157] - Book Title: "Swords Against Wizardry"
[158] - Book Title: "The Mote in God's Eye"
[159] - Book Title: "Uncharted Stars"
[160] - Book Title: "Raising Steam"
[161] - Book Title: "Ender's Game"
[162] - Book Title: "Foundation and Empire"
[163] - Book Title: "I, Robot"
[164] - Book Title: "The Hitchiker's guide to the Universe"
[165] - Book Title: "C Primer Plus"
[166] - Book Title: "On Basilisk Station"
[167] - Book Title: "The Sum of All Fears"
[168] - Book Title: "Battle Born"
[169] - Book Title: "Between Planets"
[170] - Book Title: "Stranger in a Strange Land"
[171] - Book Title: "A Soldier's Duty"
[172] - Book Title: "Swords Against Wizardry"
[173] - Book Title: "The Mote in God's Eye"
[174] - Book Title: "Uncharted Stars"
[175] - Book Title: "Raising Steam"
[176] - Book Title: "Ender's Game"
[177] - Book Title: "Foundation and Empire"
[178] - Book Title: "I, Robot"
[179] - Book Title: "The Hitchiker's guide to the Universe"
[180] - Book Title: "C Primer Plus"
[181] - Book Title: "On Basilisk Station"
[182] - Book Title: "The Sum of All Fears"
[183] - Book Title: "Battle Born"
[184] - Book Title: "Between Planets"
[185] - Book Title: "Stranger in a Strange Land"
[186] - Book Title: "A Soldier's Duty"
[187] - Book Title: "Swords Against Wizardry"
[188] - Book Title: "The Mote in God's Eye"
[189] - Book Title: "Uncharted Stars"
[190] - Book Title: "Raising Steam"
[191] - Book Title: "Ender's Game"
[192] - Book Title: "Foundation and Empire"
[193] - Book Title: "I, Robot"
[194] - Book Title: "The Hitchiker's guide to the Universe"
[195] - Book Title: "C Primer Plus"
[196] - Book Title: "On Basilisk Station"
[197] - Book Title: "The Sum of All Fears"
[198] - Book Title: "Battle Born"
[199] - Book Title: "Between Planets"
[200] - Book Title: "Stranger in a Strange Land"
[201] - Book Title: "A Soldier's Duty"
[202] - Book Title: "Swords Against Wizardry"
[203] - Book Title: "The Mote in God's Eye"
[204] - Book Title: "Uncharted Stars"
[205] - Book Title: "Raising Steam"
[206] - Book Title: "Ender's Game"
[207] - Book Title: "Foundation and Empire"
[208] - Book Title: "I, Robot"
[209] - Book Title: "The Hitchiker's guide to the Universe"
[210] - Book Title: "C Primer Plus"
[211] - Book Title: "On Basilisk Station"
[212] - Book Title: "The Sum of All Fears"
[213] - Book Title: "Battle Born"
[214] - Book Title: "Between Planets"
[215] - Book Title: "Stranger in a Strange Land"
[216] - Book Title: "A Soldier's Duty"
[217] - Book Title: "Swords Against Wizardry"
[218] - Book Title: "The Mote in God's Eye"
[219] - Book Title: "Uncharted Stars"
[220] - Book Title: "Raising Steam"
[221] - Book Title: "Ender's Game"
[222] - Book Title: "Foundation and Empire"
[223] - Book Title: "I, Robot"
[224] - Book Title: "The Hitchiker's guide to the Universe"
[225] - Book Title: "C Primer Plus"
[226] - Book Title: "On Basilisk Station"
[227] - Book Title: "The Sum of All Fears"
[228] - Book Title: "Battle Born"
[229] - Book Title: "Between Planets"
[230] - Book Title: "Stranger in a Strange Land"
[231] - Book Title: "A Soldier's Duty"
[232] - Book Title: "Swords Against Wizardry"
[233] - Book Title: "The Mote in God's Eye"
[234] - Book Title: "Uncharted Stars"
[235] - Book Title: "Raising Steam"
[236] - Book Title: "Ender's Game"
[237] - Book Title: "Foundation and Empire"
[238] - Book Title: "I, Robot"
[239] - Book Title: "The Hitchiker's guide to the Universe"
[240] - Book Title: "C Primer Plus"
[241] - Book Title: "On Basilisk Station"
[242] - Book Title: "The Sum of All Fears"
[243] - Book Title: "Battle Born"
[244] - Book Title: "Between Planets"
[245] - Book Title: "Stranger in a Strange Land"
[246] - Book Title: "A Soldier's Duty"
[247] - Book Title: "Swords Against Wizardry"
[248] - Book Title: "The Mote in God's Eye"
[249] - Book Title: "Uncharted Stars"
[250] - Book Title: "Raising Steam"
[251] - Book Title: "Ender's Game"
[252] - Book Title: "Foundation and Empire"
[253] - Book Title: "I, Robot"
[254] - Book Title: "The Hitchiker's guide to the Universe"
[255] - Book Title: "C Primer Plus"
[256] - Book Title: "On Basilisk Station"
[257] - Book Title: "The Sum of All Fears"
[258] - Book Title: "Battle Born"
[259] - Book Title: "Between Planets"
[260] - Book Title: "Stranger in a Strange Land"
[261] - Book Title: "A Soldier's Duty"
[262] - Book Title: "Swords Against Wizardry"
[263] - Book Title: "The Mote in God's Eye"
[264] - Book Title: "Uncharted Stars"
[265] - Book Title: "Raising Steam"
[266] - Book Title: "Ender's Game"
[267] - Book Title: "Foundation and Empire"
[268] - Book Title: "I, Robot"
[269] - Book Title: "The Hitchiker's guide to the Universe"
[270] - Book Title: "C Primer Plus"
[271] - Book Title: "On Basilisk Station"
[272] - Book Title: "The Sum of All Fears"
[273] - Book Title: "Battle Born"
[274] - Book Title: "Between Planets"
[275] - Book Title: "Stranger in a Strange Land"
[276] - Book Title: "A Soldier's Duty"
[277] - Book Title: "Swords Against Wizardry"
[278] - Book Title: "The Mote in God's Eye"
[279] - Book Title: "Uncharted Stars"
[280] - Book Title: "Raising Steam"
[281] - Book Title: "Ender's Game"
[282] - Book Title: "Foundation and Empire"
[283] - Book Title: "I, Robot"
[284] - Book Title: "The Hitchiker's guide to the Universe"
[285] - Book Title: "C Primer Plus"
[286] - Book Title: "On Basilisk Station"
[287] - Book Title: "The Sum of All Fears"
[288] - Book Title: "Battle Born"
[289] - Book Title: "Between Planets"
[290] - Book Title: "Stranger in a Strange Land"
[291] - Book Title: "A Soldier's Duty"
[292] - Book Title: "Swords Against Wizardry"
[293] - Book Title: "The Mote in God's Eye"
[294] - Book Title: "Uncharted Stars"
[295] - Book Title: "Raising Steam"
[296] - Book Title: "Ender's Game"
[297] - Book Title: "Foundation and Empire"
[298] - Book Title: "I, Robot"
[299] - Book Title: "The Hitchiker's guide to the Universe"
[300] - Book Title: "C Primer Plus"
[301] - Book Title: "Nice Book"
____________MENU____________
[i] - Get Book Information From Another File
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[b] - Book Information
[q] - Quit

b
Insert book location for information (1 - 301): 301]
[301] - BookTitle: Nice Book by [Yo] (2025)

Would you like to continue? (y/n)

y
Insert book location for information (1 - 301): 301
[301] - BookTitle: Nice Book by [Yo] (2025)

Would you like to continue? (y/n)

n
____________MENU____________
[i] - Get Book Information From Another File
[a] - Add Book
[w] - Write Data to Binary
[d] - Display Book List
[b] - Book Information
[q] - Quit

q
*/