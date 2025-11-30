#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

#define MODE_SENSE 36  // port to sense whether RPi-Zero is powered on or not
boolean rpiMode;       // RPi mode / ESP32 stand alone mode

//////////////////
// key definition
//////////////////

#define KEY_REPEAT 200
#define KEY_AVOID_CHATTERING 20

#define K_OUT_N 9  // number of key scan output port
#define K_IN_N 10  // number of key scan input port

int KeyPinOut[K_OUT_N] = { 10, 9, 46, 3, 8, 18, 17, 16, 15 };   // list of output ports
int KeyPinIn[K_IN_N] = { 14, 13, 12, 11, 7, 6, 5, 4, 38, 37 };  // list of input ports
#define SHIFT_ROW 0                                             // SHIFT key
#define SHIFT_CLM 0
#define CAPS_ROW 0  // CAPS key
#define CAPS_CLM 2
#define CTRL_ROW 1  // BASIC key
#define CTRL_CLM 0
#define HIS_UP_ROW 5  // CONST key
#define HIS_UP_CLM 2
#define HIS_DN_ROW 4  // ANS key
#define HIS_DN_CLM 2

// key matrix : clm, row
char keyMat[10][9] = {
  { 0, 0, 0, 0x1b, '\t', 0, ' ', 'B' + 0x80, 'A' + 0x80 },  // kana = ESC, cursor ESC[B, ESC[A
  { 0, 0, 'z', 'x', 'c', 'v', 'b', 'n', 'm' },
  { 0, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j' },
  { 3, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i' },             // 3 : ctrl-C
  { 'D' + 0x80, 'C' + 0x80, 0, '0', '.', 0, '+', '\n', 0 },  // cursor ESC[D, ESC[C
  { ',', ';', 0, '1', '2', '3', '-', 0, 0 },
  { 'k', 'l', 0, '4', '5', '6', '*', 0, 0 },
  { 'o', 'p', 0x7F, '7', '8', '9', '/', 0, 0 },  // 8 : backspace
  { 0, 0, 0, 0, 0, 0, '(', ')', 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

char keyMatCaps[10][9] = {
  { 0, 0, 0, 0, '\t', 0, ' ', 0, 0 },
  { 0, 0, 'Z', 'X', 'C', 'V', 'B', 'N', 'M' },
  { 0, 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J' },
  { 0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I' },
  { 0, 0, 0, '0', '.', 0, '+', '\n', 0 },
  { '?', ':', 0, '1', '2', '3', '-', 0, 0 },
  { 'K', 'L', 0, '4', '5', '6', '*', 0, 0 },
  { 'O', 'P', 0, '7', '8', '9', '/', 0, 0 },
  { 0, 0, 0, 0, 0, 0, '(', ')', 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

char keyMatShift[10][9] = {
  { 0, 0, 0, '_', '\t', 0, ' ', 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, '[', ']', '{', '}', '\\', '|', '~' },
  { 0, '!', '\"', '#', '$', '%', '&', '\'', '<' },
  { 0, 0, 0, '0', '.', 0, '+', '\n', 0 },
  { '?', ':', 0, '1', '2', '3', '-', 0, 0 },
  { '_', '=', 0, '4', '5', '6', '*', 0, 0 },
  { '>', '@', 0, '7', '8', '9', '/', 0, 0 },
  { 0, 0, 0, 0, 0, 0, '(', ')', 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

struct KeyPos {
  int row;
  int clm;
  boolean caps;
  boolean shift;
  boolean ctrl;
};

//////////////////
// display definition
//////////////////

// Define your custom SPI pins
#define MY_SCK 1
#define MY_MISO 39
#define MY_MOSI 2
#define MY_SS 40  // Chip Select for other SPI devices, if any

U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/40, /* dc=*/42, /* reset=*/41);  // Enable U8G2_16BIT in u8g2.h
#define DISP_WIDTH 256
#define DISP_HEIGHT 64
//#define FONT_TERM u8g2_font_mozart_nbp_tr
//#define FONT_TERM u8g2_font_6x10_mf
#define FONT_TERM u8g2_font_b12_t_japanese3  // with kanji
#define FONT_CALC u8g2_font_6x13_mf
#define FONT_TERM_HEIGHT 9
#define FONT_CALC_HEIGHT 12
#define FONT_WIDTH 6
int fontHeight = FONT_TERM_HEIGHT;

#define TEXT_WIDTH (DISP_WIDTH / FONT_WIDTH)
#define TEXT_HEIGHT (DISP_HEIGHT / fontHeight)

#define HISTORY_LINES 100  // rows of history area

//////////////////
// keyboard funcs
//////////////////

struct KeyPos keyCheck(void) {
  struct KeyPos key = { -1, -1, false, false, false };

  for (int o = 0; o < K_OUT_N; o++) {
    pinMode(KeyPinOut[o], OUTPUT);
    digitalWrite(KeyPinOut[o], LOW);
    for (int i = 0; i < K_IN_N; i++) {
      if (digitalRead(KeyPinIn[i]) == LOW) {
        if (i == CAPS_ROW && o == CAPS_CLM) {
          key.caps = true;
        } else if (i == SHIFT_ROW && o == SHIFT_CLM) {
          key.shift = true;
        } else if (i == CTRL_ROW && o == CTRL_CLM) {
          key.ctrl = true;
        } else {
          key.row = i;
          key.clm = o;
        }
      }
    }
    pinMode(KeyPinOut[o], INPUT);
  }
  return key;
}

char keyChar(struct KeyPos key) {
  static unsigned long time;
  static char c, c2;

  if (key.row < 0) {  // key release
    time = millis() - KEY_REPEAT + KEY_AVOID_CHATTERING;
    return 0;
  }
  if (key.caps)
    c = keyMatCaps[key.row][key.clm];
  else if (key.shift)
    c = keyMatShift[key.row][key.clm];
  else if (key.ctrl)
    c = keyMat[key.row][key.clm] & 0x1f;
  else
    c = keyMat[key.row][key.clm];

  if (c == c2 && time + KEY_REPEAT > millis()) {  // avoid chattering / repeat
    return 0;
  }
  time = millis();
  c2 = c;

  return c;
}

void waitRelease(void) {
  struct KeyPos keyRaw;
  do {
    keyRaw = keyCheck();
  } while (keyRaw.row >= 0);
}

//////////////////
// display funcs
//////////////////

char historyBuf[HISTORY_LINES][TEXT_WIDTH + 1];
char (*textBuf)[TEXT_WIDTH + 1] = &(historyBuf[HISTORY_LINES - TEXT_HEIGHT]);
int cursorX, cursorY;
boolean cursorOff;
int dispLine = HISTORY_LINES - TEXT_HEIGHT;
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void resetDisplaySetting(void) {
  textBuf = &(historyBuf[HISTORY_LINES - TEXT_HEIGHT]);
  dispLine = HISTORY_LINES - TEXT_HEIGHT;
  cursorX = 0;
  cursorY = 0;
  cursorOff = false;
}

void turnOffDisplay(void) {
  u8g2.clearBuffer();
  u8g2.sendBuffer();  // transfer internal memory to the display
}

void refleshDisplay(void) {
  char c2[2] = " ";

  char(*showBuf)[TEXT_WIDTH + 1] = &(historyBuf[dispLine]);
  u8g2.clearBuffer();
  for (int j = 0; j < TEXT_HEIGHT; j++) {
    showBuf[j][TEXT_WIDTH] = '\0';
    // u8g2.drawStr(0, (j + 1) * fontHeight - 1, showBuf[j]);
    u8g2.setCursor(0, (j + 1) * fontHeight - 1);
    u8g2.print(showBuf[j]);  // ready for UTF8
  }

  if (!cursorOff && dispLine == HISTORY_LINES - TEXT_HEIGHT) {  // show cursor
    u8g2.setDrawColor(2);                                       // xor
    u8g2.drawBox(cursorX * FONT_WIDTH, cursorY * fontHeight,
                 FONT_WIDTH, fontHeight);
    u8g2.setDrawColor(1);
  }

  u8g2.sendBuffer();  // transfer internal memory to the display
}

void showString(const char *s) {
  for (int i = 0; s[i] != '\0'; i++) {
    showChar(s[i]);
  }
}

void scrollUp(void) {  // scroll up including history area
  for (int i = 0; i < HISTORY_LINES - 1; i++) {
    for (int j = 0; j < TEXT_WIDTH; j++) {
      historyBuf[i][j] = historyBuf[i + 1][j];
    }
  }
  for (int j = 0; j < TEXT_WIDTH; j++) {
    historyBuf[HISTORY_LINES - 1][j] = ' ';
  }
}

void scrollDown(void) {  // scroll down of visible area only
  for (int i = TEXT_HEIGHT - 1; i > 0; i--) {
    for (int j = 0; j < TEXT_WIDTH; j++) {
      textBuf[i - 1][j] = textBuf[i][j];
    }
  }
  for (int j = 0; j < TEXT_WIDTH; j++) {
    textBuf[0][j] = ' ';
  }
}

void eraseLine(int mode) {  // 0 : erase right, 1 : erase left, 2 : erase whole line
  int start = 0;
  int end = TEXT_WIDTH;

  switch (mode) {
    case 0:
      start = cursorX;
      break;
    case 1:
      end = cursorX;
      break;
  }
  for (int j = start; j < end; j++) {
    textBuf[cursorY][j] = ' ';
  }
}

void eraceLines(int mode) {
  int start = 0;
  int end = TEXT_HEIGHT;

  switch (mode) {
    case 0:
      start = cursorY + 1;
      break;
    case 1:
      end = cursorY;
      break;
  }
  for (int i = start; i < end; i++) {
    for (int j = 0; j < TEXT_WIDTH; j++) {
      textBuf[i][j] = ' ';
    }
  }
  eraseLine(mode);
}

void escProcess(String str) {  // process escape sequence
  int end = str.charAt(str.length() - 1);
  str = str.substring(0, str.length() - 1);
  if (str.charAt(0) == '[') {  // CSI (Control Sequence Introducer)
    str = str.substring(1);
    int num = str.toInt();
    if (str.length() == 0)
      num = 1;
    switch (end) {
      case 'A':
        cursorY = MAX(cursorY - num, 0);
        break;
      case 'B':
        cursorY = MIN(cursorY + num, TEXT_HEIGHT - 1);
        break;
      case 'C':
        cursorX = MIN(cursorX + num, TEXT_WIDTH - 1);
        break;
      case 'D':
        cursorX = MAX(cursorX - num, 0);
        break;
      case 'E':
        cursorX = 0;
        cursorY = MIN(cursorY + num, TEXT_HEIGHT - 1);
        break;
      case 'F':
        cursorX = 0;
        cursorY = MAX(cursorY - num, 0);
        break;
      case 'G':
        cursorX = MAX(MIN(num - 1, TEXT_WIDTH - 1), 0);
        break;
      case 'J':
        eraceLines(str.toInt());
        break;
      case 'K':
        eraseLine(str.toInt());
        break;
      case 'H':
      case 'f':
        if (0 <= str.indexOf(';')) {
          cursorY = str.toInt() - 1;
          cursorX = str.substring(str.indexOf(';') + 1).toInt() - 1;
          cursorX = MAX(MIN(cursorX, TEXT_WIDTH - 1), 0);
          cursorY = MAX(MIN(cursorY, TEXT_HEIGHT - 1), 0);
        } else {
          cursorX = 0;
          cursorY = 0;
        }
        break;
      case 'S':
        for (int n = 0; n < num; n++) {
          scrollUp();
        }
        break;
      case 'T':
        for (int n = 0; n < num; n++) {
          scrollDown();
        }
        break;
      case '@':  // insert space (scroll right)
        for (int j = TEXT_WIDTH - num - 1; j >= cursorX; j--) {
          textBuf[cursorY][j + num] = textBuf[cursorY][j];
        }
        for (int j = cursorX; j < cursorX + num; j++) {
          if (j >= TEXT_WIDTH)
            break;
          textBuf[cursorY][j] = ' ';
        }
        break;
      case 'P':  // delete charactor (scroll left)
        for (int j = cursorX; j < TEXT_WIDTH; j++) {
          textBuf[cursorY][j] = (j + num < TEXT_WIDTH) ? textBuf[cursorY][j + num] : ' ';
        }
        break;
      case 'h':
        if (str == "?25")
          cursorOff = false;
        break;
      case 'l':
        if (str == "?25")
          cursorOff = true;
        break;
    }
  }
}

#define PROCESS_ESC 1
#define DEBUG_ESC 0

void showChar(char c) {
  static boolean esc;
  static String escStr;

  if (!c)
    return;

#if DEBUG_ESC
  char c2[2] = " ";
  c2[0] = c;
  Serial.print(c2);
#endif

#if PROCESS_ESC
  if (esc) {
    char c2[2] = " ";
    c2[0] = c;
    escStr += c2;
    if (c != '[' && (0x40 <= c && c <= 0x7f)) {  // escape sequence end
      esc = false;
      escProcess(escStr);
    }
    return;
  }
  if (c == 0x1b) {  // escape sequence start
    esc = true;
    escStr = "";
    return;
  }

  if (0x0A <= c && c <= 0x0C) {  // LF / VT / FF
    cursorX = 0;
    cursorY++;
  } else if (c == 0x0D) {
    cursorX = 0;
  } else if (c == 0x08 || c == 0x7F) {  // backspace
    cursorX = MAX(cursorX - 1, 0);
  } else if (c == 0x09) {  // tab, HT
    cursorX = cursorX + 8;
    cursorX -= cursorX % 8;
  } else {
#else  // show special characters
  {
    if (c < 0x20)
      c += 0xc0;
#endif
    if (c >= 0x20) {
      textBuf[cursorY][cursorX] = c;
      cursorX++;
    }
  }
  if (cursorX >= TEXT_WIDTH) {
    cursorX = 0;
    cursorY++;
  }
  if (cursorY >= TEXT_HEIGHT) {
    scrollUp();
    cursorY--;
  }
}

//////////////////
// RPi mode VT100 console main loop
//////////////////

void loopRpi() {
  struct KeyPos key;
  char c;
  char c2[2] = " ";
  boolean dispFlag = false;
  static unsigned long lastExec = millis(); // sleep timer
  static boolean dispOff = false;

  while (Serial2.available()) {
    c = Serial2.read();
    showChar(c);
    dispFlag = true;
  }
  if (dispFlag) {
    refleshDisplay();
    dispOff = false;
    lastExec = millis();
  }

  key = keyCheck();
  if (key.row == HIS_UP_ROW && key.clm == HIS_UP_CLM) {
    dispLine = MAX(dispLine - 1, 0);
    refleshDisplay();
    delay(50);
    return;
  } else if (key.row == HIS_DN_ROW && key.clm == HIS_DN_CLM) {
    dispLine = MIN(dispLine + 1, HISTORY_LINES - TEXT_HEIGHT);
    refleshDisplay();
    delay(50);
    return;
  } else if (key.row == 9 && key.clm == 8) {  // CLS key
    char c3[100];
    sprintf(c3, "stty rows %d cols %d; clear", TEXT_HEIGHT, TEXT_WIDTH - 1);
    Serial2.print(c3);
    waitRelease();
  }
  c = keyChar(key);
  if (c == 0) {
    if(lastExec + 10000 < millis()) { // no operation for 10 sec
      if (lastExec + 30000 < millis()) {  // for 30sec, turn off the display
        if(!dispOff) {
          turnOffDisplay();
          dispOff = true;
        }
      }
      esp_sleep_enable_timer_wakeup(300000);  // 0.3sec
      esp_light_sleep_start();
    }
    return;
  }
  if(dispOff) { // display turn on by hitting any key
    refleshDisplay();
    dispOff = false;
  }
  dispLine = HISTORY_LINES - TEXT_HEIGHT;  // reset display area
  if (c & 0x80) {                          // escape sequence
    char c3[4] = { 0x1b, '[', ' ', 0 };
    c3[2] = c & 0x7f;
    Serial2.print(c3);
  } else {
    c2[0] = c;
    Serial2.print(c2);
  }
  lastExec = millis();
}

//////////////////
// ESP32 mode calculator
//////////////////

#define MAX_DIGIT 19
#define NUM_WIDTH 17

#define STACK_DEPTH 32
double stack[STACK_DEPTH];

void push(double v) {
  for (int i = STACK_DEPTH - 1; i > 0; i--) {
    stack[i] = stack[i - 1];
  }
  stack[0] = v;
}

double pop() {
  double v;
  v = stack[0];
  for (int i = 0; i < STACK_DEPTH - 1; i++) {
    stack[i] = stack[i + 1];
  }
  stack[STACK_DEPTH - 1] = 0;
  return v;
}

#define UNDO_DEPTH 32
int undoTop = 0;
double undoBuf[UNDO_DEPTH][STACK_DEPTH];

void backup(void) {
  if (undoTop >= UNDO_DEPTH) {
    for (int i = 0; i < UNDO_DEPTH - 1; i++) {
      for (int j = 0; j < STACK_DEPTH; j++) {
        undoBuf[i][j] = undoBuf[i + 1][j];
      }
    }
    undoTop--;
  }
  for (int j = 0; j < STACK_DEPTH; j++) {
    undoBuf[undoTop][j] = stack[j];
  }
  undoTop++;
}

void restore(void) {
  if (undoTop == 0)  // no undo data
    return;
  undoTop--;
  for (int j = 0; j < STACK_DEPTH; j++) {
    stack[j] = undoBuf[undoTop][j];
  }
}

void messageBox(String str) {
  int width = str.length() * FONT_WIDTH;
  width = width / 2 + 10;

  u8g2.setDrawColor(1);  // xor
  u8g2.drawBox(DISP_WIDTH / 2 - width - 1, DISP_HEIGHT / 2 - fontHeight - 1,
               width * 2 + 2, fontHeight * 2 + 2);
  u8g2.setDrawColor(0);  // xor
  u8g2.drawBox(DISP_WIDTH / 2 - width, DISP_HEIGHT / 2 - fontHeight,
               width * 2, fontHeight * 2);

  u8g2.setCursor(DISP_WIDTH / 2 - width + 10, DISP_HEIGHT / 2 + fontHeight / 2 - 1);
  u8g2.setDrawColor(1);
  u8g2.print(str);
  u8g2.sendBuffer();  // transfer internal memory to the display
}

void dispClear(void) {
  for (int i = 0; i < TEXT_HEIGHT; i++) {
    for (int j = 0; j < TEXT_WIDTH; j++) {
      textBuf[i][j] = ' ';
    }
  }
}

#define VARIABLE_NUM 3
#define VARIABLE_LETTER 'X'
double memory[VARIABLE_NUM];  // variable memory X, Y, Z

boolean degree = false;  // radian / degree
String enteringStr = "";

void update_display(void) {
  int entlen = enteringStr.length();

  resetDisplaySetting();
  dispClear();
  cursorX = MAX_DIGIT + 3;
  cursorY = 0;
  showString(degree ? "degree" : "radian");

  for (int i = 0; i < VARIABLE_NUM; i++) {  // show content of variable numbers
    if (memory[i] != 0) {
      String s = " ";
      s[0] = VARIABLE_LETTER + i;
      s = s + fp64_to_string_wrap(memory[i]);
      cursorX = MAX_DIGIT + 3;
      cursorY = i + 1;
      showString(s.c_str());
    }
  }
  for (int i = 0; i < TEXT_HEIGHT; i++) {
    String s = " ";
    s[0] = 'F' - i;
    if (s[0] < 'A')
      s[0] = ' ';
    s = s + fp64_to_string_wrap(stack[i]);
    cursorX = 0;
    cursorY = TEXT_HEIGHT - 1 - i;
    showString(s.c_str());
  }
  String dispEntStr;
  if (entlen > NUM_WIDTH - 2) {
    dispEntStr = ".." + enteringStr.substring(entlen - (NUM_WIDTH - 2));
  } else {
    dispEntStr = ">" + enteringStr;
    dispEntStr = dispEntStr.substring(0, NUM_WIDTH);
  }
  cursorX = MAX_DIGIT + 3;
  cursorY = TEXT_HEIGHT - 1;
  showString(dispEntStr.c_str());
  refleshDisplay();
}

String fp64_to_string_wrap(double n) {
  char str[30];
  String s;

  sprintf(str, "%15.13g", n);

  s = str;
  while (s.length() < MAX_DIGIT) {
    s = " " + s;  // align right
  }
  return s;
}

double toRad(double x) {
  if (degree) {
    return x * M_PI / 180.0;
  } else return x;
}

double fromRad(double x) {
  if (degree) {
    return x * 180.0 / M_PI;
  } else return x;
}

void enter(void) {
  if (enteringStr.length()) {
    backup();
    push(atof((char *)enteringStr.c_str()));
    enteringStr = "";
  }
}

void loopESP32() {
  double x, y;
  struct KeyPos keyRaw;
  char key;
  boolean dispFlag = false;
  boolean shift = false;
  static unsigned long lastExec = millis(); // sleep timer

  keyRaw = keyCheck();
  if (keyRaw.shift) {  // shift key is regarded caps in calc mode
    keyRaw.shift = false;
    keyRaw.caps = true;
  }
  key = keyChar(keyRaw);

  if (keyRaw.row == 4 && keyRaw.clm == 5) {  // Exp key for 1.23E10 style numbers
    key = '@';                               // instead of 'E', use '@' temporarily
  }
  if (keyRaw.row == 8) {
    switch (keyRaw.clm) {
      case 0:  // pi
        enter();
        backup();
        push(M_PI);
        break;
      case 1:  // sqrt
        enter();
        backup();
        if (shift) {
          push(pow(pop(), 1 / 3.0));
        } else {
          push(sqrt(pop()));
        }
        break;
      case 2:  // deg
        degree = !degree;
        messageBox(degree ? "degree mode" : "radian mode");
        delay(1000);
        update_display();
        break;
      case 3:  // x^2
        enter();
        backup();
        x = pop();
        if (shift) {
          push(x * x * x);
        } else {
          push(x * x);
        }
        break;
      case 4:  // y^x
        enter();
        backup();
        x = pop();
        y = pop();
        push(pow(y, x));
        break;
      case 5:  // log
        enter();
        backup();
        if (shift)
          push(pow(10, pop()));
        else
          push(log10(pop()));
        break;
    }
    lastExec = millis();
    update_display();
    waitRelease();
  } else if (keyRaw.row == 9) {
    switch (keyRaw.clm) {
      case 0:  // npr
        break;
      case 1:  // 2nd-f
        break;
      case 2:  // sin
        enter();
        backup();
        if (shift)
          push(fromRad(asin(pop())));
        else
          push(sin(toRad(pop())));
        break;
      case 3:  // ln
        enter();
        backup();
        if (shift)
          push(exp(pop()));
        else
          push(log(pop()));
        break;
      case 4:  // cos
        enter();
        backup();
        if (shift)
          push(fromRad(acos(pop())));
        else
          push(cos(toRad(pop())));
        break;
      case 5:  // tan
        enter();
        backup();
        if (shift)
          push(fromRad(atan(pop())));
        else
          push(tan(toRad(pop())));
        break;
      case 6:  // 1/x
        enter();
        backup();
        push(1.0 / pop());
        break;
      case 7:  // F<>E : swap
        enter();
        backup();
        x = pop();
        y = pop();
        push(x);
        push(y);
        break;
      case 8:  // CLS : drop
        enter();
        backup();
        pop();
        break;
    }
    lastExec = millis();
    update_display();
    waitRelease();
  } else {
    if (key == 0) {                                                               // no key press : sleep
      if ((keyRaw.row == 2 && keyRaw.clm == 0) || lastExec + 30000 < millis()) {  // OFF key or 30sec untouched
        turnOffDisplay();
        esp_sleep_enable_timer_wakeup(300000);  // 0.3sec
        for (;;) {
          esp_light_sleep_start();
          keyRaw = keyCheck();
          if (keyRaw.row >= 0 || keyRaw.shift || keyRaw.caps || keyRaw.ctrl)  // any key
            break;
        }
        lastExec = millis();
        cursorOff = false;
        update_display();                        // display turn on
      } else if (lastExec + 10000 < millis()) {  // untouch for 10sec
        esp_sleep_enable_timer_wakeup(50000);  // 50msec nap
        esp_light_sleep_start();
      }
      return;
    }
    switch (key) {
      case '\n':  // enter
        if (enteringStr.length()) {
          enter();
        } else {  // DUP
          backup();
          x = pop();
          push(x);
          push(x);
        }
        break;
      case '+':
        enter();
        backup();
        x = pop();
        y = pop();
        push(y + x);
        break;
      case '*':
        enter();
        backup();
        x = pop();
        y = pop();
        push(y * x);
        break;
      case '-':
        if (enteringStr.indexOf("E") != -1) {  // entering 1.23E-10 style number
          enteringStr.concat(key);
          break;
        }
        enter();
        backup();
        if (shift) {
          x = pop();
          push(-x);
        } else {
          x = pop();
          y = pop();
          push(y - x);
        }
        break;
      case '/':
        enter();
        backup();
        x = pop();
        y = pop();
        push(y / x);
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        enteringStr.concat(key);
        break;
      case '.':
        if (enteringStr.indexOf(".") == -1 && enteringStr.indexOf("E") == -1) {
          enteringStr.concat(key);
        }
        break;
      case '@':
        if (enteringStr.indexOf("E") == -1) {
          enteringStr.concat('E');
        }
        break;
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
        enter();
        backup();
        push(stack['f' - key]);  // copy from stack directly
        break;
      case 0x7F:  // backspace : delete one character
        if (enteringStr.length()) {
          enteringStr = enteringStr.substring(0, enteringStr.length() - 1);
        }
        break;
      case 'D' + 0x80:
        restore();  // undo by <= (left arrow)
        break;
      case 'X':
      case 'Y':
      case 'Z':
        memory[key - VARIABLE_LETTER] = stack[0];
        break;
      case 'x':
      case 'y':
      case 'z':
        backup();
        push(memory[key - 'a' + 'A' - VARIABLE_LETTER]);
        break;
    }
  }
  lastExec = millis();
  update_display();
}

//////////////////
// main section
//////////////////

void setup() {
  Serial2.begin(115200, SERIAL_8N1, 47, 48);
#if DEBUG_ESC
  Serial.begin(115200);
#endif

  SPI.begin(MY_SCK, MY_MISO, MY_MOSI, MY_SS);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFontMode(1);
  u8g2.setDrawColor(1);

  for (int i = 0; i < TEXT_HEIGHT; i++) {
    for (int j = 0; j < TEXT_WIDTH; j++) {
      textBuf[i][j] = ' ';
    }
  }

  for (int i = 0; i < K_OUT_N; i++) {
    pinMode(KeyPinOut[i], INPUT);
  }
  for (int i = 0; i < K_IN_N; i++) {
    pinMode(KeyPinIn[i], INPUT_PULLUP);
  }

  pinMode(MODE_SENSE, INPUT_PULLDOWN);
  delay(100);
  rpiMode = digitalRead(MODE_SENSE);
  if (rpiMode) {              // RPi powered on
    u8g2.setFont(FONT_TERM);  // choose a suitable font
    fontHeight = FONT_TERM_HEIGHT;
    resetDisplaySetting();
    messageBox("Raspberry Pi OS mode\n");
    delay(1000);
  } else {
    u8g2.setFont(FONT_CALC);  // choose a suitable font
    fontHeight = FONT_CALC_HEIGHT;
    resetDisplaySetting();
    update_display();
  }
}

void loop() {
  if (rpiMode) {
    loopRpi();
  } else {
    loopESP32();
  }
}
