#ifndef __PRINT_COLOR_CONTROL_H__
#define __PRINT_COLOR_CONTROL_H__

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define printHelp(cmd, des) do {                                        \
        printf("%s%s\n    %s%s%s\n\n", KBLU, (cmd), KGRN, (des), KNRM); \
    } while(0)

#define printRed(s) do {                        \
        printf("%s%s%s\n", KRED, (s), KNRM);    \
    }while (0)

#define printGrn(s) do {                        \
        printf("%s%s%s\n", KGRN, (s), KNRM);    \
    }while (0)

#define printYel(s) do {                        \
        printf("%s%s%s\n", KYEL, (s), KNRM);    \
    }while (0)

#define printBlu(s) do {                        \
        printf("%s%s%s\n", KBLU, (s), KNRM);    \
    }while (0)

#define printMag(s) do {                        \
        printf("%s%s%s\n", KMAG, (s), KNRM);    \
    }while (0)

#define printCyn(s) do {                        \
        printf("%s%s%s\n", KCYN, (s), KNRM);    \
    }while (0)

#define printWht(s) do {                        \
        printf("%s%s%s\n", KWHT, (s), KNRM);    \
    }while (0)

static inline void color_set_nrm()
{
    printf("%s", KNRM);
}

static inline void color_set_red()
{
    printf("%s", KRED);
}

static inline void color_set_grn()
{
    printf("%s", KGRN);
}

static inline void color_set_yel()
{
    printf("%s", KYEL);
}

static inline void color_set_blu()
{
    printf("%s", KBLU);
}

static inline void color_set_mag()
{
    printf("%s", KMAG);
}

static inline void color_set_cyn()
{
    printf("%s", KCYN);
}

static inline void color_set_wht()
{
    printf("%s", KWHT);
}

#endif  /**< end print_color_control.h */
