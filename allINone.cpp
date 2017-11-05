#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

#include <vector>
using namespace std;

class dict_t {

    public:
        dict_t(char *dict_path);
        ~dict_t();

        bool find_word(char *letters);

    private:
        bool find_word(int word_len, char *letters);
        bool finer_match(char* word, char *letters);

    private:
#define     MAX_WORD_LEN    20
        typedef vector<char*> vecWords;

        char *m_path;
        vecWords m_words[MAX_WORD_LEN+1];
};


dict_t::dict_t(char *dict_path)
{
    FILE *f = fopen(dict_path, "r");
    if (f == NULL) {
        perror("Open dictionary file failed");
        exit(1);
    }

    //read in diction
    size_t size = 40;
    char *line = (char*)malloc(size);
    int nread;

    // we use the dictionary: american-english
    while ((nread = getline(&line, &size, f)) != EOF) {
        // Get rid of words like /Adirondacks's/
        if (NULL != strchr(line, '\''))
            continue;

        char *p = strpbrk(line, "\n");
        int nlen;

        if (p == NULL)
            nlen = strlen(line);
        else
            nlen = p - line;

        if (nlen < 3 || nlen > MAX_WORD_LEN)
            continue;

        if (!isalpha(line[0]) || !isalpha(line[1]))
            continue;

        //printf("%s\n", line);
        m_words[nlen].push_back(strdup(line));
    }//end while

    fclose (f);

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

bool dict_t::find_word(char *letters)
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


    while (nlen < MAX_WORD_LEN && !find_word(nlen, letters)) {
        nlen++;
    }

    return nlen != MAX_WORD_LEN;
}


bool dict_t::find_word(int word_len, char *letters)
{
    vector<char*>::iterator it;
    char *word;
    bool ret = false;

    for (it = m_words[word_len].begin(); it != m_words[word_len].end(); it++) {
        word = *it;

        int lidx = 0;
        int widx = 0;
        int count = 0;

        for (int i = 0; i < strlen(letters); i++) {
            for (int j = 0; j < word_len; j++) {
                if (letters[i] == word[j]) {
                    count++;
                    break;
                }
            }

            if (count == strlen(letters) && finer_match(word, letters)) {
                printf("%s", word);
                ret = true;
            }
        }
    }
    return ret;
}

bool dict_t::finer_match(char* word, char* letters)
{
    int arr[26];
    int i;

    for (i = 0; i < 26; i++)
        arr[i] = 0;

    for (i = 0; i < strlen(letters); i++)
        arr[letters[i]-'a']++;

    for (i = 0; i < 26; i++) {
        if (arr[i] > 1) {
            int cnt = 0;
            int j;

            for (j = 0; j < strlen(word); j++) {
                if (word[j] == 'a' + i)
                    cnt++;
            }

            if (cnt < arr[i])
                return false;
        }
    }

    return true;
}

void Usage(const char *prog)
{
    printf("Description: Extract the first letter of each word of a word list to generate an easily-rememberable word.\n"
            "\nUsage:\n"
            "\t%s [-d path of dictionary]\n"
            "\t [-h This help]\n"
            "\t <-w a list of words>\n", prog);

    exit(0);
}

int main(int argc, char* argv[])
{
    char *fdict;
    char *words;
    int opt;

    while ((opt = getopt(argc, argv, "dwh")) != -1) {
        switch(opt) {
            case 'd':
                fdict = strdup(argv[optind]);
                break;
            case 'w':
                words = strdup(argv[optind]);
                break;

            case 'h':
            default:
                Usage(argv[0]);
                break;
        }
    }

    if (optind >= argc) {
        Usage(argv[0]);
    }


    //Get the first letter from each word.
    char *ch = words;
    char letters[64];
    int i = 0;

    while (*ch != '\0') {
        while (*ch != '\0' && *ch == ' ') ch++;

        if (isalpha(*ch)) {
            letters[i++] = tolower(*ch);
            ch++;
        }

        while (*ch != '\0' && *ch != ' ') ch++;
    }
    letters[i] = 0;

    //printf("word list: %s\n", words);

    //start finding
    dict_t dict(fdict);
    dict.find_word(letters);

    return 0;
}
