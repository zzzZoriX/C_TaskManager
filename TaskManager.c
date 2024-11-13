#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>
#include <time.h>

#define TASKF "tasks.dat"
#define COUNTF "count_task.dat"

int count;

typedef struct{

    int id;
    char desc[30];
    int date;
} Task;

int Select(char act, char *task);

int CreateTask(char *desc);
int Print();
int Clear();
int Delete();
int MoveTask();

void CheckingForFiles();

int *DifOnTaskDate(int userDate);

int *DecrDate(int date);
int EncrDate(int *pointDuserPointDatea);


int main() {

    CheckingForFiles();

    setlocale(LC_ALL, "ru_RU.UTF-8");

    int actionPointer = 0;

    char action[50];
    char task[30];

    while( 1 ){

        if( actionPointer == 48 ){ actionPointer = 0; }

        puts("Выберите действие:\n'a' - добавить\n'd' - удалить\n'c' - очистить\n'm' - переместить\n'i' - информация\n'p' - вывести\n'e' - выйти\n");

        printf("-> ");
        action[actionPointer] = getchar();

        while( getchar() != '\n' );

        if( action[actionPointer] == 'a'){

            printf("напишите задание: ");
            
            fgets(task, 30, stdin);

            int length = strlen(task);
            task[length-1] = '\0';
        }
        printf("\n");

        if( action[actionPointer] == 'e' ){ break; }

        Select(action[actionPointer], task);

        actionPointer++;

    }

    return 0;
}

int Select(char act, char *desc){

    switch( act ){

        case 'a':
            CreateTask(desc);
            break;
        
        case 'd':
            Delete();
            break;

        case 'c':
            Clear();
            break;
        
        case 'm':
            MoveTask();
            break;

        case 'p':
            Print();
            break;

        case 'i':
            puts("добавление - добавляет написаное вами таск в конец\nудаление - удаляет таск по выбранному вами номеру\nпереместить - перемещает таск под выбраным вами номером на номер, который вы указали вторым аргументом\nвывести - выводит все таски на консоль\n\nтаск не должен быть больше 30-ти букв, все что идет дальше тридцатой буквы - не учитывается при записи");
            break;

        default:
            puts("такое действие недоступно!");
            return 1;
            break;
    }

    return 0;
}

int CreateTask(char *desc){

    int *userPointDate = (int *)calloc(4, sizeof(int *));
    if( userPointDate == NULL ){

        perror("ошибка");

        free(userPointDate);

        return 1;
    }

    printf("введите кол-во дней, которые вы будете делать таск: ");
    scanf("%d", &userPointDate[0]);

    while( getchar() != '\n' );

    printf("введите кол-во часов, которые вы будете делать таск: ");
    scanf("%d", &userPointDate[1]);
    
    while( getchar() != '\n' );

    printf("введите кол-во минут, которые вы будете делать задание: ");
    scanf("%d", &userPointDate[2]);

    while( getchar() != '\n' );

    printf("введите кол-во секунд, которые вы будете делать задание: ");
    scanf("%d", &userPointDate[3]);

    while( getchar() != '\n' );

    if( userPointDate[0] < 0 || userPointDate[1] < 0 || userPointDate[2] < 0 || userPointDate[3] < 0 ){

        printf("вы ввели отрицательное число, дедлайн становится максимального времени\n");

        userPointDate[0] = 6;
        userPointDate[1] = 23;
        userPointDate[2] = 59;
        userPointDate[3] = 59;
    }
    
    FILE *fpt = fopen(TASKF, "ab"); //файл тасков
    FILE *fpcr = fopen(COUNTF, "rb"); //файл кол-ва тасков, откытый на чтение
    if( !fpcr || !fpt ){

        perror("ошибка");

        fclose(fpcr);
        fclose(fpt);

        return 1;
    }

    while( (fscanf(fpcr, "%d", &count)) != EOF ){ sleep(1); } //считывание тасков

    fclose(fpcr);

    //проверка кол-ва тасков
    if( count > 10){

        puts("кол-во тасков больше десяти!");

        fclose(fpt);

        return 1;
    }

    FILE *fpcw = fopen(COUNTF, "wb"); //файл кол-ва тасков, открытый на запись
    if( !fpcw ){

        perror("ошибка");

        fclose(fpcw);
        fclose(fpt);

        return 1;
    }

    count++;

    //формируем таск
    Task task;

    task.id = count;
    
    strncpy(task.desc, desc, sizeof(task.desc) - 1);
    task.desc[sizeof(task.desc) - 1] = '\0';

    task.date = EncrDate(userPointDate);

    if( userPointDate[3] >= 60 ){

        userPointDate[2] += userPointDate[3] / 60;
        userPointDate[3] %= 60;
    }
    if( userPointDate[2] >= 60 ){

        userPointDate[1] += userPointDate[2] / 60;
        userPointDate[2] %= 60;
    }
    if( userPointDate[1] >= 24 ){

        userPointDate[0] += userPointDate[1] / 24;
        userPointDate[1] %= 24;
    }

    puts("таск сохраняется...");

    sleep(1);

    fwrite(&task, sizeof(Task), 1, fpt); //запись таска

    fprintf(fpcw, "%d", count); //запись кол-ва тасков

    printf("таск-id: %d\nтаск: %s\nдедлайн: %d %d:%d:%d\n\n", task.id, task.desc, userPointDate[0], userPointDate[1], userPointDate[2], userPointDate[3]);

    fclose(fpt);
    fclose(fpcw);

    free(userPointDate);

    return 1;
}

