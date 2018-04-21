/* 
 * Author: Timo Kumpulainen    Studentnumber: 1961889
 * 
 * Function of the program: To save and sort out, and
 * print 100 most popular words of a textfile.
 */

/* Used headers and pre-defined values */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MOST_USED 100
#define FILE_NAME_SIZE 50
#define MAX_WORD_LENGTH 25

/*
 * countWords function: Reads every character in file and
 * decides if they are really a words or to be discarded.
 */

int countWords(FILE * input) {
    int ch, i = 0, wordCount = 0;
    char utility[MAX_WORD_LENGTH], character;
    for (ch = fgetc(input); ch != EOF; ch = fgetc(input)) {
        if (ch > 64 && ch < 91 || ch > 96 && ch < 123 || ch == 39) {
            character = (char) ch;
            if (ch != 39) {
                character = toupper(character);
            }
            utility[i] = character;
            utility[i + 1] = '\0';
            i++;
        } else if (utility[0] != 0 && (ch < 65 && ch > 90 || ch < 97 && ch > 122 || ch != 39)) {
            memset(utility, '\0', sizeof (char)*MAX_WORD_LENGTH);
            wordCount = wordCount + 1;
            i = 0;
        }
    }
    return (wordCount);
}

/*
 * wordCounter function: Tries to open file for counting
 * the words inside of it.
 */

int wordCounter(char * fileName) {
    int wordCount;
    FILE *input;
    if ((input = fopen(fileName, "r")) == NULL) {
        printf("Failed to read file %s, aborting . . . ", fileName);
    }
    wordCount = countWords(input);
    fclose(input);
    return (wordCount);
}

/*
 * hashWord function: Uses algorith to create checksums
 * out of saved words.
 */

int hashWord(char *wordToHash, int hashKey) {

    int i, checkSum;
    unsigned long long sum = 0;
    char temp[MAX_WORD_LENGTH];
    memset(temp, '\0', sizeof (char)*MAX_WORD_LENGTH);

    for (i = 0; i < strlen(wordToHash); i++) {
        temp[i] = wordToHash[i];
    }

    sum = (temp[0] + temp[1] * 256);

    for (i = 2; i < strlen(temp); i++) {
        sum = sum + ((int) temp[i]) * (int) (pow((double) 256, (double) i));
    }
    checkSum = (int) sum % hashKey;

    return (checkSum);
}

/*
 * collisionHandler function: If word collided with another one
 * because of checksum in addToHashTable function, collisionHandler
 * uses linear probing to handle the collisions.
 */

void collisionHandler(char **allWords, int wordHashValue, char *wordToHash, int hashKey, int *wordAmounts) {

    int i, result = 0;
    i = 0;

    while (result < 1) {
        if (strcmp(allWords[wordHashValue], wordToHash) == 0) {
            wordAmounts[wordHashValue] = wordAmounts[wordHashValue] + 1;
            result++;
        }
        if (strcmp(allWords[wordHashValue], "\0") == 0) {
            strcpy(allWords[wordHashValue], wordToHash);
            wordAmounts[wordHashValue] = wordAmounts[wordHashValue] + 1;
            result++;
        }
        wordHashValue++;
        if (wordHashValue >= hashKey) {
            wordHashValue = 0;
        }
    }
}

/*
 * addToHashTable function: Calls for hash function to every saved
 * word and uses the checksum to save words to array. It also keeps
 * record of the occurrence amounts.
 */

void addToHashTable(int *wordAmounts, char **allWords, int hashKey, char *wordToSave) {
    int wordHashValue, i;
    wordHashValue = hashWord(wordToSave, hashKey);
    if (wordHashValue < 0) {
        wordHashValue = wordHashValue * -1;
    }
    if (strcmp(allWords[wordHashValue], wordToSave) == 0) {
        wordAmounts[wordHashValue] = wordAmounts[wordHashValue] + 1;
    }

    if (strcmp(allWords[wordHashValue], "\0") == 0) {
        strncpy(allWords[wordHashValue], wordToSave, MAX_WORD_LENGTH);
        wordAmounts[wordHashValue] = 0;
        wordAmounts[wordHashValue] = wordAmounts[wordHashValue] + 1;
    }
    if (strcmp(allWords[wordHashValue], wordToSave) != 0) {
        collisionHandler(allWords, wordHashValue, wordToSave, hashKey, wordAmounts);
    }
}

/*
 * saveWords function: Reads every single character in file and
 * decides if they will be kept or ignored. 
 */

