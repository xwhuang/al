#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>		/* Required for time(). */
#include "print_color_control.h"

/**
 * @population     样本总数
 * @length         个体算子总数
 * @crossover_rate 杂交率
 * @mutation_rate  变异率
 * @max_generation 迭代次数上限,0表示无限迭代
 * @max_factor     商因子;如果max_generation为0,则迭代终止条件为迭代发现同商个体次数达
 *                 max_factor * population
 * @sum_target     个体算子求和的目标值, 个体特征之一
 * @product_target 个体算子求积的目标值, 个体特征之一
 * @pool           样本空间
 * @max_error      最大误差值
 */
struct ga {
    int     population; 
    int     length; 
    float   crossover_rate;
    float   mutation_rate; 
    int     max_generation; 
    int     max_factor;
    int     sum_target; 
    int     product_target; 
    int   **pool;  
    float   max_error; 
};

//copy from BIND9 random.c
static void initialize_rand(void)
{
	unsigned int pid = getpid();

	/*
	 * The low bits of pid generally change faster.
	 * Xor them with the high bits of time which change slowly.
	 */
	pid = ((pid << 16) & 0xffff0000) | ((pid >> 16) & 0xffff);

	srand(time(NULL) ^ pid);    
}

struct ga* ga_init(int population,
                   int length,
                   float crossover_rate,
                   float mutation_rate,
                   int max_generation,
                   int max_factor,
                   int sum_target,
                   int product_target,
                   float max_error)
{
    struct ga *ga0 = NULL;
    int i = 0, j = 0;

    ga0 = (struct ga*)malloc(sizeof(struct ga));
    if (ga0 == NULL) return ga0;

    ga0->population     = population;
    ga0->length         = length;
    ga0->crossover_rate = crossover_rate;
    ga0->mutation_rate  = mutation_rate;
    ga0->max_generation = max_generation;
    ga0->max_factor     = max_factor;
    ga0->sum_target     = sum_target;
    ga0->product_target = product_target;    
    ga0->max_error      = max_error;

    ga0->pool = (int**)malloc(sizeof(int *) * ga0->population);
    if (ga0->pool == NULL) {
        free(ga0);
        return NULL;
    }
    for (i = 0; i < ga0->population; i++) {
        ga0->pool[i] = (int *)malloc(sizeof(int) * ga0->length);
        if (ga0->pool[i] == NULL) {
            int j = i - 1;
            for (j = i - 1; j >= 0; j--) {
                free(ga0->pool[j]);
            }
            free(ga0->pool);
            free(ga0);
            return NULL;
        }
    }
#ifdef USE_RANDOM_SEED 
    initialize_rand();
#endif
    for (i = 0; i < ga0->population; i++) {
        for (j = 0; j < ga0->length; j++) {
            ga0->pool[i][j] = rand() % 2;
        }
    }

    return ga0;
}

static void ga_finit(struct ga *ga0)
{
    int i = 0, j = 0;
    if (ga0 == NULL) return;
    if (ga0->pool != NULL) {
        for (i = 0; i < ga0->population; i++) {
            if (ga0->pool[i] != NULL) {
                free(ga0->pool[i]);
                ga0->pool[i] = NULL;
            }
        }
        free(ga0->pool);
        ga0->pool = NULL;
    }
    free(ga0);
    ga0 = NULL;
}

static void debug_show_ga(struct ga *ga0)
{
    int i = 0, j = 0;
    if (ga0 == NULL) return;

    printf("population     = %d\n", ga0->population);
    printf("length         = %d\n", ga0->length);
    printf("crossover_rate = %f\n", ga0->crossover_rate);
    printf("mutation_rate  = %f\n", ga0->mutation_rate);
    printf("max_generation = %d\n", ga0->max_generation);
    printf("max_factor     = %d\n", ga0->max_factor);
    printf("sum_target     = %d\n", ga0->sum_target);
    printf("product_target = %d\n", ga0->product_target);    
    printf("max_error      = %f\n", ga0->max_error);
#if 0
    for (i = 0; i < ga0->population; i++) {
        for (j = 0; j < ga0->length; j++) {
            printf("%d ", ga0->pool[i][j]);
        }
        printf("\n");
    }
#endif
}

/**
 * @function 计算指定样本个体的商,商越小表明越接近目标
 *
 * @param ga0 @see struct ga
 * @param n   样本个体
 * 
 * @return 返回个体的商, 0.0f表示完全匹配
 */
