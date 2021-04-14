#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>

const short delete_price = 7; // 7 bytes on delete
const short insert_price = 8; // 8 bytes on insert
const short replace_price = 8; // 8 bytes on replace

const char space = ' ';

unsigned long long min_3(unsigned long long first,
                         unsigned long long second,
                         unsigned long long third) {
    if(first < second) {
        if(first < third) {
           return first;
        } else {
           return third;
        }
    } else if(second < third) {
        return first;
    } else {
        return third;
    }
}


bool delete_writer(FILE*& tmp, char& oper, unsigned long& pos) {
    fwrite(&oper, sizeof(char), 1, tmp);
    fwrite(&space, sizeof(char), 1, tmp);
    fwrite(&pos, sizeof(unsigned long), 1, tmp);
    fwrite(&space, sizeof(char), 1, tmp);

    return 1;
}

bool default_writer(FILE*& tmp, char& oper, unsigned long& pos, char& symbol) {
    fwrite(&oper, sizeof(char), 1, tmp);
    fwrite(&space, sizeof(char), 1, tmp);
    fwrite(&pos, sizeof(unsigned long), 1, tmp);
    fwrite(&space, sizeof(char), 1, tmp);
    fwrite(&symbol, sizeof(char), 1, tmp);
    fwrite(&space, sizeof(char), 1, tmp);

    return 1;
}

bool diff_writer(FILE*& diff,
                 std::vector<char>& opers,
                 std::vector<unsigned long>& positions,
                 std::vector<char>& symbols) {
    for (int i = opers.size() - 1; i >= 0; --i) {
        if(opers[i] == 'd') {
            delete_writer(diff, opers[i], positions[i]);
        } else {
            default_writer(diff, opers[i], positions[i], symbols[i]);
        }
    }

    return 1;
}

int replace_price_calc(char& a, char& b) {
    return (a != b) * replace_price;
}

bool calculate_diff(FILE*& ans, std::vector<char>& old_data, std::vector<char>& new_data) {
    size_t Old_size = old_data.size();
    size_t New_size = new_data.size();
    unsigned long long Levenshtein_table[Old_size + 1][New_size + 1];

    Levenshtein_table[0][0] = 0;
    for(unsigned long j = 1; j <= New_size; ++j)
        Levenshtein_table[0][j] = Levenshtein_table[0][j - 1] + insert_price;

    for(unsigned long i = 1; i <= Old_size; ++i) {
        Levenshtein_table[i][0] = Levenshtein_table[i - 1][0] + delete_price;

        for(unsigned long j = 1; j <= New_size; ++j) {
            Levenshtein_table[i][j] = min_3(Levenshtein_table[i - 1][j] + delete_price,
                                                 Levenshtein_table[i][j - 1] + insert_price,
                                                 (Levenshtein_table[i - 1][j - 1] + replace_price_calc(old_data[i - 1], new_data[j - 1]))
                                      );
        }
    }

    long long i = Old_size, j = New_size;
    std::vector<char> operations;
    std::vector<unsigned long> positions;
    std::vector<char> new_symbols;

    while(i > 0 && j > 0) {
        unsigned long long MinStepValue = min_3(
               Levenshtein_table[std::max(0ll, i - 1)][std::max(0ll, j)],
               Levenshtein_table[std::max(0ll, i - 1)][std::max(0ll, j - 1)],
               Levenshtein_table[std::max(0ll, i)][std::max(0ll, j - 1)]
               );
        if(Levenshtein_table[std::max(0ll, i)][std::max(0ll, j)] == MinStepValue)
            continue;

        if(Levenshtein_table[std::max(0ll, i - 1)][std::max(0ll, j)] == MinStepValue)
        {
            operations.push_back('d');
            positions.push_back(i);
            new_symbols.push_back('a');
            --i;
            continue;
        }

        if(Levenshtein_table[std::max(0ll, i - 1)][std::max(0ll, j)] == MinStepValue)
        {
            operations.push_back('r');
            positions.push_back(i);
            new_symbols.push_back(new_data[j]);
            --i;
            --j;
            continue;
        }

        // if(Levenshtein_table[std::max(0ll, i - 1)][std::max(0ll, j)] == MinStepValue) because it's the last case
        {
            operations.push_back('i');
            positions.push_back(i);
            new_symbols.push_back(new_data[j]);
            --j;
            continue;
        }
    }

    diff_writer(ans, operations, positions, new_symbols);
    return 1;
}

