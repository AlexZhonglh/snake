#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<unistd.h>
#include "kbhit.c"

#define MAXSCORE 1000
#define LINE 32
#define COLUMN 62

typedef struct food
{
	bool if_food_exist;
	int food_pos_line;
	int food_pos_column;
} Food;

typedef struct node
{
	int line_pos;
	int column_pos;
} Node;
typedef struct head
{
	int head_line_pos;
	int head_column_pos;
	int head_move_direction; // 1234分别代表上下左右
}Head;
typedef struct game_snake
{
	Head snake_head;
	Node snake_nodes[MAXSCORE+2];
	int node_now;
} Snake;

void init_map_array(char (*map)[COLUMN], int line);
void print_game_view(char (*map)[COLUMN],int line);
// bool if_food(Food * pfood);
void food_make(Food * pfood, Snake * snake);
void char_make(Food * pfood, Snake * snake, char (*map)[COLUMN]);
bool if_food_aten(Snake * snake, Food * pfood);
bool if_die(Snake * snake);
void snake_head_move(Snake * snake);
void pre_head_pos_get(Snake * snake, int * a);
void snake_node_move(Snake * snake, int * a);
void move_contrl(Snake * snake);

void init_map_array(char (*map)[COLUMN], int line) //用于每一个视图初始化或。。
{
	for (int i =0; i < line; i++)
	{
		for (int j = 0; j < COLUMN; j++)
			map[i][j] = ' ';
	}
	for (int i = 0; i < COLUMN; i++)
	{
		map[0][i] = '*';
		map[line-1][i] = '*';
	}
	for (int i = 0; i < line; i++)
	{
		map[i][0] = '*';
		map[i][COLUMN-1] = '*';
	}
}

void print_game_view(char (*map)[COLUMN], int line) //用在刷新游戏视图后， 所有元素位置都已经确定
{
	for (int i =0; i < line; i++)
	{
		for (int j = 0; j < COLUMN; j++)
			printf("%c", map[i][j]);
		printf("\n");
	}
}

void move_contrl(Snake * snake)
{
	int ch;

	if (kbhit())
	{
		ch = getchar();
		switch(ch)
		{
			case 'w':
				if (snake->snake_head.head_move_direction != 2)
					snake->snake_head.head_move_direction = 1;
				break;
			case 's':
				if (snake->snake_head.head_move_direction != 1)
					snake->snake_head.head_move_direction = 2;
				break;
			case 'a':
				if (snake->snake_head.head_move_direction != 4)
					snake->snake_head.head_move_direction = 3;
				break;
			case 'd':
				if (snake->snake_head.head_move_direction != 3)
					snake->snake_head.head_move_direction = 4;
				break;
		}
	}
}
void init_game(Snake * snake, Food * pfood, int * a, char (*map)[COLUMN])
{
	snake->node_now = 2;
	snake->snake_head.head_column_pos = 3;
	snake->snake_head.head_line_pos = 10;
	snake->snake_head.head_move_direction = 4;
	snake->snake_nodes[0].line_pos = 10;
	snake->snake_nodes[1].line_pos = 10;
	snake->snake_nodes[0].column_pos = 2;
	snake->snake_nodes[1].column_pos = 1;
	pfood->if_food_exist = false;
	a[0] = 10;
	a[2] = 3;
	init_map_array(map, LINE);
}

int main(void)
{
	char map[LINE][COLUMN];
	Food * pfood = (Food *) malloc(sizeof(Food));
	int pre_head_pos[2];
	Snake * psnake = (Snake *) malloc(sizeof(Snake));
	char ans;
	
	puts("Press s to start or q to quit");
	while (scanf("%c", &ans) && ans == 's')
	{
		init_game(psnake, pfood, pre_head_pos, map);
		food_make(pfood, psnake);
		char_make(pfood,psnake, map);
		print_game_view(map, LINE);
		while (1)
		{
			move_contrl(psnake);
			pre_head_pos_get(psnake, pre_head_pos);
			snake_head_move(psnake);
			if (if_die(psnake))
				break;
			if (if_food_aten(psnake, pfood))
				food_make(pfood, psnake);
			snake_node_move(psnake, pre_head_pos);
			usleep(300000);
			system("clear");
			init_map_array(map, LINE);
			char_make(pfood, psnake, map);
			print_game_view(map, LINE);
			move_contrl(psnake);
		}
		system("clear");
		puts("game over");
		printf("Your score: %d\n", (psnake->node_now-2));
		puts("Press s continue or q to quit:");
	}

	return 0;
}

