/**
 * @file runner.c

 * @author kate (zanetty54@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2022-05-13
 * @copyright Copyright (c) 2022
 */

/*NOTE: program still assumes users provides correct arguments*/
/*for the different options. Error check still on the table*/
/*to be delivered sometime in the future*/

/*__README__ FOR MORE INFO*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define and         &&
#define or          ||
#define bool        char
#define true        1
#define false       0
#define STDOUT      1
#define STDIN       0
#define STDERR      2
#define BUFFERSIZE  256

#define HELP        "--help"
#define PLAYERS     "-p"
#define SEED        "-s"
#define GAMES       "-n"
#define VERSION     "--version"
#define FILE_OUTPUT "-o"
#define VERSION_MAJOR   1
#define VERSION_MINOR   0

/*compare two strings. return true if they match exactly*/
bool string_comp(const char* str1, const char* str2)
{
    bool result = true;
    int size1 = strlen(str1);
    int size2 = strlen(str2);
    if (size1 != size2) result = false;
    int i = 0; 
    int j = 0;
    while (i < size1 && j < size2 and result)
    {
        if (str1[i] != str2[j]) result = false;
        ++i;
        ++j;
    }
    return result;
}

/*program usage help*/
void usage(void)
{
    char output_buffer[BUFFERSIZE];
    sprintf(output_buffer, "usage: ./runner [OPTIONS]...\n");
    write(STDOUT, output_buffer, strlen(output_buffer));
    sprintf(output_buffer, "this program runs Moria games with random seeds\n");
    write(STDOUT, output_buffer, strlen(output_buffer));
    sprintf(output_buffer, "enter --help command for more help on commands\n");
    write(STDOUT, output_buffer, strlen(output_buffer));
    exit(EXIT_SUCCESS);
}

/*display program version*/
void display_version(void)
{
    char output_buffer[BUFFERSIZE];
    sprintf(output_buffer, "Moria runner scuffed: %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
    write(STDOUT, output_buffer, strlen(output_buffer));
    sprintf(output_buffer, "release date: 5/13/2022 3:54 AM\n");
    write(STDOUT, output_buffer, strlen(output_buffer));
    exit(EXIT_SUCCESS);
}

/*print str to std error and exit program*/
void error_and_exit(const char* str, int ex_cd)
{
    perror(str); exit(ex_cd);
}

/*display program help*/
void display_help(void)
{
    char buffer[BUFFERSIZE];
    sprintf(buffer, "./runner --help\n");
    write(STDOUT, buffer, strlen(buffer));
    sprintf(buffer, "\t(p): enter game players -> -p player1 player2 player3 player4\n");
    write(STDOUT, buffer, strlen(buffer));
    sprintf(buffer, "\t(s): enter game seed -> -s [seed_value]\n");
    write(STDOUT, buffer, strlen(buffer));
    sprintf(buffer, "\t(n): enter number of games -> -n [number_games]\n");
    write(STDOUT, buffer, strlen(buffer));
    sprintf(buffer, "\t(o): enter file output suffix -> -o file_name_suffix\n");
    write(STDOUT, buffer, strlen(buffer));
    sprintf(buffer, "\t(version): display program version and exit -> --version\n");
    write(STDOUT, buffer, strlen(buffer));
    sprintf(buffer, "\t(help): display program help and exit -> --help\n");
    write(STDOUT, buffer, strlen(buffer));
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    char option_pos[4];
    option_pos[0] = option_pos[1] = -1;
    option_pos[2] = option_pos[3] = -1;
    /**
     * options_pos[0] -> GAMES 
     * options_pos[1] -> SEED
     * options_pos[2] -> PLAYERS
     * options_pos[3] -> FILE_OUTPUT
     */
    for (int i = 1; i < argc; ++i) 
    {
        int result = string_comp(argv[i], HELP);
        int result2 = string_comp(argv[i], VERSION);
        if (result) display_help();
        else if (result2) display_version();
        
        int res1 = string_comp(argv[i], PLAYERS);
        int res2 = string_comp(argv[i], SEED);
        int res3 = string_comp(argv[i], GAMES);
        int res4 = string_comp(argv[i], FILE_OUTPUT);

        if (res1) option_pos[2] = i;
        if (res2) option_pos[1] = i;
        if (res3) option_pos[0] = i;
        if (res4) option_pos[3] = i;

    }

    if (argc < 3) usage();
    
    /*must specify at least number of games*/
    if (option_pos[0] == -1) 
    {
        char buff[128];
        sprintf(buff, "user must specify minimum amount of games\n");
        write(STDOUT, buff, strlen(buff));
        exit(EXIT_SUCCESS);
    }

    /*must specify all players*/
    if (option_pos[2] == -1) 
    {
        char buff[128];
        sprintf(buff, "user must specify all four players\n");
        write(STDOUT, buff, strlen(buff));
        exit(EXIT_SUCCESS);
    }
    
    int n_GAMES = atoi(argv[option_pos[0]+1]);
    for (int i = 0; i < n_GAMES; ++i)
    {
        pid_t game_instance = fork();
        if (game_instance == -1) error_and_exit("__fork__ error syscall", -1);
        else if (game_instance == 0)
        {   
            char file_out[128];
            char default_res[128];
            sprintf(default_res, "default_%d.res", i);
            if (option_pos[3] == -1)
            {
                /*user didnt specify a file output prefix*/
                sprintf(file_out, "result_instance_%d", i);
            }
            else sprintf(file_out, "%s%d", argv[(option_pos[3]+1)], i);
            int fileHandle = open(file_out, O_WRONLY | O_CREAT | O_TRUNC,  S_IRWXU);
            if (fileHandle == -1) error_and_exit("__open__ error syscall at child", -1);
            close(STDERR); dup(fileHandle);
            char* const arguments[] = {
                             "./Game",
                argv[option_pos[2]+1],
                argv[option_pos[2]+2],
                argv[option_pos[2]+3],
                argv[option_pos[2]+4],
                "-s",
                argv[(option_pos[1]+1)],
                "-i",
                "default.cnf",  //this file should containt the game info such board size, 
                                //etc we need one per game unless you want to run games with different setupos
                "-o",
                default_res,
                (char*)NULL
            };
            execvp("./Game", arguments);
        }
    }
    while (waitpid(-1, NULL, 0) > 0);



}