#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_DIODES 4
#define DIODE_FIELDS 3

#define PERIOD_MAX 2000
#define PERIOD_MIN 200
#define PERIOD_DELTA 100

#define DUTY_MAX 100
#define DUTY_MIN 10
#define DUTY_DELTA 10

const char key_up[] = "\033[A";
const char key_down[] = "\033[B";
const char key_left[] = "\033[D";
const char key_right[] = "\033[C";
const char clear[] = "\033c";
const char green[] = "\033[32m";
const char reset[] = "\033[0m";

enum mode { idle, on, off, blink };
const char *mode_names[] = {"idle", "on", "off", "blink"};

struct diode_status {
  enum mode mode;
  unsigned int period;
  unsigned int duty_cicle;
};

struct diode_status diodes[NUM_DIODES];

void update_status_up(int selected_field) {
  int col = selected_field / DIODE_FIELDS;
  int row = selected_field % DIODE_FIELDS;

  struct diode_status *diode = &diodes[col];

  switch (row) {
  case 0:
    if (diode->mode != blink) {
      ++diode->mode;
    }
    break;
  case 1:
    if (diode->period != PERIOD_MAX) {
      diode->period += PERIOD_DELTA;
    }
    break;
  case 2:
    if (diode->duty_cicle != DUTY_MAX) {
      diode->duty_cicle += DUTY_DELTA;
    }
    break;
  };
  // TODO send update to MCU
}

void update_status_down(int selected_field) {
  int col = selected_field / DIODE_FIELDS;
  int row = selected_field % DIODE_FIELDS;

  struct diode_status *diode = &diodes[col];

  switch (row) {
  case 0:
    if (diode->mode != idle) {
      --diode->mode;
    }
    break;
  case 1:
    if (diode->period != PERIOD_MIN) {
      diode->period -= PERIOD_DELTA;
    }
    break;
  case 2:
    if (diode->duty_cicle != DUTY_MIN) {
      diode->duty_cicle -= DUTY_DELTA;
    }
    break;
  };
  // TODO send update to MCU
}

void print_diode_status(int selected_field) {
  for (int i = 0; i < NUM_DIODES; ++i) {
    int col = selected_field / DIODE_FIELDS;
    int row = selected_field % DIODE_FIELDS;
    printf("Diode%d:", i);

    if (col == i && row == 0) {
      printf(green);
    }
    printf("%8s%s", mode_names[diodes[i].mode], reset);

    if (col == i && row == 1) {
      printf(green);
    }
    printf("%6d[ms]%s", diodes[i].period, reset);

    if (col == i && row == 2) {
      printf(green);
    }
    printf("%6d[ms]%s\n", diodes[i].duty_cicle, reset);
  }
}

void clear_screen(void) {return; printf(clear); }

bool key_match(const char *str, const char *key) {
  return strncmp(str, key, strlen(key)) == 0;
}

void diode_setup(void) {
  for (int i = 0; i < NUM_DIODES; ++i) {
    diodes[i].mode = idle;
    diodes[i].period = PERIOD_MIN;
    diodes[i].duty_cicle = DUTY_MIN;
  }
}

int main(void) {
  char *buff;
  size_t buff_size = 0;
  int selected_field = 0;

  diode_setup();

  clear_screen();
  print_diode_status(selected_field);

  while (true) {
    printf(">> ");

    size_t len = getline(&buff, &buff_size, stdin);
    buff[len - 1] = '\0';

    for (char *c = buff; *c != '\0'; ++c) {
      if (key_match(c, key_up)) {
        update_status_up(selected_field);
      } else if (key_match(c, key_down)) {
        update_status_down(selected_field);
      } else if (key_match(c, key_left)) {
        if (selected_field > 0) {
          --selected_field;
        }
      } else if (key_match(c, key_right)) {
        if (selected_field < NUM_DIODES * DIODE_FIELDS - 1) {
          ++selected_field;
        }
      }
    }

    clear_screen();
    print_diode_status(selected_field);
  }

  free(buff);
  return 0;
}