void food_make(Food * pfood, Snake * snake)   //生成食物的坐标  需要确定位置是否正确后方可更改数组
{
	 int flag = 1, l, c, i;

	 do 
	 {
		 srand(time(0));
		 l = rand()%(LINE-2)+1;
		 srand(time(0));
		 c = rand()%(COLUMN-2)+1;
		 if (l == snake->snake_head.head_line_pos && c == snake->snake_head.head_column_pos)
			continue;
		 else
		 {
		 	for (i = 0; i < snake->node_now; i++)
		 	{
				 if (l == snake->snake_nodes[i].line_pos && c == snake->snake_nodes[i].column_pos)
					 break;
			}
		 	if (i == snake->node_now)
			  flag = 0; //表示位置正确
		 }
	 } while (flag);
	 pfood->food_pos_line = l;
	 pfood->food_pos_column = c;
	 pfood->if_food_exist = true;
}

void char_make(Food * pfood, Snake * snake, char (*map)[COLUMN])
{
	int l, c;
	
	l = snake->snake_head.head_line_pos;
	c = snake->snake_head.head_column_pos;
	map[l][c] = '@';
	l = pfood->food_pos_line;
	c = pfood->food_pos_column;
	map[l][c] = 'O';
	for (int i = 0; i < snake->node_now; i++)
	{
		l = snake->snake_nodes[i].line_pos;
		c = snake->snake_nodes[i].column_pos;
		map[l][c] = '#';
	}
}

bool if_food_aten(Snake *snake, Food * pfood) //需要新的本视图的ｈｅａｄ位置打印视图前使用
{
	int l, c;
	
	if (!(pfood->if_food_exist))
		return true;
	l = snake->snake_head.head_line_pos;
	c = snake->snake_head.head_column_pos;
	if (l == pfood->food_pos_line && c == pfood->food_pos_column)
	{
		pfood->if_food_exist = false;
		snake->node_now++; //增加节数
		return true;
	}
	else
		return false;
}

bool if_die(Snake * snake)
{
	int l = snake->snake_head.head_line_pos;
	int c = snake->snake_head.head_column_pos;
	int i = 0;

	if (l == 0 || l == LINE-1 || c == 0 || c == COLUMN-1)
		return true;
	else
	{
		for (i = 0; i < snake->node_now; i++)
		{
			if (l == snake->snake_nodes[i].line_pos && c == snake->snake_nodes[i].column_pos)
				break;
		}
		if (i == snake->node_now)
			return false;
		else 
			return true;
	}
}
void pre_head_pos_get(Snake * snake, int * a)//更新ｈｅａｄ坐标前使用
{
	a[0] = snake->snake_head.head_line_pos;
	a[1] = snake->snake_head.head_column_pos;
}

void snake_head_move(Snake * snake)//更新ｈｅａｄ坐标
{
	int i = snake->snake_head.head_move_direction;
	
	if (i == 1)
		snake->snake_head.head_line_pos--;
	else if (i == 2)
		snake->snake_head.head_line_pos++;
	else if (i == 3)
		snake->snake_head.head_column_pos--;
	else if (i == 4)
		snake->snake_head.head_column_pos++;
}

void snake_node_move(Snake * snake, int * a)
{
	for (int i = snake->node_now-1; i > 0; i--)
	{
		snake->snake_nodes[i].line_pos = snake->snake_nodes[i-1].line_pos;
		snake->snake_nodes[i].column_pos = snake->snake_nodes[i-1].column_pos;
	}
	snake->snake_nodes[0].line_pos = a[0];
	snake->snake_nodes[0].column_pos = a[1];
}