bool update_file(FILE*& ans,
                 std::vector<char>& old_data,
                 std::vector<char>& opers,
                 std::vector<unsigned long>& poss,
                 std::vector<char>& symbols){
    unsigned long printed = 0;
    for(int i = 0; i < symbols.size(); ++i) {
        if(poss[i] - printed > 0) {
            fwrite(&old_data[printed], sizeof(char), poss[i] - printed, ans);\
            printed += (poss[i] - printed);
        }
        if(opers[i] == 'd') {
            ++printed;
        } else if(opers[i] == 'i') {
            fwrite(&symbols[i], sizeof(char), 1, ans);
        } else {
            fwrite(&symbols[i], sizeof(char), 1, ans);
            ++printed;
        }
    }

    fwrite(&old_data[printed], sizeof(char), old_data.size() - printed, ans);

    return 1;
}

bool oper_reader(FILE*& new_file,
                 std::vector<char>& opers,
                 std::vector<unsigned long>& poss,
                 std::vector<char>& symbols) {
    char tmp;
    unsigned long tmp_pos;

    while(!feof(new_file)) {
        fread(&tmp, sizeof(char), 1, new_file);
        opers.push_back(tmp);
        if(tmp == 'd') {
            fread(&tmp, sizeof(char), 1, new_file);
            fread(&tmp_pos, sizeof(unsigned long), 1, new_file);
            poss.push_back(tmp_pos);
            symbols.push_back(' ');
            fread(&tmp, sizeof(char), 1, new_file);
        } else {
            fread(&tmp, sizeof(char), 1, new_file);
            fread(&tmp_pos, sizeof(unsigned long), 1, new_file);
            poss.push_back(tmp_pos);
            fread(&tmp, sizeof(char), 1, new_file);
            fread(&tmp, sizeof(char), 1, new_file);
            symbols.push_back(tmp);
            fread(&tmp, sizeof(char), 1, new_file);
        }
    }
    return 1;
}

bool menu(){
    int user_ans = 0;
    std::cout << "What mode do you want to use?" << '\n' << "Difference mode: 1 and Update mode: 2" << '\n';
    std::cin >> user_ans;
    if(user_ans == 1) {
        std::vector<char> old_data;
        std::vector<char> new_data;
        FILE *old_file;
        if ((old_file = fopen("old_file", "rb"))==NULL) {
            printf ("Cannot open old file.\n");
            exit(1);
        }
        fseek(old_file, 0L, SEEK_END);
        old_data.resize(ftell(old_file));
        rewind(old_file);
        fread(&old_data[0], old_data.size()*sizeof(char), 1, old_file);

        FILE *new_file;
        if ((new_file = fopen("new_file", "rb"))==NULL) {
            printf ("Cannot open new file.\n");
            exit(1);
        }
        fseek(new_file, 0L, SEEK_END);
        new_data.resize(ftell(new_file));
        rewind(new_file);

        fread(&new_data[0], new_data.size()*sizeof(char), 1, new_file);

        FILE *ans;
        if ((ans = fopen("diff", "wb"))==NULL) {
            printf ("Cannot open answer file.\n");
            exit(1);
        }

        calculate_diff(ans, old_data, new_data);
        fclose(ans);
        return 1;
    }

    if(user_ans == 2){
        std::vector<char> old_data;
        std::vector<char> operations;
        std::vector<unsigned long> positions;
        std::vector<char> new_symbols;
        FILE *old_file;
        if ((old_file = fopen("old_file", "rb"))==NULL) {
            printf ("Cannot open old_file file.\n");
            exit(1);
        }
        fseek(old_file, 0L, SEEK_END);
        old_data.resize(ftell(old_file));
        rewind(old_file);
        fread(&old_data[0], old_data.size()*sizeof(char), 1, old_file);

        FILE *new_file;
        if ((new_file = fopen("update_file", "rb"))==NULL) {
            printf ("Cannot open update file.\n");
            exit(1);
        }
        oper_reader(new_file, operations, positions, new_symbols);

        FILE *ans;
        if ((ans = fopen("updated_file", "wb"))==NULL) {
            printf ("Cannot open updated file.\n");
            exit(1);
        }

        update_file(ans, old_data, operations, positions, new_symbols);
        fclose(ans);
        return 1;
    }

    std::cout << "Wrong input" << '\n';
    menu();
}

int main()
{
    menu();
    return 0;
}