int Print(){

    FILE *fpc = fopen(COUNTF, "rb");
    FILE *fpt = fopen(TASKF, "rb");
    if( !fpt || !fpc ){

        perror("ошибка: ");

        fclose(fpt);
        fclose(fpc);

        return 1;
    }

    fscanf(fpc, "%d", &count);

    fclose(fpc);

    //считывание тасков
    Task *tasks = calloc(count, sizeof(Task));
    if( tasks == NULL ){

        perror("ошибка");

        free(tasks);

        fclose(fpt);

        return 1;
    }

    for( int i = 0 ; i < count ; i++ ){

        if( fread(&tasks[i], sizeof(Task), 1, fpt) != 1 ){

            if( feof(fpt) ){

                break;
            }
            else{

                perror("ошибка");

                free(tasks);
                fclose(fpt);

                return 1;
            }
        }

    }

    fclose(fpt);

    for( int i = 0 ; i < count ; i++ ){

        int *dotd = DifOnTaskDate(tasks[i].date);
        printf("%d: %s - %d %d:%d:%d\n", tasks[i].id, tasks[i].desc, dotd[3], dotd[2], dotd[1], dotd[0]);
        free(dotd);
    }

    free(tasks);

    printf("\n");

    return 0;
}

int Clear(){

    FILE *fpt = fopen(TASKF, "wb"); //таски
    FILE *fpc = fopen(COUNTF, "wb"); //кол-во тасков
    if( !fpt || !fpc ){

        perror("ошибка:");

        fclose(fpt);
        fclose(fpc);

        return 1;
    }

    fclose(fpt);
    fclose(fpc);

    puts("таски успешно очищены!\n");

    return 0;
}

