#define _CRT_SECURE_NO_DEPRECATE //Wrote while using visual studio, which deprecated a lot of calls (like fopen) for being unsafe
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/***********************
Dictionary File Format:
Word synonym synonym\n

Words will be sorted in alphabetical order - max 4 synonyms
*/
#define NEW_DATA "synon_text.txt"
#define ARG_NUM 3 //Expecting 2 arguments (first arg is programs name)
#define WORD_LEN 100
#define LINE_LEN 100 //maximum length of a line expected for the program (reading shoulld stop on '\n' before reaching 100 bytes)
#define MAX_SYN 4
#define ASCII_SMALL 32 //number to add to capital letters to decapitalize them

char * findSynoms(FILE *p, char *word);
char * randSynoms(char *line);
int compare(char *word, char *letter);

int main(int argc, char *argv[])
{
	FILE *dict; //File containing the words and synonyms
	FILE *data; //File containing the english text for edit
	FILE *newData; //File to save the english text after editing
	char *word = malloc(WORD_LEN);

	if (argc != ARG_NUM)
	{
		return 1;
	}
	data = fopen(argv[2], "r");
	dict = fopen(argv[1], "r");
	newData = fopen(NEW_DATA, "w"); //
	if (data == NULL || dict == NULL) //Check if files have opened succefuly
	{
		return 1;
	}
	//All is good till now
	while (fscanf(data, "%s", word) != EOF)
	{
		word = findSynoms(dict, word);
		fputs(word, newData);
		fputc(' ', newData);
	}

	fclose(dict);
	fclose(data);
	fclose(newData);
	//system("PAUSE");
}

char * findSynoms(FILE *p, char *word)
{
	/*
	Function receives a word and a file descriptor for the dictionary, then searches for a matching synonym for the word
	If found, one of the synonyms is saved to *word*, otherwise, *word* is unchanged
	*/
	char *letter = malloc(LINE_LEN);
	char *temp = malloc(LINE_LEN);
	char *l;
	//for (fscanf(p, "%s", letter); letter != EOF; fscanf(p, "%s", letter)) //compare the first letter of the word in the dictionary
	for (l = fgetc(p); l != EOF; l = fgetc(p))
	{
		if (word[0] == l)
		{
			fseek(p, -1, SEEK_CUR);
			fscanf(p, "%s", letter);
			if (compare(word, letter))
			{
				fgets(temp, LINE_LEN, p);
				word = randSynoms(temp + 1); //using temp + 1 because the space that was left after reading the first word is not needed
				break;
			}
		}
		else
		{
			fgets(temp, 100, p);
		}
		
	}
	fseek(p, 0, SEEK_SET);
	return word;
}

char * randSynoms(char *line)
{
	//Functions receieves a line (word follown by synonyms) and parses by spaces. a synonym is return randomly
	static char *synoms[MAX_SYN] = { 0 }; //synoms is static so i won't have to reallocate memory every t
	int ran, i, j = 0, length;
	char *word = malloc(WORD_LEN);
	srand(time(NULL));
	length = strlen(line);
	for (i = 0; i < MAX_SYN && j < length; i++) //looping on each synonym (max 4) and adding it to the array
	{
		if (synoms[i] == NULL)
		{
			synoms[i] = malloc(WORD_LEN);
		}
		for (j = 0; line[j] != ' ' && line[j] != NULL; j++) //parsing line by spaces
		{
			if (line[j] == '\n')
			{
				break;
			}
			synoms[i][j] = line[j];
		}
		synoms[i][j] = NULL;
		line = (line + j + 1);

	}
	//finding the last occupied cell
	for (i = 0; i < MAX_SYN; i++)
	{
		if (strlen(synoms[i]) == 0)
		{
			break;
		}
	}
	ran = rand() % i; //choose random synonym to replace word, excludes first cell which contains original word
	return synoms[ran];
}

int compare(char *word, char *letter)
{
	/*
	Function recieves two strings, changes them to small letters and then compares (using strcmp).
	Return 1 if true and 0 if false
	*/
	char *str = word; //create a copy by value to avoid changing of original words
	char *cmp = letter;
	int i, len;
	len = strlen(str); //saving a call to strlen again
	if (len != strlen(cmp))
	{
		return 0;
	}
	for (i = 0; i < len; i++)
	{
		if (str[i] < 'a')
		{
			str[i] = str[i] + ASCII_SMALL;
		}
		if (cmp[i] < 'a')
		{
			cmp[i] = cmp[i] + ASCII_SMALL;
		}
	}
	if (!strcmp(str, cmp)) // i hate that strcmp returns 0 if equal (1/-1 for bigger than or smaller than strings)
	{
		return 1;
	}
	return 0;
}