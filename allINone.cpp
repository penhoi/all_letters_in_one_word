#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

#include <fstream>
#include <vector>
#include <string>
using namespace std;

class dict_t {

#define     MAX_WORD_LEN    20

    public:
        dict_t(char *dict_path);
        ~dict_t();

        bool letters_in_word(char *letters, int distance = 1);
        bool word_in_letters(char *letters, int distance = 1);

    private:
        enum enum_match {match_letters2word, match_word2letters};

        bool find_word(int word_len, char *letters, enum_match e);

    private:
        typedef vector<char*> vecWords;

        char *m_path;
        vecWords m_words[MAX_WORD_LEN+1];
};


dict_t::dict_t(char *dict_path)
{
    ifstream f(dict_path, ios::in);
    if (!f.is_open()) {
        perror("Open dictionary file failed");
        exit(1);
    }

    //read words from dictionary
    string line;

    // we use the dictionary: american-english
    while (getline(f, line)) {
        // Get rid of words like /Adirondacks's/
        if (string::npos != line.rfind('\''))
            continue;

        if (line.length() < 3 || line.length() > MAX_WORD_LEN)
            continue;

        if (!isalpha(line[0]) || !isalpha(line[1]))
            continue;

        //printf("%s\n", line);
        m_words[line.length()].push_back(strdup(line.c_str()));
    }//end while

    f.close();

    m_path = strdup(dict_path);
}

dict_t::~dict_t()
{
    for(int i = 0; i <= MAX_WORD_LEN; i++) {
        vector<char*>::iterator it;

        for (it = m_words[i].begin(); it != m_words[i].end(); it++) {
            free(*it);
        }

        m_words[i].clear();
    }

    free(m_path);
}


bool dict_t::letters_in_word(char *letters, int distance)
{
    char low[MAX_WORD_LEN+1];
    int nlen = strlen(letters);
    int i;

    //to lowercase
    if (nlen > MAX_WORD_LEN)
        return false;

    for (i = 0; i < nlen; i++) {
        low[i] = tolower(letters[i]);
    }
    low[i] = 0;


    int d = min(nlen+distance, MAX_WORD_LEN);
    while (nlen <= d && !find_word(nlen, letters, match_letters2word)) {
        nlen++;
    }

    return nlen != MAX_WORD_LEN;
}

bool dict_t::word_in_letters(char *letters, int distance)
{
    char low[MAX_WORD_LEN+1];
    int nlen = strlen(letters);
    int i;

    //to lowercase
    if (nlen > MAX_WORD_LEN)
        return false;

    for (i = 0; i < nlen; i++) {
        low[i] = tolower(letters[i]);
    }
    low[i] = 0;


    int d = max(nlen-distance, 2);
    while (nlen >= d && !find_word(nlen, letters, match_word2letters)) {
        nlen--;
    }

    return nlen != 2;
}


bool dict_t::find_word(int word_len, char *letters, dict_t::enum_match ematch)
{
    vector<char*>::iterator it;
    char *word;
    bool ret = false;

    int larr[26] = {0};
    int i;
    for (i = 0; i < strlen(letters); i++)
        larr[letters[i]-'a']++;

    for (it = m_words[word_len].begin(); it != m_words[word_len].end(); it++) {
        word = *it;

        int warr[26] = {0};
        for (int j = 0; j < strlen(word); j++) {
            if (word[j] < 'a')
                warr[word[j]-'A']++;
            else
                warr[word[j]-'a']++;
        }

        bool bmatched = true;
        switch (ematch) {
            case match_letters2word:
                for (i = 0; i < 26; i++) {
                    if (larr[i] > warr[i])
                        bmatched = false;
                }
                break;

            case match_word2letters:
                for (i = 0; i < 26; i++) {
                    if (warr[i] > larr[i])
                        bmatched = false;
                }
                break;
        }
        if (bmatched) {
            printf("%s\n", word);
            ret = true;
        }
    }
    return ret;
}


bool find_word(char *fdict, char *letters, int mode = 0, int distance = 2)
{
    char low[MAX_WORD_LEN+1];
    int nlen = strlen(letters);
    int i;

    //to lowercase
    if (nlen > MAX_WORD_LEN)
        return false;

    for (i = 0; i < nlen; i++) {
        low[i] = tolower(letters[i]);
    }
    low[i] = 0;

    //start finding
    dict_t dict(fdict);

    if (mode == 0)
        return dict.letters_in_word(letters, distance);
    else if (mode == 1)
        return dict.word_in_letters(letters, distance);
    else
        return false;
}


void Usage(const char *prog)
{
    printf("Description: Extract the first letter of each word of a word list to generate an easily-rememberable word.\n"
            "\nUsage:\n"
            "\t%s [-h This help]\n"
            "\t [-d path of dictionary]\n"
            "\t [-m letters_in_word or word_in_letters, l2w or w2l]\n"
            "\t [-l the maximum distance between target word and the set of given letters]\n"
            "\t <-w a list of words>\n", prog);

    exit(0);
}

int main(int argc, char* argv[])
{
    char *fdict = NULL;
    char *mode = NULL;
    char *words = NULL;
    int maxdistance = 22;
    int opt;

    while ((opt = getopt(argc, argv, "hd:m:l:w:")) != -1) {
        switch(opt) {
            case 'd':
                fdict = strdup(optarg);
                break;

            case 'm':
                mode = strdup(optarg);
                break;

            case 'l':
                maxdistance = atoi(optarg);
                break;

            case 'w':
                words = strdup(optarg);
                break;

            case 'h':
            default:
                Usage(argv[0]);
                break;
        }
    }

    if (optind != argc) {
        Usage(argv[0]);
    }
    if (fdict == NULL || mode == NULL || words == NULL)
        Usage(argv[0]);

    int m;
    if (!strcasecmp("l2w", mode))
        m = 0;
    else if (!strcasecmp("w2l", mode))
        m = 1;
    else
        Usage(argv[0]);


    //Get the first letter from each word.
    char *ch = words;
    char letters[64];
    int i = 0;

    while (*ch != '\0') {
        while (*ch != '\0' && *ch == ' ') ch++;

        if (isalpha(*ch)) {
            letters[i++] = *ch;
            ch++;
        }

        while (*ch != '\0' && *ch != ' ') ch++;
    }
    letters[i] = 0;

    //printf("word list: %s\n", words);
    find_word(fdict, letters, m, maxdistance);

    return 0;
}