int Delete(){

    int DeleteTaskNumber;

    printf("напишите номер удаляемого таска: ");
    scanf("%d", &DeleteTaskNumber);

    while( getchar() != '\n' );

    FILE *fpcr = fopen(COUNTF, "rb");
    FILE *fptr = fopen(TASKF, "rb");
    if( !fpcr || !fptr ){

        perror("ошибка: ");

        fclose(fptr);
        fclose(fpcr);

        return 1;
    }

    fscanf(fpcr, "%d", &count);

    fclose(fpcr);

    if( count == 1 ){

        fclose(fptr);

        FILE *fpt = fopen(TASKF, "wb");
        fclose(fpt);

        return 1;
    }

    if( count < DeleteTaskNumber || DeleteTaskNumber < 0 ){

        puts("введен неправильный номер таска!");

        fclose(fptr);

        return 1;
    }

    Task *tasksWithoutDelete = calloc(count - 1, sizeof(Task));
    if( tasksWithoutDelete == NULL ){

        perror("ошибка");

        free(tasksWithoutDelete);
        fclose(fptr);

        return 1;
    }
    
    int rememberDeleteNumber = DeleteTaskNumber;

    for( int i = 0 ; i < count ; i++ ){


        if( DeleteTaskNumber == 1 ){

            if( fseek(fptr, sizeof(Task), SEEK_CUR) != 0 ){

                perror("ошибка");

                free(tasksWithoutDelete);
                fclose(fptr);

                return 1;
            }
        }

        if( fread(&tasksWithoutDelete[i], sizeof(Task), 1, fptr) != 1 ){

            if( feof(fptr) ){

                break;
            }
            else{

                perror("ошибка");

                free(tasksWithoutDelete);
                fclose(fptr);

                return 1;
            }
        }

        DeleteTaskNumber--;
    }

    fclose(fptr);
    count--;

    rememberDeleteNumber--;
    for( int i = rememberDeleteNumber ; i < count ; i++ ){

        tasksWithoutDelete[i].id = i + 1;
    }

    FILE *fptw = fopen(TASKF, "wb");
    FILE *fpcw = fopen(COUNTF, "wb");
    if( !fptw || !fpcw ){

        perror("ошибка");

        free(tasksWithoutDelete);
        fclose(fptw);
        fclose(fpcw);

        return 1;
    }

    for( int i = 0 ; i < count ; i++ ){

        fwrite(&tasksWithoutDelete[i], sizeof(Task), 1, fptw);
    }

    fprintf(fpcw, "%d", count);

    puts("таск успешно удален!\n");

    free(tasksWithoutDelete);
    fclose(fptw);
    fclose(fpcw);

    return 0;
}

int MoveTask(){

    int MoveTaskNumber, SwapTaskNumber;

    printf("напишите номер премещаемого таска: ");
    scanf("%d", &MoveTaskNumber);

    while( getchar() != '\n' );

    printf("напиште номер таска, c которым вы хотите поменять местами: ");
    scanf("%d", &SwapTaskNumber);

    while( getchar() != '\n' );

    FILE *fptr = fopen(TASKF, "rb");
    FILE *fpc = fopen(COUNTF, "rb");
    if( !fptr || !fpc ){

        perror("ошибка");

        fclose(fpc);
        fclose(fptr);

        return 1;
    }

    fscanf(fpc, "%d", &count);

    fclose(fpc);

    if( SwapTaskNumber > count ){

        SwapTaskNumber = count;
    }
    if( MoveTaskNumber > count || MoveTaskNumber <= 0 ){

        SwapTaskNumber = count;
    }
    if( SwapTaskNumber == MoveTaskNumber ){

        puts("вы ввели одинаковые номера");

        fclose(fptr);
        
        return 0;
    }

    Task *movedTasks = calloc(count, sizeof(Task));
    if( movedTasks == NULL ){

        perror("ошибка");

        free(movedTasks);
        fclose(fptr);

        return 1;
    }

    MoveTaskNumber--;
    SwapTaskNumber--;

    for( int i = 0 ; i < count ; i++ ){

        if( i == MoveTaskNumber ){

            if( fread(&movedTasks[SwapTaskNumber], sizeof(Task), 1, fptr) != 1 ){

                perror("ошибка");

                free(movedTasks);
                fclose(fptr);

                return 1;
            }

            continue;
        }
        else if( i == SwapTaskNumber ){

            if( fread(&movedTasks[MoveTaskNumber], sizeof(Task), 1, fptr) != 1 ){

                perror("ошибка");

                free(movedTasks);
                fclose(fptr);

                return 1;
            }

            continue;
        }

        if( fread(&movedTasks[i], sizeof(Task), 1, fptr) != 1 ){

            if( feof(fptr) ){

                break;
            }
            else{

                perror("ошибка");

                free(movedTasks);
                fclose(fptr);

                return 1;
            }
        }
    }

    fclose(fptr);
    
    for( int i = 0 ; i < count ; i++ ){

        movedTasks[i].id = i + 1;
    }
    
    FILE *fptw = fopen(TASKF, "wb");
    if( !fptw ){

        perror("ошибка");

        free(movedTasks);
        fclose(fptw);

        return 1;
    }

    for( int i = 0 ; i < count ; i++ ){

        fwrite(&movedTasks[i], sizeof(Task), 1, fptw);
    }

    puts("таск успешно перемещен!\n");

    free(movedTasks);
    fclose(fptw);

    return 0;
}

