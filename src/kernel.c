typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;

static void write_char(char c);
static void write_str(char* str);
static void clear_scr(void);
static void color_scr(uint8_t color);
static void advance(void);

uint16_t* VGA = (uint16_t*) 0xB8000;

uint8_t row = 0;
uint8_t col = 0;

void kmain()
{
    color_scr(0x90);

    write_str(" __   ________ _   _  ____  _   _ \n");
    write_str(" \\ \\ / |  ____| \\ | |/ __ \\| \\ | |\n");
    write_str("  \\ V /| |__  |  \\| | |  | |  \\| |\n");
    write_str("   > < |  __| | . ` | |  | | . ` |\n");
    write_str("  / . \\| |____| |\\  | |__| | |\\  |\n");
    write_str(" /_/ \\_|______|_| \\_|\\____/|_| \\_|\n");

    while (1);
}

void write_char(char c)
{
    if (c == '\n')
    {
        row++;
        col = 0;
        return;
    }

    VGA[row * 80 + col] = (0x9F << 8) | c;
    advance();
}

void write_str(char* str)
{
    while (*str)
        write_char(*str++);
}

void clear_scr(void)
{
    color_scr(0x00);
}

void color_scr(uint8_t color)
{
    for (uint16_t i = 0; i < 80 * 25; i++)
        VGA[i] = color << 8;
}

void advance(void)
{
    col = (col + 1) % 80;
    row = (col == 0) ? row + 1 : row;
}