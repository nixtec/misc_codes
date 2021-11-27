// David

#define MAX_DIR_PATH 2048
#define MAX_LINE_LEN 1000

extern char* curr_dir();
extern void list();
extern void move(char* source_path, char* dest_path);
extern void copy(char* source_path, char* dest_path);
extern void change_dir();