static float ga_evaluate(struct ga *ga0, int n) 
{    
    int sum = 0, prod = 1;
    int i = 0, j = 0;
    float ret = 0.0f;

    if (ga0 == NULL || n >= ga0->population || n < 0) {
        return ret;
    }

    for(i = 0; i < ga0->length; i++) {
        if (ga0->pool[n][i]==0) {
            sum += (1+i);
        } else {
            prod *= (1+i);
        }        
    }
    ret = (float)abs(sum - ga0->sum_target)/ga0->sum_target + 
          (float)abs(prod - ga0->product_target)/ga0->product_target;
    
    return ret;
}

/**
 * @function 返回一个[0, max-1]内的随机整数
 * 
 */
static int ga_rand(int max)
{
    return (rand()%max);
}

/**
 * @function 返回一个[0, 1.0f]内的随机浮点数
 * 
 */
static float ga_randf(void)
{
    int base = 4087;
    int x = rand();
    return (float)(x%base)/(float)base;
}

/**
 * @function 将两个算子杂交,w保持不变,l变
 * 
 */
static void ga_crossover(struct ga* ga0, int w, int l)
{
    float x = 0.0f;
    int i = 0;

    for (i = 0; i < ga0->length; i++) {
        x = ga_randf();
        if (x < ga0->crossover_rate) {
            ga0->pool[l][i] = ga0->pool[w][i];
        }
    }
}

/**
 * @function 算子变异
 * 
 */
static void ga_mutation(struct ga* ga0, int l)
{
    float x = 0.0f;
    int i = 0;

    for (i = 0; i < ga0->length; i++) {
        x = ga_randf();
        if (x < ga0->mutation_rate) {
            ga0->pool[l][i] = (ga0->pool[l][i] + 1)%2;
        }
    }
}

static void ga_run(struct ga *ga0)
{
    int sum = 0, prod = 1;
    int winner = 0, loser = 0;
    int g = 0;
    float error = 0.0f;
    float lerr = 0.0f;
    int i = 0;
    int normalization_cnt = 0;

    while (ga0->max_generation == 0 || g < ga0->max_generation) {
        winner = ga_rand(ga0->population);
        loser  = ga_rand(ga0->population);
        if (ga_evaluate(ga0, winner) > ga_evaluate(ga0, loser)) {
            loser = winner + loser;
            winner = loser - winner;
            loser = loser - winner;
        }
        if ((error = ga_evaluate(ga0, winner)) <= ga0->max_error) {
            break;
        }
        lerr = error;
        ga_crossover(ga0, winner, loser);
        ga_mutation(ga0, loser);
        if ((error = ga_evaluate(ga0, loser)) <= ga0->max_error) {
            winner = loser;
            break;
        }
        if (lerr == error) {
            normalization_cnt++;
#if 0            
            printf("\ngeneration:%d, w = %d, l = %d\n", g, winner, loser);
            printf("    winner: ");
            for (i = 0; i < ga0->length; i++) {
                printf("%02d ", ga0->pool[winner][i]);
            }
            printf("\n");
            printf("     loser: ");
            for (i = 0; i < ga0->length; i++) {
                printf("%02d ", ga0->pool[loser][i]);
            }
            printf("\n\n");
#endif
            if (ga0->max_generation == 0 &&
                normalization_cnt > ga0->population * ga0->max_factor) {
                printf("Failed, Quotient stop.Do %d generations.\n", g);
                return;
            }
        }
        lerr = error;
        g++;
    }

    if (g == ga0->max_generation) {
        printf("Failed, Do %d generations.\n", g);
    } else if (winner < ga0->population) {
        printf("Found at \x1B[36m%dth\x1B[0m generation.\n", g);
        int i = 0;
        for (i = 0; i < ga0->length; i++) {
            if (ga0->pool[winner][i]==0) {                
                sum += (1+i);
                printf("%s%d%s ", KBLU, (i+1), KNRM);
            } else {
                prod *= (1+i);
                printf("%s%d%s ", KRED, (i+1), KNRM);
            }
        }
        printf("\n");
        printf("%s  error:%f\n", KMAG,error);
        printf("%s    sum:%d/%d\n%s",KBLU, sum, ga0->sum_target, KNRM);
        printf("%sproduct:%d/%d\n%s",KRED, prod, ga0->product_target, KNRM);
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    struct ga *ga0;
    
    if ((ga0 = ga_init(100, 15, 0.5f, 0.01f, 0, 10000, 75, 14850, 0.00001f)) != NULL) {
        printf("\nGA Info:\n");
        printf("------------------------------\n");
        color_set_cyn();
        debug_show_ga(ga0);
        color_set_nrm();
        printf("\nGA Result:\n");
        printf("------------------------------\n");
        ga_run(ga0);
        ga_finit(ga0);
    }

    return 0;
}
