#ifndef _HELPER_H_
#define _HELPER_H_

char** InputPiecesSemiColon(char* input , char a);
void functionTable(char** arr , char* path);
void StringArray(char* str1 , char* str2,int basicReplace, int basicReplaceIncaseSensetive, int BracketSituation,int startReplace ,int EndReplace ,int StarReplace , char* path);
char* starBracketFunction(char* buf , char* str1 ,char* str2,int b,int startReplace ,int EndReplace);

#endif