saveWords(FILE *input, int *wordAmounts, char **allWords, int hashKey) {
    int ch, i = 0;
    char wordToSave[MAX_WORD_LENGTH], character;
    char *wordToSavePtr;
    memset(wordToSave, '\0', sizeof (char)*MAX_WORD_LENGTH);
    wordToSavePtr = &wordToSave[0];
    for (ch = fgetc(input); ch != EOF; ch = fgetc(input)) {
        if (ch > 64 && ch < 91 || ch > 96 && ch < 123 || ch == 39) {
            character = (char) ch;
            if (ch != 39) {
                character = toupper(character);
            }
            wordToSave[i] = character;
            i++;
        } else if (wordToSave[0] != '\0' && (ch < 65 && ch > 90 || ch < 97 && ch > 122 || ch != 39)) {
            addToHashTable(wordAmounts, allWords, hashKey, wordToSavePtr);
            memset(wordToSave, '\0', sizeof (char)*MAX_WORD_LENGTH);
            i = 0;
        }
    }
}

/*
 * readFileToArray function: Tries to open file for reading and
 * saving of all words in the file.
 */

void readFileToArray(char *fileName, int *wordAmounts, char **allWords, int hashKey) {

    FILE *input;

    if ((input = fopen(fileName, "r")) == NULL) {
        printf("Failed to read file %s, aborting . . . ", fileName);
    }

    saveWords(input, wordAmounts, allWords, hashKey);

    fclose(input);
}

/*
 * cleanArrays function: Meant to be used in making smaller
 * arrays to store just all different words, not every single
 * word that occurs in file.
 */

int cleanArrays(int *wordAmounts, char **allWords, int wordCount) {
    int i, differentWords = 0;

    for (i = 0; i < wordCount; i++) {
        if (wordAmounts[i] == 0 || strcmp(allWords[i], "\0") == 0) {
            wordAmounts[i] = 0;
        }
        if (strcmp(allWords[i], "\0") != 0 && wordAmounts[i] != 0) {
            differentWords++;
        }
    }
    return (differentWords);
}

/*
 * copyArrays function: Copies all words and occurrence amounts into
 * smaller arrays.
 */

void copyArrays(char **allWords, char **finalWords, int *allAmounts, int *finalAmounts, int wordCount) {
    int i = 0, j = 0;

    for (i = 0; i < wordCount; i++) {
        if (strcmp(allWords[i], "\0") != 0 && allAmounts[i] != 0) {
            strcpy(finalWords[j], allWords[i]);
            finalAmounts[j] = allAmounts[i];
            j++;
        }
    }
}

/*
 * comp function: Used in quicksort function in makeFinalArray function.
 */

int comp(const void * elem1, const void * elem2) {
    int f = *((int*) elem1);
    int s = *((int*) elem2);
    if (f > s) return 1;
    if (f < s) return -1;
    return 0;
}

/*
 * makeFinalArray function: Uses already gathered information to
 * save needed words and occurrence amounts to array meant for
 * printing the information to user.
 */

void makeFinalArray(char **mostUsedWords, int *mostUsedAmounts, char **finalWords, int *finalAmounts, int differentWords) {
    int i = 0, j = 0, temp = 0, tempArray[differentWords], result = 0, loopTime = MOST_USED;

    if (differentWords < MOST_USED) {
        loopTime = differentWords;
    }
    memset(tempArray, '\n', differentWords);
    for (i = 0; i < differentWords; i++) {
        tempArray[i] = finalAmounts[i];
    }
    qsort(finalAmounts, differentWords, sizeof (int), comp);
    j = differentWords - 1;
    i = 0;
    while (i < j) {
        temp = finalAmounts[i];
        finalAmounts[i] = finalAmounts[j];
        finalAmounts[j] = temp;
        i++;
        j--;
    }
    j = 0;
    for (i = 0; i < loopTime; i++) {
        if (finalAmounts[i] == tempArray[j]) {
            strcpy(mostUsedWords[i], finalWords[j]);
            j++;
        } else if (finalAmounts[i] != tempArray[j]) {
            while (result < 1) {
                if (finalAmounts[i] == tempArray[j]) {
                    strcpy(mostUsedWords[i], finalWords[j]);
                    result++;
                }
                j++;
                if (j > differentWords) {
                    j = 0;
                }
            }
            result = 0;
        }
        mostUsedAmounts[i] = finalAmounts[i];
    }
}

/* Main function: Calls other functions to save and manipulate
 * arrays containing textual content from a file. Also allocates
 * dynimically memory for arrays used to save text strings and 
 * integers.  
 */
