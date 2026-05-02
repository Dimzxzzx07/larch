#include <stdint.h>
#include <string.h>

#define MAX_HISTORY 20
#define MAX_CMD_LEN 256

static char history[MAX_HISTORY][MAX_CMD_LEN];
static int history_count = 0;
static int history_position = 0;
static int current_history_index = -1;

void history_add(const char* cmd) {
    if (history_count < MAX_HISTORY) {
        strcpy(history[history_count], cmd);
        history_count++;
    } else {
        for (int i = 1; i < MAX_HISTORY; i++) {
            strcpy(history[i-1], history[i]);
        }
        strcpy(history[MAX_HISTORY-1], cmd);
    }
    current_history_index = history_count;
    if (history_count > MAX_HISTORY) current_history_index = MAX_HISTORY;
}

const char* history_prev(void) {
    if (history_count == 0) return "";
    if (current_history_index > 0) {
        current_history_index--;
    }
    return history[current_history_index];
}

const char* history_next(void) {
    if (history_count == 0) return "";
    if (current_history_index < history_count - 1) {
        current_history_index++;
    } else {
        current_history_index = history_count;
        return "";
    }
    return history[current_history_index];
}