void CheckingForFiles(){

    FILE *fpt = fopen(TASKF, "rb");
    FILE *fpc = fopen(COUNTF, "rb");
    
    if( !fpt ){

        fclose(fpt);
        FILE *fptc = fopen(TASKF, "wb");
        fclose(fptc);
    }
    if( !fpc ){

        fclose(fpc);
        FILE *fpcc = fopen(COUNTF, "wb");
        fclose(fpcc);
    }
    
    fclose(fpt);
    fclose(fpc);
}

int *DifOnTaskDate(int userDate){

    int *dotd = (int *)calloc(4, sizeof(int *));
    int *udd = (int *)calloc(4, sizeof(int *));
    if( dotd == NULL || udd == NULL ){

        free(dotd);
        free(udd);

        perror("ошибка");

        return NULL;
    }

    time_t timeVar = time(NULL);
    struct tm *now = localtime(&timeVar);

    udd = DecrDate(userDate);

    if( udd[3] < now->tm_mday ){

        for(int i = 0; i < 4; i++){ dotd[i] = 0; return dotd; }
    }
    else if( udd[3] == now->tm_mday ){

        if( udd[2] < now->tm_hour ){

            for(int i = 0; i < 4; i++){ dotd[i] = 0; return dotd; }
        }
        else if( udd[2] == now->tm_hour ){

            if(udd[1] < now->tm_min ){

                for(int i = 0; i < 4; i++){ dotd[i] = 0; return dotd; }
            }
            else if( udd[1] == now->tm_min ){

                if( udd[0] < now->tm_sec ){

                    for(int i = 0; i < 4; i++){ dotd[i] = 0; return dotd; }
                }
            }
        }
    }

    dotd[3] = udd[3] - now->tm_mday;
    dotd[2] = udd[2] - now->tm_hour;
    dotd[1] = udd[1] - now->tm_min;
    dotd[0] = udd[0] - now->tm_sec;
    if( dotd[0] < 0 ){ dotd[0] = 0; }

    return dotd;
}

int *DecrDate(int date){

    int *err = (int *)1;

    int *pointDateDecr = (int *)calloc(4, sizeof(int *));

    if( pointDateDecr == NULL ){

        perror("ошибка!\n");

        free(pointDateDecr);

        return err;
    }

    pointDateDecr[0] = date % 100;
    date /= 100;

    pointDateDecr[1] = date % 100;
    date /= 100;

    pointDateDecr[2] = date % 100;
    date /= 100;

    pointDateDecr[3] = date;

    return pointDateDecr;
}

int EncrDate(int *pointDate){

    int pointDateEncr;
    time_t timeVar = time(NULL);
    struct tm *now = localtime(&timeVar);

    pointDate[0] += now->tm_mday;
    pointDate[1] += now->tm_hour;
    pointDate[2] += now->tm_min;
    pointDate[3] += now->tm_sec;

    pointDateEncr = pointDate[0] * 100 + pointDate[1];
    pointDateEncr = pointDateEncr * 100 + pointDate[2];
    pointDateEncr = pointDateEncr * 100 + pointDate[3];

    return pointDateEncr;
}