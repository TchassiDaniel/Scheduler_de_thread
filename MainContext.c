#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define NBRECONTEXT 2
#define TIMETHREAD 2
#define TIMESHEDULER 5

#define STACK_SIZE 4096 /* large enough value for AMODE 64 */

ucontext_t contextes[NBRECONTEXT];//Tableau qui va contenir les contextes à scheduler
int indiceContexte = 0;//Variable qui va contenir l'indice du dernier contexte executer ou en cours
ucontext_t scheduCont;//Variable qui va contenir le contexte du scheduler

//On va utilisder un signal alarme pour ordonnancer le scheduler
void handlerAlarm(int signal){
    swapcontext(&contextes[indiceContexte], &scheduCont);
}

void schedulerFonc(){

//On donne la main aux contextes infini pendant 10 secondes
    //Je ne trouve pas cela très efficace
//On ordonnance les contextes
while (1)
{
    indiceContexte = (indiceContexte + 1) % NBRECONTEXT;
    alarm(TIMESHEDULER);
    swapcontext(&scheduCont, &contextes[indiceContexte]);
}

}

void contextFonc(){
    //On recupère le nom du thread(son pid)
    int monNom = indiceContexte;
    //On defini le comportement du thread lors de la reception d'un signal SIGUSR1

    while(1){
        printf("Mon nom est: %d\n",monNom);
        sleep(TIMETHREAD);
    }
}

int main(){

    //On definit le siganl alarm
    signal(SIGALRM, handlerAlarm);

    //On cree les contextes en leur envoyant un numémros vomme leur nom
    for(int i = 0; i < NBRECONTEXT; i++){
        indiceContexte++;
        getcontext(&contextes[i]);//On l'initialise d'abord avec le contexte actuel
        contextes[i].uc_stack.ss_sp = malloc(STACK_SIZE);//On alloue la memoire pour la pile
        contextes[i].uc_stack.ss_size = STACK_SIZE;
        contextes[i].uc_stack.ss_flags = 0;
        makecontext(&contextes[i], contextFonc, 0, NULL);
    }

    //On cree le contexte du scheduler
    getcontext(&scheduCont);//On l'initialise d'abord avec le contexte actuel
    
    scheduCont.uc_stack.ss_sp = malloc(STACK_SIZE);//On alloue la mémoire pour la pile
    scheduCont.uc_stack.ss_size = STACK_SIZE;
    scheduCont.uc_stack.ss_flags = 0;
    
    makecontext(&scheduCont, schedulerFonc, 1, NULL);

    //On passe la main au scheduler
    setcontext(&scheduCont);

    return 0;
}