int main(int argc, char** argv) {

    /* Fields and pointers for main */

    char fileName[FILE_NAME_SIZE], rowLength = MAX_WORD_LENGTH;
    char **allWords, **finalWords, **mostUsedWords;
    char *temp, *temp2, *temp3;
    int wordCount, hashKey, i = 0, differentWords = 0;
    int *wcp, *allWordsAmounts, *finalWordsAmounts, *mostUsedAmounts;
    clock_t begin, end;
    double time_spent;

    /*  Main logic begins with counting of words in file and
     *  allocating memory for arrays used in final print. After
     *  that it allocates memory for all words and their occurrence
     *  array.
     */

    printf("Enter file name to be read: ");
    scanf("%s", fileName);
    begin = clock();
    wordCount = wordCounter(fileName);
    printf("Wordcount = %d\n", wordCount);
    hashKey = wordCount;

    mostUsedWords = malloc(MOST_USED * sizeof (char*));
    if (mostUsedWords == 0) {
        printf("ERROR: out of memory.");
    }
    temp3 = (char *) malloc(MOST_USED * rowLength * sizeof (char));
    if (temp3 == 0) {
        printf("ERROR: out of memory");
    }
    for (i = 0; i < MOST_USED; i++) {
        mostUsedWords[i] = temp3 + (i * rowLength);
    }
    mostUsedAmounts = calloc(MOST_USED, sizeof (int));

    for (i = 0; i < MOST_USED; i++) {
        memset(mostUsedWords[i], '\0', sizeof (char)*MAX_WORD_LENGTH);
    }
    memset(mostUsedAmounts, 0, MOST_USED * sizeof (int));

    allWords = malloc(wordCount * sizeof (char*));
    if (allWords == 0) {
        printf("ERROR: out of memory.");
    }
    temp = (char *) malloc(wordCount * rowLength * sizeof (char));
    if (temp == 0) {
        printf("ERROR: out of memory");
    }
    for (i = 0; i < wordCount; i++) {
        allWords[i] = temp + (i * rowLength);
    }
    allWordsAmounts = calloc(wordCount, sizeof (int));

    for (i = 0; i < wordCount; i++) {
        memset(allWords[i], '\0', sizeof (char)*MAX_WORD_LENGTH);
    }
    memset(allWordsAmounts, 0, wordCount * sizeof (int));

    /*
     * At this point program reads file and saves the words in array
     * and also keeps count of their occurrences. After that it uses
     * differentWords variable to allocate memory for each different
     * words used.
     */

    readFileToArray(fileName, allWordsAmounts, allWords, hashKey);

    differentWords = cleanArrays(allWordsAmounts, allWords, wordCount);

    finalWords = malloc(differentWords * sizeof (char*));
    if (finalWords == 0) {
        printf("ERROR: out of memory.");
    }
    temp2 = (char *) malloc(differentWords * rowLength * sizeof (char));
    if (temp2 == 0) {
        printf("ERROR: out of memory");
    }
    for (i = 0; i < differentWords; i++) {
        finalWords[i] = temp2 + (i * rowLength);
    }

    finalWordsAmounts = calloc(differentWords, sizeof (int));
    for (i = 0; i < differentWords; i++) {
        memset(finalWords[i], '\0', sizeof (char)*MAX_WORD_LENGTH);
    }
    memset(finalWordsAmounts, 0, differentWords * sizeof (int));

    /*
     * Now program copies words from all words array and stores them
     * in new, smaller array allocated for them. After that it uses
     * the information gathered to make the 100 index arrays meant
     * for printing out the information needed for the user.
     */

    copyArrays(allWords, finalWords, allWordsAmounts, finalWordsAmounts, wordCount);

    makeFinalArray(mostUsedWords, mostUsedAmounts, finalWords, finalWordsAmounts, differentWords);

    /*
     * Finally program prints information, deallocates memory and closes.
     */

    printf("Number of different words = %d\n", differentWords);
    for (i = 0; i < MOST_USED; i++) {
        printf("%d. word: %15s \t, occurrences %d\n", i + 1, mostUsedWords[i], mostUsedAmounts[i]);
    }

    free(temp);
    free(temp2);
    free(temp3);
    free(allWords);
    free(finalWords);
    free(mostUsedWords);
    free(allWordsAmounts);
    free(finalWordsAmounts);
    free(mostUsedAmounts);

    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent was: %f\n", time_spent);
    
    return (EXIT_SUCCESS);
}
